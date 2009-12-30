/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "actionthread.moc"

// C++ includes

#include <cmath>

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtDebug>
#include <QDateTime>
#include <QFileInfo>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ktempdir.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "kpwriteimage.h"
#include "pluginsversion.h"

namespace KIPIExpoBlendingPlugin
{

class ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        align               = true;
        cancel              = false;
        enfuseProcess       = 0;
        alignProcess        = 0;
        preprocessingTmpDir = 0;
    }

    class Task
    {
        public:

            bool                             align;
            KUrl::List                       urls;
            KUrl                             outputUrl;
            Action                           action;
            SaveSettingsWidget::OutputFormat outputFormat;
            RawDecodingSettings              rawDecodingSettings;
            EnfuseSettings                   enfuseSettings;
    };

    bool                             cancel;
    bool                             align;

    QMutex                           mutex;

    QWaitCondition                   condVar;

    QList<Task*>                     todo;

    KProcess*                        enfuseProcess;
    KProcess*                        alignProcess;

    KDcraw                           rawdec;

    KTempDir*                        preprocessingTmpDir;

    /**
     * List of results files produced by enfuse that may need cleaning.
     * Only access this through the provided mutex.
     */
    KUrl::List                       enfuseTmpUrls;
    QMutex                           enfuseTmpUrlsMutex;

    SaveSettingsWidget::OutputFormat outputFormat;

    RawDecodingSettings              rawDecodingSettings;

    EnfuseSettings                   enfuseSettings;

    void cleanAlignTmpDir()
    {
        if (preprocessingTmpDir)
        {
            preprocessingTmpDir->unlink();
            delete preprocessingTmpDir;
            preprocessingTmpDir = 0;
        }
    }
};

ActionThread::ActionThread(QObject* parent)
            : QThread(parent), d(new ActionThreadPriv)
{
    qRegisterMetaType<ActionData>();
}

ActionThread::~ActionThread()
{

    kDebug() << "ActionThread shutting down."
             << "Canceling all actions and waiting for them";

    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    kDebug() << "Thread finished";

    d->cleanAlignTmpDir();

    cleanUpResultFiles();

    delete d;
}

void ActionThread::cleanUpResultFiles()
{
    // Cleanup all tmp files created by Enfuse process.
    QMutexLocker(&d->enfuseTmpUrlsMutex);
    foreach(const KUrl url, d->enfuseTmpUrls)
    {
        kDebug() << "Removing temp file " << url.toLocalFile();
        KTempDir::removeDir(url.toLocalFile());
    }
    d->enfuseTmpUrls.clear();
}

void ActionThread::setPreProcessingSettings(bool align, const RawDecodingSettings& settings)
{
    d->align               = align;
    d->rawDecodingSettings = settings;
}

void ActionThread::setEnfuseSettings(const EnfuseSettings& settings, SaveSettingsWidget::OutputFormat frmt)
{
    d->enfuseSettings = settings;
    d->outputFormat   = frmt;
}

void ActionThread::identifyFiles(const KUrl::List& urlList)
{
    foreach(const KUrl url, urlList)
    {
        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->action                 = IDENTIFY;
        t->urls.append(url);

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
    }
}

void ActionThread::loadProcessed(const KUrl& url)
{
    ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
    t->action                 = LOAD;
    t->urls.append(url);

    QMutexLocker lock(&d->mutex);
    d->todo << t;
    d->condVar.wakeAll();
}

void ActionThread::preProcessFiles(const KUrl::List& urlList)
{
    ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
    t->action                 = PREPROCESSING;
    t->urls                   = urlList;
    t->rawDecodingSettings    = d->rawDecodingSettings;
    t->align                  = d->align;

    QMutexLocker lock(&d->mutex);
    d->todo << t;
    d->condVar.wakeAll();
}

void ActionThread::enfuseFiles(const KUrl::List& alignedUrls, const KUrl& outputUrl)
{
    ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
    t->action                 = ENFUSE;
    t->urls                   = alignedUrls;
    t->outputUrl              = outputUrl;
    t->outputFormat           = d->outputFormat;
    t->enfuseSettings         = d->enfuseSettings;

    QMutexLocker lock(&d->mutex);
    d->todo << t;
    d->condVar.wakeAll();
}

void ActionThread::cancel()
{
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->cancel = true;

    if (d->enfuseProcess)
        d->enfuseProcess->kill();

    if (d->alignProcess)
        d->alignProcess->kill();

    d->rawdec.cancel();

    d->condVar.wakeAll();
}

