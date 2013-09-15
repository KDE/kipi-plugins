/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2013 by Soumajyoti Sarkar <ergy dot ergy at gmail dot com>
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

#include "preprocesstask.moc"

// Qt includes

#include <QFileInfo>

// KDE includes

#include <klocale.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <ktempdir.h>
// Local includes

#include "kpmetadata.h"
#include "kpversion.h"
#include "kpwriteimage.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

PreProcessTask::PreProcessTask(QObject* const parent, const KUrl::List& inUrls, 
			       const RawDecodingSettings& rawSettings, const bool align,
			       const QString& alignPath)
    : Task(parent, PREPROCESSING, inUrls), urls(inUrls), settings(rawSettings),
      align(align), binaryPath(alignPath)
{}

PreProcessTask::PreProcessTask(const KUrl::List& inUrls, 
			       const RawDecodingSettings& rawSettings, const bool align,
			       const QString& alignPath)
    : Task(0, PREPROCESSING, inUrls), urls(inUrls), settings(rawSettings),
      align(align), binaryPath(alignPath)
{}

PreProcessTask::~PreProcessTask()
{}

void PreProcessTask::run()
{
    ActionData ad1;
    ad1.action   = PREPROCESSING;
    ad1.inUrls   = urls;
    ad1.starting = true;
    emit starting(ad1);

    ItemUrlsMap preProcessedUrlsMap;
    QString     errors;

    cancel = false;
    bool result  = startPreProcessing(urls, preProcessedUrlsMap, align, settings, binaryPath, errors);

    ActionData ad2;
    ad2.action              = PREPROCESSING;
    ad2.inUrls              = urls;
    ad2.preProcessedUrlsMap = preProcessedUrlsMap;
    ad2.success             = result;
    ad2.message             = errors;
    emit finished(ad2);
    
}

bool PreProcessTask::startPreProcessing(const KUrl::List& inUrls, ItemUrlsMap& preProcessedUrlsMap,
                                      bool align, const RawDecodingSettings& settings,
                                      const QString& alignPath, QString& errors)
{
    QString prefix = KStandardDirs::locateLocal("tmp", QString("kipi-expoblending-preprocessing-tmp-") +
                                                       QString::number(QDateTime::currentDateTime().toTime_t()));

    //cleanAlignTmpDir();

    preprocessingTmpDir = new KTempDir(prefix);

    volatile bool error = false;

    KUrl::List mixedUrls;
    
    for (int i = 0; i < inUrls.size(); ++i)
    {

        if (error)
        {
            continue;
        }

        KUrl url = inUrls.at(i);

        if (KPMetadata::isRawFile(url.toLocalFile()))
        {
            KUrl preprocessedUrl, previewUrl;

            if (!convertRaw(url, preprocessedUrl, settings))
            {
		successFlag = false;
                error = true;
                continue;
            }

            if (!computePreview(preprocessedUrl, previewUrl))
            {
	        successFlag = false;
                error = true;
                continue;
            }
            mixedUrls.append(preprocessedUrl);
            // In case of alignment is not performed.
            preProcessedUrlsMap.insert(url, ItemPreprocessedUrls(preprocessedUrl, previewUrl));


        }
        else
        {
            // NOTE: in this case, preprocessed Url is original file Url.
            KUrl previewUrl;
            if (!computePreview(url, previewUrl))
            {
	        successFlag = false;
                error = true;
                continue;
            }
            mixedUrls.append(url);
            // In case of alignment is not performed.
            preProcessedUrlsMap.insert(url, ItemPreprocessedUrls(url, previewUrl));

	}
    }

    if (error)
    {
        successFlag = false;
        return false;
    }

    if (align)
    {
        // Re-align images
        alignProcess = new KProcess;
        alignProcess->clearProgram();
        alignProcess->setWorkingDirectory(preprocessingTmpDir->name());
        alignProcess->setOutputChannelMode(KProcess::MergedChannels);

        QStringList args;
        args << alignPath;
        args << "-v";
        args << "-a";
        args << "aligned";

	foreach(const KUrl& url, mixedUrls)
        {
            args << url.toLocalFile();
        }
        
        alignProcess->setProgram(args);

        kDebug() << "Align command line: " << alignProcess->program();

        alignProcess->start();

        if (!alignProcess->waitForFinished(-1))
        {
	    successFlag = false;  
            errors = getProcessError(alignProcess);
            return false;
        }

        uint    i=0;
        QString temp;
        preProcessedUrlsMap.clear();

        foreach(const KUrl& url, inUrls)
        {
            KUrl previewUrl;
            KUrl alignedUrl = KUrl(preprocessingTmpDir->name() + temp.sprintf("aligned%04i", i) + QString(".tif"));
            if (!computePreview(alignedUrl, previewUrl))
	    {
	        successFlag = false;
                return false;
	    }
            preProcessedUrlsMap.insert(url, ItemPreprocessedUrls(alignedUrl, previewUrl));
            i++;
        }

        for (QMap<KUrl, ItemPreprocessedUrls>::const_iterator it = preProcessedUrlsMap.constBegin() ; it != preProcessedUrlsMap.constEnd(); ++it)
        {
            kDebug() << "Pre-processed output urls map: " << it.key() << " , "
                                                          << it.value().preprocessedUrl << " , "
                                                          << it.value().previewUrl << " ; ";
        }
        kDebug() << "Align exit status    : "         << alignProcess->exitStatus();
        kDebug() << "Align exit code      : "         << alignProcess->exitCode();

        if (alignProcess->exitStatus() != QProcess::NormalExit)
	{
	    successFlag = false;
            return false;
	}

        if (alignProcess->exitCode() == 0)
        {
            // Process finished successfully !
            return true;
        }

        errors = getProcessError(alignProcess);
	successFlag = false;
        return false;
    }
    else
    {
        for (QMap<KUrl, ItemPreprocessedUrls>::const_iterator it = preProcessedUrlsMap.constBegin() ; it != preProcessedUrlsMap.constEnd(); ++it)
        {
            kDebug() << "Pre-processed output urls map: " << it.key() << " , "
                                                          << it.value().preprocessedUrl << " , "
                                                          << it.value().previewUrl << " ; ";
        }
        kDebug() << "Alignment not performed.";
        return true;
    }
}