void ActionThread::run()
{
    d->cancel = false;
    while (!d->cancel)
    {
        ActionThreadPriv::Task *t = 0;
        {
            QMutexLocker lock(&d->mutex);
            if (!d->todo.isEmpty())
                t = d->todo.takeFirst();
            else
                d->condVar.wait(&d->mutex);
        }

        if (t)
        {
            switch (t->action)
            {
                case IDENTIFY:
                {
                    // Identify Exposure.

                    QString avLum;

                    if (!t->urls.isEmpty())
                    {
                        float val = getAverageSceneLuminance(t->urls[0].toLocalFile());
                        if (val != -1)
                            avLum.setNum(log2f(val), 'g', 2);
                    }

                    ActionData ad;
                    ad.action  = t->action;
                    ad.inUrls  = t->urls;
                    ad.message = avLum.isEmpty() ? i18n("unknown") : avLum;
                    ad.success = avLum.isEmpty();
                    emit finished(ad);
                    break;
                }

                case PREPROCESSING:
                {
                    ActionData ad1;
                    ad1.action   = PREPROCESSING;
                    ad1.inUrls   = t->urls;
                    ad1.starting = true;
                    emit starting(ad1);

                    ItemUrlsMap alignedUrlsMap;
                    QString     errors;

                    bool result  = startPreProcessing(t->urls, alignedUrlsMap, t->align, t->rawDecodingSettings, errors);

                    ActionData ad2;
                    ad2.action         = PREPROCESSING;
                    ad2.inUrls         = t->urls;
                    ad2.alignedUrlsMap = alignedUrlsMap;
                    ad2.success        = result;
                    ad2.message        = errors;
                    emit finished(ad2);
                    break;
                }

                case LOAD:
                {
                    ActionData ad1;
                    ad1.action   = LOAD;
                    ad1.inUrls   = t->urls;
                    ad1.starting = true;
                    emit starting(ad1);

                    QImage image;
                    bool result  = image.load(t->urls[0].toLocalFile());

                    // rotate image
                    if (result)
                    {
                        KExiv2 meta(t->urls[0].toLocalFile());
                        meta.rotateExifQImage(image, meta.getImageOrientation());
                    }

                    ActionData ad2;
                    ad2.action         = LOAD;
                    ad2.inUrls         = t->urls;
                    ad2.success        = result;
                    ad2.image          = image;
                    emit finished(ad2);
                    break;
                }

                case ENFUSE:
                {
                    ActionData ad1;
                    ad1.action   = ENFUSE;
                    ad1.inUrls   = t->urls;
                    ad1.starting = true;
                    emit starting(ad1);

                    KUrl    destUrl = t->outputUrl;
                    bool    result  = false;
                    QString errors;

                    result = startEnfuse(t->urls, destUrl, t->enfuseSettings, t->outputFormat, errors);

                    // We will take first image metadata from stack to restore Exif, Iptc, and Xmp.
                    KExiv2 meta;
                    meta.load(t->urls[0].toLocalFile());
                    meta.setXmpTagString("Xmp.kipi.EnfuseInputFiles", t->enfuseSettings.inputImagesList(), false);
                    meta.setXmpTagString("Xmp.kipi.EnfuseSettings", t->enfuseSettings.asCommentString().replace("\n", " ; "), false);
                    meta.setImageDateTime(QDateTime::currentDateTime());
                    if (t->outputFormat != SaveSettingsWidget::OUTPUT_JPEG)
                    {
                        QImage img;
                        if (img.load(destUrl.toLocalFile()))
                            meta.setImagePreview(img.scaled(1280, 1024, Qt::KeepAspectRatio));
                    }
                    meta.save(destUrl.toLocalFile());

                    // To be cleaned in destructor.
                    {
                        QMutexLocker(&d->enfuseTmpUrlsMutex);
                        d->enfuseTmpUrls << destUrl;
                    }

                    ActionData ad2;
                    ad2.action         = ENFUSE;
                    ad2.inUrls         = t->urls;
                    ad2.outUrls        = KUrl::List() << destUrl;
                    ad2.success        = result;
                    ad2.message        = errors;
                    ad2.enfuseSettings = t->enfuseSettings;
                    emit finished(ad2);
                    break;
                }

                default:
                {
                    qCritical() << "Unknown action specified" << endl;
                }
            }
        }

        delete t;
    }
}

bool ActionThread::startPreProcessing(const KUrl::List& inUrls, ItemUrlsMap& preProcessedUrlsMap,
                                      bool align, const RawDecodingSettings& settings,
                                      QString& errors)
{
    QString prefix = KStandardDirs::locateLocal("tmp", QString("kipi-expoblending-preprocessing-tmp-") +
                                                       QString::number(QDateTime::currentDateTime().toTime_t()));


    d->cleanAlignTmpDir();

    d->preprocessingTmpDir = new KTempDir(prefix);

    // Pre-process RAW files if necessary.

    KUrl::List mixedUrls;     // Original non-RAW + Raw converted urls to align.

    foreach(const KUrl url, inUrls)
    {
        if (isRawFile(url.toLocalFile()))
        {
            KUrl outUrl;

            if (!convertRaw(url, outUrl, settings))
                return false;

            mixedUrls.append(outUrl);
            // In case of alignment is not performed.
            preProcessedUrlsMap.insert(url, outUrl);
        }
        else
        {
            mixedUrls.append(url);
            // In case of alignment is not performed.
            preProcessedUrlsMap.insert(url, url);
        }
    }

    if (align)
    {
        // Re-align images

        d->alignProcess = new KProcess;
        d->alignProcess->clearProgram();
        d->alignProcess->clearEnvironment();
        d->alignProcess->setWorkingDirectory(d->preprocessingTmpDir->name());
        d->alignProcess->setOutputChannelMode(KProcess::MergedChannels);

        QStringList args;
        args << "align_image_stack";
        args << "-v";
        args << "-a";
        args << "aligned";

        foreach(const KUrl url, mixedUrls)
        {
            args << url.toLocalFile();
        }

        d->alignProcess->setProgram(args);

        kDebug() << "Align command line: " << d->alignProcess->program();

        d->alignProcess->start();

        if (!d->alignProcess->waitForFinished(-1))
        {
            errors = getProcessError(d->alignProcess);
            return false;
        }

        uint    i=0;
        QString temp;
        preProcessedUrlsMap.clear();

        foreach(const KUrl url, inUrls)
        {
            preProcessedUrlsMap.insert(url, KUrl(d->preprocessingTmpDir->name() + temp.sprintf("aligned%04i", i) + QString(".tif")));
            i++;
        }

        kDebug() << "Pre-processed output urls map: " << preProcessedUrlsMap;
        kDebug() << "Align exit status    : "         << d->alignProcess->exitStatus();
        kDebug() << "Align exit code      : "         << d->alignProcess->exitCode();

        if (d->alignProcess->exitStatus() != QProcess::NormalExit)
            return false;

        if (d->alignProcess->exitCode() == 0)
        {
            // Process finished successfully !
            return true;
        }

        errors = getProcessError(d->alignProcess);
        return false;
    }
    else
    {
        kDebug() << "Pre-processed output urls map: " << preProcessedUrlsMap;
        kDebug() << "Alignment not performed.";
        return true;
    }
}

bool ActionThread::convertRaw(const KUrl& inUrl, KUrl& outUrl, const RawDecodingSettings& settings)
{
    int        width, height, rgbmax;
    QByteArray imageData;

    if (d->rawdec.decodeRAWImage(inUrl.toLocalFile(), settings, imageData, width, height, rgbmax))
    {
        uchar* sptr  = (uchar*)imageData.data();
        float factor = 65535.0 / rgbmax;
        unsigned short tmp16[3];

        // Set RGB color components.
        for (int i = 0 ; !d->cancel && (i < width * height) ; i++)
        {
            // Swap Red and Blue and re-ajust color component values
            tmp16[0] = (unsigned short)((sptr[5]*256 + sptr[4]) * factor);      // Blue
            tmp16[1] = (unsigned short)((sptr[3]*256 + sptr[2]) * factor);      // Green
            tmp16[2] = (unsigned short)((sptr[1]*256 + sptr[0]) * factor);      // Red
            memcpy(&sptr[0], &tmp16[0], 6);
            sptr += 6;
        }

        KExiv2 meta;
        meta.load(inUrl.toLocalFile());
        meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
        meta.setImageDimensions(QSize(width, height));
        meta.setExifTagString("Exif.Image.DocumentName", inUrl.fileName());
        meta.setXmpTagString("Xmp.tiff.Make",  meta.getExifTagString("Exif.Image.Make"));
        meta.setXmpTagString("Xmp.tiff.Model", meta.getExifTagString("Exif.Image.Model"));
        meta.setImageOrientation(KExiv2Iface::KExiv2::ORIENTATION_NORMAL);

        QByteArray prof = KPWriteImage::getICCProfilFromFile(settings.outputColorSpace);

        KPWriteImage wImageIface;
        wImageIface.setCancel(&d->cancel);
        wImageIface.setImageData(imageData, width, height, true, false, prof, meta);
        outUrl = d->preprocessingTmpDir->name();
        QFileInfo fi(inUrl.toLocalFile());
        outUrl.setFileName(QString(".") + fi.completeBaseName().replace(".", "_") + QString(".tif"));

        if (!wImageIface.write2TIFF(outUrl.toLocalFile()))
            return false;
    }
    else
    {
        return false;
    }

    kDebug() << "Convert RAW output url: "  << outUrl;

    return true;
}