void PreProcessTask::cleanAlignTmpDir()
{
    if (preprocessingTmpDir)
    {
	preprocessingTmpDir->unlink();
        delete preprocessingTmpDir;
        preprocessingTmpDir = 0;
    }
}

bool PreProcessTask::computePreview(const KUrl& inUrl, KUrl& outUrl)
{
    outUrl = preprocessingTmpDir->name();
    QFileInfo fi(inUrl.toLocalFile());
    outUrl.setFileName(QString(".") + fi.completeBaseName().replace('.', '_') + QString("-preview.jpg"));

    QImage img;
    if (img.load(inUrl.toLocalFile()))
    {
        QImage preview = img.scaled(1280, 1024, Qt::KeepAspectRatio);
        bool saved     = preview.save(outUrl.toLocalFile(), "JPEG");
        // save exif information also to preview image for auto rotation
        if (saved)
        {
            KPMetadata metaIn(inUrl.toLocalFile());
            KPMetadata metaOut(outUrl.toLocalFile());
            metaOut.setImageOrientation(metaIn.getImageOrientation());
            metaOut.applyChanges();
        }
        kDebug() << "Preview Image url: " << outUrl << ", saved: " << saved;
        return saved;
    }
    return false;
}

bool PreProcessTask::convertRaw(const KUrl& inUrl, KUrl& outUrl, const RawDecodingSettings& settings)
{
    int        width, height, rgbmax;
    QByteArray imageData;

    QPointer<KDcraw> rawdec = new KDcraw;

    bool decoded = rawdec->decodeRAWImage(inUrl.toLocalFile(), settings, imageData, width, height, rgbmax);

    if (decoded)
    {
        uchar* sptr  = (uchar*)imageData.data();
        float factor = 65535.0 / rgbmax;
        unsigned short tmp16[3];

        // Set RGB color components.
        for (int i = 0 ; !cancel && (i < width * height) ; ++i)
        {
            // Swap Red and Blue and re-ajust color component values
            tmp16[0] = (unsigned short)((sptr[5]*256 + sptr[4]) * factor);      // Blue
            tmp16[1] = (unsigned short)((sptr[3]*256 + sptr[2]) * factor);      // Green
            tmp16[2] = (unsigned short)((sptr[1]*256 + sptr[0]) * factor);      // Red
            memcpy(&sptr[0], &tmp16[0], 6);
            sptr += 6;
        }

        KPMetadata meta;
        meta.load(inUrl.toLocalFile());
        meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
        meta.setImageDimensions(QSize(width, height));
        meta.setExifTagString("Exif.Image.DocumentName", inUrl.fileName());
        meta.setXmpTagString("Xmp.tiff.Make",  meta.getExifTagString("Exif.Image.Make"));
        meta.setXmpTagString("Xmp.tiff.Model", meta.getExifTagString("Exif.Image.Model"));
        meta.setImageOrientation(KPMetadata::ORIENTATION_NORMAL);

        QByteArray prof = KPWriteImage::getICCProfilFromFile(settings.outputColorSpace);

        KPWriteImage wImageIface;
        wImageIface.setCancel(&cancel);
        wImageIface.setImageData(imageData, width, height, true, false, prof, meta);
        outUrl = preprocessingTmpDir->name();
        QFileInfo fi(inUrl.toLocalFile());
        outUrl.setFileName(QString(".") + fi.completeBaseName().replace('.', '_') + QString(".tif"));

        if (!wImageIface.write2TIFF(outUrl.toLocalFile()))
            return false;
    }
    else
    {
        return false;
    }

    kDebug() << "Convert RAW output url: " << outUrl;

    return true;
}

QString PreProcessTask::getProcessError(KProcess* const proc) const
{
    if (!proc) return QString();

    QString std = proc->readAll();
    return (i18n("Cannot run %1:\n\n %2", proc->program()[0], std));
}

} // namespace KIPIExpoBlendingPlugin