bool ActionThread::isRawFile(const KUrl& url)
{
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());

    QFileInfo fileInfo(url.toLocalFile());
    if (rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper()))
        return true;

    return false;
}

bool ActionThread::startEnfuse(const KUrl::List& inUrls, KUrl& outUrl,
                               const EnfuseSettings& enfuseSettings,
                               SaveSettingsWidget::OutputFormat frmt,
                               QString& errors)
{
    QString comp;
    QString ext;

    switch(frmt)
    {
        case SaveSettingsWidget::OUTPUT_JPEG:
            ext = ".jpg";
            break;
        case SaveSettingsWidget::OUTPUT_TIFF:
            ext  = ".tif";
            comp = "--compression=DEFLATE";
            break;
        case SaveSettingsWidget::OUTPUT_PPM:
            ext = ".ppm";
            break;
        case SaveSettingsWidget::OUTPUT_PNG:
            ext = ".png";
            break;
    }

    outUrl.setFileName(QString(".kipi-expoblending-tmp-") + QString::number(QDateTime::currentDateTime().toTime_t()) + ext);

    d->enfuseProcess = new KProcess;
    d->enfuseProcess->clearProgram();
    d->enfuseProcess->clearEnvironment();
    d->enfuseProcess->setOutputChannelMode(KProcess::MergedChannels);
    QStringList args;
    args << "enfuse";

    if (!enfuseSettings.autoLevels)
    {
        args << "-l";
        args << QString::number(enfuseSettings.levels);
    }

    if (enfuseSettings.ciecam02)
        args << "-c";

    if (!comp.isEmpty())
        args << comp;

    if (enfuseSettings.hardMask)
        args << "--HardMask";

    args << QString("--wExposure=%1").arg(enfuseSettings.exposure);
    args << QString("--wSaturation=%1").arg(enfuseSettings.saturation);
    args << QString("--wContrast=%1").arg(enfuseSettings.contrast);

    args << "-v";
    args << "-o";
    args << outUrl.toLocalFile();

    foreach(const KUrl url, inUrls)
        args << url.toLocalFile();

    d->enfuseProcess->setProgram(args);

    kDebug() << "Enfuse command line: " << d->enfuseProcess->program();

    d->enfuseProcess->start();

    if (!d->enfuseProcess->waitForFinished(-1))
    {
        errors = getProcessError(d->enfuseProcess);
        return false;
    }

    kDebug() << "Enfuse output url: "  << outUrl;
    kDebug() << "Enfuse exit status: " << d->enfuseProcess->exitStatus();
    kDebug() << "Enfuse exit code:   " << d->enfuseProcess->exitCode();

    if (d->enfuseProcess->exitStatus() != QProcess::NormalExit)
        return false;

    if (d->enfuseProcess->exitCode() == 0)
    {
        // Process finished successfully !
        return true;
    }

    errors = getProcessError(d->enfuseProcess);
    return false;
}

QString ActionThread::getProcessError(KProcess* proc) const
{
    if (!proc) return QString();

    QString std = proc->readAll();
    return (i18n("Cannot run %1 :\n\n %2", proc->program()[0], std));
}

/**
 * This function obtains the "average scene luminance" (cd/m^2) from an image file.
 * "average scene luminance" is the L (aka B) value mentioned in [1]
 * You have to take a log2f of the returned value to get an EV value.
 *
 * We are using K=12.07488f and the exif-implied value of N=1/3.125 (see [1]).
 * K=12.07488f is the 1.0592f * 11.4f value in pfscalibration's pfshdrcalibrate.cpp file.
 * Based on [3] we can say that the value can also be 12.5 or even 14.
 * Another reference for APEX is [4] where N is 0.3, closer to the APEX specification of 2^(-7/4)=0.2973.
 *
 * [1] http://en.wikipedia.org/wiki/APEX_system
 * [2] http://en.wikipedia.org/wiki/Exposure_value
 * [3] http://en.wikipedia.org/wiki/Light_meter
 * [4] http://doug.kerr.home.att.net/pumpkin/#APEX
 *
 * This function tries first to obtain the shutter speed from either of
 * two exif tags (there is no standard between camera manifacturers):
 * ExposureTime or ShutterSpeedValue.
 * Same thing for f-number: it can be found in FNumber or in ApertureValue.
 *
 * F-number and shutter speed are mandatory in exif data for EV calculation, iso is not.
 */
float ActionThread::getAverageSceneLuminance(const KUrl& url)
{
    KExiv2 meta;
    meta.load(url.toLocalFile());
    if (!meta.hasExif())
        return -1;

    long num = 1, den = 1;

    // default not valid values

    float    expo = -1.0;
    float    iso  = -1.0;
    float    fnum = -1.0;
    QVariant rationals;

    if (meta.getExifTagRational("Exif.Photo.ExposureTime", num, den))
    {
        if (den)
            expo = (float)(num) / (float)(den);
    }
    else if (getXmpRational("Xmp.exif.ExposureTime", num, den, meta))
    {
        if (den)
            expo = (float)(num) / (float)(den);
    }
    else if (meta.getExifTagRational("Exif.Photo.ShutterSpeedValue", num, den))
    {
        long   nmr = 1, div = 1;
        double tmp = 0.0;

        if (den)
            tmp = exp(log(2.0) * (float)(num) / (float)(den));

        if (tmp > 1.0)
        {
            div = (long)(tmp + 0.5);
        }
        else
        {
            nmr = (long)(1 / tmp + 0.5);
        }

        if (div)
            expo = (float)(nmr) / (float)(div);
    }
    else if (getXmpRational("Xmp.exif.ShutterSpeedValue", num, den, meta))
    {
        long   nmr = 1, div = 1;
        double tmp = 0.0;

        if (den)
            tmp = exp(log(2.0) * (float)(num) / (float)(den));

        if (tmp > 1.0)
        {
            div = (long)(tmp + 0.5);
        }
        else
        {
            nmr = (long)(1 / tmp + 0.5);
        }

        if (div)
            expo = (float)(nmr) / (float)(div);
    }

    kDebug() << url.fileName() << " : expo = " << expo;

    if (meta.getExifTagRational("Exif.Photo.FNumber", num, den))
    {
        if (den)
            fnum = (float)(num) / (float)(den);
    }
    else if (getXmpRational("Xmp.exif.FNumber", num, den, meta))
    {
        if (den)
            fnum = (float)(num) / (float)(den);
    }
    else if (meta.getExifTagRational("Exif.Photo.ApertureValue", num, den))
    {
        if (den)
            fnum = (float)(exp(log(2.0) * (float)(num) / (float)(den) / 2.0));
    }
    else if (getXmpRational("Xmp.exif.ApertureValue", num, den, meta))
    {
        if (den)
            fnum = (float)(exp(log(2.0) * (float)(num) / (float)(den) / 2.0));
    }

    kDebug() << url.fileName() << " : fnum = " << fnum;

    // Some cameras/lens DO print the fnum but with value 0, and this is not allowed for ev computation purposes.

    if (fnum == 0.0)
        return -1.0;

    // If iso is found use that value, otherwise assume a value of iso=100. (again, some cameras do not print iso in exif).

    if (meta.getExifTagRational("Exif.Photo.ISOSpeedRatings", num, den))
    {
        if (den)
            iso = (float)(num) / (float)(den);
    }
    else if (getXmpRational("Xmp.exif.ISOSpeedRatings", num, den, meta))
    {
        if (den)
            iso = (float)(num) / (float)(den);
    }
    else
    {
        iso = 100.0;
    }

    kDebug() << url.fileName() << " : iso = " << iso;

    // At this point the three variables have to be != -1

    if (expo != -1.0 && iso != -1.0 && fnum != -1.0)
    {
        float asl = (expo * iso) / (fnum * fnum * 12.07488f);
        kDebug() << url.fileName() << " : ASL ==> " << asl;

        return asl;
    }

    return -1.0;
}

bool ActionThread::getXmpRational(const char* xmpTagName, long& num, long& den, KExiv2& meta)
{
    QVariant rationals = meta.getXmpTagVariant(xmpTagName);

    if (!rationals.isNull())
    {
        QVariantList list = rationals.toList();

        if (list.size() == 2)
        {
            num = list[0].toInt();
            den = list[1].toInt();

            return true;
        }
    }

    return false;
}

}  // namespace KIPIExpoBlendingPlugin
