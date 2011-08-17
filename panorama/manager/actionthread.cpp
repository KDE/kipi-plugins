/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
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

#include <iostream>

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QPointer>

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

namespace KIPIPanoramaPlugin
{

struct ActionThread::ActionThreadPriv
{
    ActionThreadPriv()
        : cancel(false), celeste(false), hdr(false), savePTO(false),
          CPFindProcess(0), CPCleanProcess(0), autoOptimiseProcess(0), pto2MkProcess(0),
          preprocessingTmpDir(0) {}

    struct Task
    {
        bool                celeste;
        bool                hdr;
        bool                horizon;
        bool                projectionAndSize;
        PanoramaFileType    fileType;
        bool                tiffOutput;
        bool                savePTO;
        KUrl::List          urls;
        ItemUrlsMap         preProcessedUrlsMap;
        KUrl                ptoUrl;
        KUrl                mkUrl;
        KUrl                outputUrl;
        Action              action;
        RawDecodingSettings rawDecodingSettings;
    };

    bool                            cancel;
    bool                            celeste;
    bool                            hdr;
    PanoramaFileType                fileType;
    bool                            savePTO;

    QWaitCondition                  condVar;

    QList<Task*>                    todo;
    QMutex                          todo_mutex;

    KProcess*                       CPFindProcess;
    KProcess*                       CPCleanProcess;
    KProcess*                       autoOptimiseProcess;
    KProcess*                       pto2MkProcess;
    KProcess*                       makeProcess;

    /**
     * A list of all running raw instances. Only access this variable via
     * <code>mutex</code>.
     */
    QList<QPointer<KDcraw> >        rawProcesses;
    QList<QPointer<KProcess> >      makeProcesses;

    KTempDir*                       preprocessingTmpDir;

    RawDecodingSettings             rawDecodingSettings;

    void cleanPreprocessingTmpDir()
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

    d->cleanPreprocessingTmpDir();

    cleanUpResultFiles();

    delete d;
}

void ActionThread::setPreProcessingSettings(bool celeste, bool hdr, PanoramaFileType fileType,
                                            const KDcrawIface::RawDecodingSettings& settings)
{
    d->celeste              = celeste;
    d->hdr                  = hdr;
    d->fileType             = fileType;
    d->rawDecodingSettings  = settings;
}

void ActionThread::preProcessFiles(const KUrl::List& urlList)
{
    ActionThreadPriv::Task* t   = new ActionThreadPriv::Task;
    t->action                   = PREPROCESS;
    t->urls                     = urlList;
    t->rawDecodingSettings      = d->rawDecodingSettings;
    t->celeste                  = d->celeste;
    t->hdr                      = d->hdr;
    t->fileType                 = d->fileType;

    QMutexLocker lock(&d->todo_mutex);
    d->todo << t;
    d->condVar.wakeAll();
}

void ActionThread::optimizeProject(const KUrl& ptoUrl, bool levelHorizon, bool optimizeProjectionAndSize)
{
    ActionThreadPriv::Task* t   = new ActionThreadPriv::Task;
    t->action                   = OPTIMIZE;
    t->ptoUrl                   = ptoUrl;
    t->horizon                  = levelHorizon;
    t->projectionAndSize        = optimizeProjectionAndSize;

    QMutexLocker lock(&d->todo_mutex);
    d->todo << t;
    d->condVar.wakeAll();
}

void ActionThread::generatePanoramaPreview(const KUrl& ptoUrl, const ItemUrlsMap& preProcessedUrlsMap)
{
    ActionThreadPriv::Task* t   = new ActionThreadPriv::Task;
    t->action                   = PREVIEW;
    t->preProcessedUrlsMap      = preProcessedUrlsMap;
    t->ptoUrl                   = ptoUrl;

    QMutexLocker lock(&d->todo_mutex);
    d->todo << t;
    d->condVar.wakeAll();
}

void ActionThread::compileProject(const KUrl& ptoUrl, const ItemUrlsMap& preProcessedUrlsMap,
                                  PanoramaFileType fileType)
{
    ActionThreadPriv::Task* t   = new ActionThreadPriv::Task;
    t->action                   = STITCH;
    t->preProcessedUrlsMap      = preProcessedUrlsMap;
    t->ptoUrl                   = ptoUrl;
    t->fileType                 = fileType;

    QMutexLocker lock(&d->todo_mutex);
    d->todo << t;
    d->condVar.wakeAll();
}

void ActionThread::copyFiles(const KUrl& ptoUrl, const KUrl& panoUrl, const KUrl& finalPanoUrl,
                             const ItemUrlsMap& preProcessedUrlsMap, bool savePTO)
{
    ActionThreadPriv::Task* t   = new ActionThreadPriv::Task;
    t->action                   = COPY;
    t->ptoUrl                   = ptoUrl;
    t->urls.append(panoUrl);
    t->preProcessedUrlsMap      = preProcessedUrlsMap;
    t->outputUrl                = finalPanoUrl;
    t->savePTO                  = savePTO;

    QMutexLocker lock(&d->todo_mutex);
    d->todo << t;
    d->condVar.wakeAll();
}

void ActionThread::cancel()
{
    QMutexLocker lock(&d->todo_mutex);
    d->todo.clear();
    d->cancel = true;

    if (d->CPFindProcess)
        d->CPFindProcess->kill();

    if (d->CPCleanProcess)
        d->CPCleanProcess->kill();

    if (d->autoOptimiseProcess)
        d->autoOptimiseProcess->kill();

    if (d->pto2MkProcess)
        d->pto2MkProcess->kill();

    if (d->makeProcess)
        d->makeProcess->kill();

    foreach (QPointer<KDcraw> rawProcess, d->rawProcesses)
    {
        if (rawProcess)
        {
            rawProcess->cancel();
        }
    }

    foreach (KProcess* makeProcess, d->makeProcesses)
    {
        if (makeProcess)
        {
            makeProcess->kill();
        }
    }

    d->condVar.wakeAll();
}

void ActionThread::cleanUpResultFiles()
{
}

void ActionThread::run()
{
    d->cancel = false;
    while (!d->cancel)
    {
        ActionThreadPriv::Task* t = 0;
        {
            QMutexLocker lock(&d->todo_mutex);
            if (!d->todo.isEmpty())
                t = d->todo.takeFirst();
            else
                d->condVar.wait(&d->todo_mutex);
        }

        if (t)
        {
            switch (t->action)
            {
                case PREPROCESS:
                {
                    ActionData ad1;
                    ad1.action      = PREPROCESS;
                    ad1.inUrls      = t->urls;
                    ad1.starting    = true;
                    emit starting(ad1);

                    ItemUrlsMap preProcessedUrlsMap;
                    QString     errors;

                    bool result_success  = startPreProcessing(t->urls, preProcessedUrlsMap, t->rawDecodingSettings);
                    kDebug() << "Preprocess status: " << result_success;
                    if (result_success)
                    {
                        result_success = createPTO(t->hdr, t->fileType, preProcessedUrlsMap, t->ptoUrl);
                        if (result_success)
                        {
                            result_success = startCPFind(t->ptoUrl, t->celeste, errors);
                            if (result_success)
                            {
                                result_success = startCPClean(t->ptoUrl, errors);
                            }
                        }
                    }

                    ActionData ad2;
                    ad2.action              = PREPROCESS;
                    ad2.inUrls              = t->urls;
                    ad2.ptoUrl              = t->ptoUrl;
                    ad2.preProcessedUrlsMap = preProcessedUrlsMap;
                    ad2.success             = result_success;
                    ad2.message             = errors;
                    emit finished(ad2);
                    break;
                }

                case OPTIMIZE:
                {
                    ActionData ad1;
                    ad1.action      = OPTIMIZE;
                    ad1.starting    = true;
                    ad1.ptoUrl      = t->ptoUrl;
                    emit starting(ad1);

                    QString errors;
                    bool    result = false;

                    result  = startOptimization(t->ptoUrl, t->horizon, t->projectionAndSize, errors);

                    ActionData ad2;
                    ad2.action      = OPTIMIZE;
                    ad2.success     = result;
                    ad2.message     = errors;
                    ad2.ptoUrl      = t->ptoUrl;
                    emit finished(ad2);
                    break;
                }

                case PREVIEW:
                {
                    ActionData ad1;
                    ad1.action                  = PREVIEW;
                    ad1.starting                = true;
                    ad1.ptoUrl                  = t->ptoUrl;
                    ad1.preProcessedUrlsMap     = t->preProcessedUrlsMap;
                    emit starting(ad1);

                    QString errors;
                    bool    result = false;

                    KUrl previewUrl;
                    result = computePanoramaPreview(ad1.ptoUrl, previewUrl, ad1.preProcessedUrlsMap, errors);

                    ActionData ad2;
                    ad2.action                  = PREVIEW;
                    ad2.success                 = result;
                    ad2.message                 = errors;
                    ad2.outUrl                  = previewUrl;
                    emit finished(ad2);
                    break;
                }

                case STITCH:
                {
                    QString errors;
                    bool result = false;

                    KUrl mkUrl, panoUrl;
                    result = createMK(t->ptoUrl, mkUrl, panoUrl, t->fileType, errors);
                    if (result)
                    {
                        result = compileMKStepByStep(mkUrl, t->preProcessedUrlsMap, errors);
                    }

                    ActionData ad2;
                    ad2.action                  = STITCH;
                    ad2.success                 = result;
                    ad2.message                 = errors;
                    ad2.outUrl                  = panoUrl;
                    emit finished(ad2);
                    break;
                }

                case COPY:
                {
                    ActionData ad1;
                    ad1.action                  = COPY;
                    ad1.starting                = true;
                    ad1.ptoUrl                  = t->ptoUrl;
                    ad1.inUrls                  = t->urls;
                    ad1.outUrl                  = t->outputUrl;

                    emit starting(ad1);

                    QString errors;
                    bool result = copyFiles(t->urls[0],
                                            t->outputUrl,
                                            t->ptoUrl,
                                            t->preProcessedUrlsMap,
                                            t->savePTO,
                                            errors);

                    ActionData ad2;
                    ad2.action                  = COPY;
                    ad2.success                 = result;
                    ad2.message                 = errors;
                    ad2.ptoUrl                  = t->ptoUrl;
                    ad2.inUrls                  = t->urls;
                    ad2.outUrl                  = t->outputUrl;
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
                                      const RawDecodingSettings& settings)
{
    QString prefix = KStandardDirs::locateLocal("tmp", QString("kipi-panorama-preprocessing-tmp-") +
                                                       QString::number(QDateTime::currentDateTime().toTime_t()));

    d->cleanPreprocessingTmpDir();

    d->preprocessingTmpDir = new KTempDir(prefix);

    // Pre-process RAW files if necessary.

    KUrl::List mixedUrls;     // Original non-RAW + Raw converted urls to align.

    volatile bool error = false;

#pragma omp parallel for
    for (int i = 0; i < inUrls.size(); ++i)
    {

        if (d->cancel || error)
        {
            continue;
        }

        KUrl url = inUrls.at(i);

        if (isRawFile(url.toLocalFile()))
        {
            KUrl preprocessedUrl, previewUrl;

            if (!convertRaw(url, preprocessedUrl, settings))
            {
                error = true;
                continue;
            }

            if (!computePreview(preprocessedUrl, previewUrl))
            {
                error = true;
                continue;
            }

            emit stepFinished();

#pragma omp critical (listAppend)
            {
                mixedUrls.append(preprocessedUrl);
                // In case of alignment is not performed.
                preProcessedUrlsMap.insert(url, ItemPreprocessedUrls(preprocessedUrl, previewUrl));
            }
        }
        else
        {
            // NOTE: in this case, preprocessed Url is original file Url.
            KUrl previewUrl;
            if (!computePreview(url, previewUrl))
            {
                error = true;
                continue;
            }

            emit stepFinished();

#pragma omp critical (listAppend)
            {
                mixedUrls.append(url);
                // In case of alignment is not performed.
                preProcessedUrlsMap.insert(url, ItemPreprocessedUrls(url, previewUrl));
            }
        }
    }

    if (error)
    {
        return false;
    }

    return true;
}

bool ActionThread::startCPFind(KUrl& cpFindPtoUrl, bool celeste, QString& errors)
{
    // Run CPFind to get control points and order the images

    KUrl ptoInUrl(cpFindPtoUrl);
    cpFindPtoUrl = d->preprocessingTmpDir->name();
    cpFindPtoUrl.setFileName(QString("cp_pano.pto"));

    d->CPFindProcess = new KProcess();
    d->CPFindProcess->clearProgram();
    d->CPFindProcess->clearEnvironment();
    d->CPFindProcess->setWorkingDirectory(d->preprocessingTmpDir->name());
    d->CPFindProcess->setOutputChannelMode(KProcess::MergedChannels);

    QStringList args;
    args << "cpfind";
    if (celeste)
        args << "--celeste";
    args << "-o";
    args << cpFindPtoUrl.toLocalFile();
    args << ptoInUrl.toLocalFile();

    d->CPFindProcess->setProgram(args);

    kDebug() << "CPFind command line: " << d->CPFindProcess->program();

    d->CPFindProcess->start();

    if (!d->CPFindProcess->waitForFinished(-1) || d->CPFindProcess->exitCode() != 0)
    {
        errors = getProcessError(d->CPFindProcess);
        delete d->CPFindProcess;
        d->CPFindProcess = 0;
        return false;
    }

    emit stepFinished();

    delete d->CPFindProcess;
    d->CPFindProcess = 0;
    return true;
}

bool ActionThread::startCPClean(KUrl& ptoUrl, QString& errors)
{
    KUrl ptoInUrl = ptoUrl;
    ptoUrl = d->preprocessingTmpDir->name();
    ptoUrl.setFileName(QString("cp_pano_clean.pto"));

    d->CPCleanProcess = new KProcess();
    d->CPCleanProcess->clearProgram();
    d->CPCleanProcess->clearEnvironment();
    d->CPCleanProcess->setWorkingDirectory(d->preprocessingTmpDir->name());
    d->CPCleanProcess->setOutputChannelMode(KProcess::MergedChannels);

    QStringList args;
    args << "cpclean";
    args << "-o";
    args << ptoUrl.toLocalFile();
    args << ptoInUrl.toLocalFile();

    d->CPCleanProcess->setProgram(args);

    kDebug() << "CPClean command line: " << d->CPCleanProcess->program();

    d->CPCleanProcess->start();

    if (!d->CPCleanProcess->waitForFinished(-1) || d->CPCleanProcess->exitCode() != 0)
    {
        errors = getProcessError(d->CPCleanProcess);
        delete d->CPCleanProcess;
        d->CPCleanProcess = 0;
        return false;
    }

    emit stepFinished();

    delete d->CPCleanProcess;
    d->CPCleanProcess = 0;
    return true;
}

bool ActionThread::startOptimization(KUrl& ptoUrl, bool levelHorizon, bool optimizeProjectionAndSize, QString& errors)
{
    KUrl ptoAOUrl = d->preprocessingTmpDir->name();
    ptoAOUrl.setFileName(QString("auto_op_pano.pto"));
    KUrl ptoVOUrl = d->preprocessingTmpDir->name();
    ptoVOUrl.setFileName(QString("vig_op_pano.pto"));

    d->autoOptimiseProcess = new KProcess();
    d->autoOptimiseProcess->clearProgram();
    d->autoOptimiseProcess->clearEnvironment();
    d->autoOptimiseProcess->setWorkingDirectory(d->preprocessingTmpDir->name());
    d->autoOptimiseProcess->setOutputChannelMode(KProcess::MergedChannels);

    QStringList argsAO;
    argsAO << "autooptimiser";
    argsAO << "-am";
    if (levelHorizon)
    {
        argsAO << "-l";
    }
    if (optimizeProjectionAndSize)
    {
        argsAO << "-s";
    }
    argsAO << "-o";
    argsAO << ptoAOUrl.toLocalFile();
    argsAO << ptoUrl.toLocalFile();

    d->autoOptimiseProcess->setProgram(argsAO);

    kDebug() << "autooptimiser command line: " << d->autoOptimiseProcess->program();

    d->autoOptimiseProcess->start();

    if (!d->autoOptimiseProcess->waitForFinished(-1) || d->autoOptimiseProcess->exitCode() != 0)
    {
        errors = getProcessError(d->autoOptimiseProcess);
        delete d->autoOptimiseProcess;
        d->autoOptimiseProcess = 0;
        return false;
    }

    ptoUrl = ptoAOUrl;

    emit stepFinished();

    delete d->autoOptimiseProcess;
    d->autoOptimiseProcess = 0;

    return true;
}

bool ActionThread::computePanoramaPreview(KUrl& ptoUrl, KUrl& previewUrl, const ItemUrlsMap& preProcessedUrlsMap, QString& errors)
{
    kDebug() << "Preview Generation (" << ptoUrl.toLocalFile() << ")";
    QFile input(ptoUrl.toLocalFile());
    QStringList pto;
    if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        kDebug() << "Can't read PTO File!";
        return false;
    }
    else
    {
        QTextStream in(&input);
        while (!in.atEnd())
        {
            pto.append(in.readLine());
        }
        input.close();
    }

    ptoUrl.setFileName("preview.pto");
    input.setFileName(ptoUrl.toLocalFile());
    if (!input.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        kDebug() << "Can't create a new PTO File!";
        return false;
    }

    QTextStream previewPtoStream(&input);

    KExiv2 metaOrig(preProcessedUrlsMap.begin().value().preprocessedUrl.toLocalFile());
    KExiv2 metaPreview(preProcessedUrlsMap.begin().value().previewUrl.toLocalFile());
    double scalingFactor = ((double) metaPreview.getPixelSize().width()) / ((double) metaOrig.getPixelSize().width());

    foreach (QString line, pto)
    {
        QString tmp;
        QStringList parameters = line.split(' ', QString::SkipEmptyParts);
        if (line[0] == 'p')
        {
            tmp = "";
            foreach (QString p, parameters)
            {
                if (p[0] == 'w' || p[0] == 'h')
                {
                    int size = ((double) (p.right(p.size() - 1)).toInt()) * scalingFactor;
                    tmp.append(p[0]);
                    tmp.append(QString::number(size));
                }
                else if (p[0] == 'n')
                {
                    tmp.append("n\"JPEG q90\"");
                    break;          // n should be the last parameter (and the space before qXX introduce another parameter)
                }
                else
                {
                    tmp.append(p);
                }
                tmp.append(" ");
            }
        }
        else if (line[0] == 'm')
        {
            tmp = line;
        }
        else if (line[0] == 'i')
        {
            tmp = "";
            QStringList realParameters;
            bool nRead = false;
            foreach (QString p, parameters)
            {
                if (p[0] != 'n')
                {
                    if (nRead)
                    {
                        realParameters[realParameters.size() - 1] += " " + p;
                    }
                    else
                    {
                        realParameters << p;
                    }
                }
                else
                {
                    nRead = true;
                    realParameters << p;
                }
            }
            foreach (QString p, realParameters)
            {
                if (p[0] == 'w')
                {
                    tmp.append("w");
                    tmp.append(QString::number(metaPreview.getPixelSize().width()));
                }
                else if (p[0] == 'h')
                {
                    tmp.append("h");
                    tmp.append(QString::number(metaPreview.getPixelSize().height()));
                }
                else if (p[0] == 'n')
                {
                    QString imgFileName = p.mid(2, p.size() - 3);
                    KUrl imgUrl(KUrl(d->preprocessingTmpDir->name()), imgFileName);
                    ItemUrlsMap::iterator it;
                    for (it = (ItemUrlsMap::iterator) preProcessedUrlsMap.begin(); it != preProcessedUrlsMap.end() && it.value().preprocessedUrl != imgUrl; ++it);
                    if (it == preProcessedUrlsMap.end())
                    {
                        input.close();
                        kDebug() << "Unknown input File in the PTO: " << imgFileName;
                        kDebug() << "IMG: " << imgUrl.toLocalFile();
                        return false;
                    }
                    tmp.append("n\"");
                    tmp.append(it.value().previewUrl.fileName());
                    tmp.append("\"");
                    break;
                }
                else
                {
                    tmp.append(p);
                }
                tmp.append(" ");
            }
        }
        else
        {
            continue;
        }
        previewPtoStream << tmp << endl;
    }

    // Add two commented line for a JPEG output
    previewPtoStream << "#hugin_outputImageType jpg" << endl;
    previewPtoStream << "#hugin_outputJPEGQuality 90" << endl;

    input.close();

    kDebug() << "Preview PTO File created: " << ptoUrl.fileName();

    KUrl mkUrl;
    if (!createMK(ptoUrl, mkUrl, previewUrl, JPEG, errors))
    {
        kDebug() << "Makefile creation failed!";
        return false;
    }

    if (!compileMK(mkUrl, errors))
    {
        kDebug() << "Makefile execution failed!";
        return false;
    }

    return true;
}

bool ActionThread::computePreview(const KUrl& inUrl, KUrl& outUrl)
{
    outUrl = d->preprocessingTmpDir->name();
    QFileInfo fi(inUrl.toLocalFile());
    outUrl.setFileName(QString(".") + fi.completeBaseName().replace(".", "_") + QString("-preview.jpg"));

    QImage img;
    if (img.load(inUrl.toLocalFile()))
    {
        QImage preview = img.scaled(1280, 1024, Qt::KeepAspectRatio);
        bool saved = preview.save(outUrl.toLocalFile(), "JPEG");
        // save exif information also to preview image for auto rotation
        if (saved)
        {
            KExiv2 metai(inUrl.toLocalFile());
            KExiv2 metao(outUrl.toLocalFile());
            metao.setImageOrientation(metai.getImageOrientation());
            metao.setImageDimensions(QSize(preview.width(), preview.height()));
            metao.applyChanges();
        }
        kDebug() << "Preview Image url: " << outUrl << ", saved: " << saved;
        return saved;
    }
    return false;
}

bool ActionThread::convertRaw(const KUrl& inUrl, KUrl& outUrl, const RawDecodingSettings& settings)
{
    int        width, height, rgbmax;
    QByteArray imageData;

    QPointer<KDcraw> rawdec = new KDcraw;

    {
        QMutexLocker lock(&d->todo_mutex);
        d->rawProcesses << rawdec;
    }

    bool decoded = rawdec->decodeRAWImage(inUrl.toLocalFile(), settings, imageData, width, height, rgbmax);

    {
        QMutexLocker lock(&d->todo_mutex);
        d->rawProcesses.removeAll(rawdec);
    }

    if (decoded)
    {
        uchar* sptr  = (uchar*)imageData.data();
        float factor = 65535.0 / rgbmax;
        unsigned short tmp16[3];

        // Set RGB color components.
        for (int i = 0 ; !d->cancel && (i < width * height) ; ++i)
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
        outUrl.setFileName(fi.completeBaseName().replace(".", "_") + QString(".tif"));

        if (!wImageIface.write2TIFF(outUrl.toLocalFile()))
            return false;
        else
            meta.save(outUrl.toLocalFile());
    }
    else
    {
        return false;
    }

    kDebug() << "Convert RAW output url: " << outUrl;

    return true;
}

bool ActionThread::isRawFile(const KUrl& url)
{
    QString rawFilesExt(KDcraw::rawFiles());

    QFileInfo fileInfo(url.toLocalFile());
    if (rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper()))
        return true;

    return false;
}

bool ActionThread::createPTO(bool hdr, PanoramaFileType fileType, const ItemUrlsMap& urlList, KUrl& ptoUrl)
{
    ptoUrl = d->preprocessingTmpDir->name();
    ptoUrl.setFileName(QString("pano_base.pto"));

    QFile pto(ptoUrl.toLocalFile());
    if (pto.exists())
        return false;
    if (!pto.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;

    QTextStream pto_stream(&pto);

    // The pto is created following the file format described here:
    // http://hugin.sourceforge.net/docs/nona/nona.txt

    // 1. Project parameters
    pto_stream << "p";
    pto_stream << " f1";                    // Cylindrical projection
    pto_stream << " n\"TIFF_m c:LZW\"";
    pto_stream << " R" << (hdr ? '1' : '0');// HDR output
    //pto_stream << " T\"FLOAT\"";            // 32bits color depth
    //pto_stream << " S," << X_left << "," << X_right << "," << X_top << "," << X_bottom;   // Crop values
    pto_stream << " k0";                    // Reference image
    pto_stream << endl;

    // 2. Images
    pto_stream << endl;
    int i = 0;
    foreach (ItemPreprocessedUrls url, urlList)
    {
        KExiv2 meta;
        meta.load(url.preprocessedUrl.toLocalFile());
        QSize size = meta.getPixelSize();

        pto_stream << "i";
        pto_stream << " f0";                    // Lens projection type (rectilinear)
        pto_stream << " w" << size.width();     // Image width
        pto_stream << " h" << size.height();    // Image height
        if (i > 0)
        {
            // We suppose that the pictures are all taken with the same camera and lens
            pto_stream << " a=0 b=0 c=0 d=0 e=0 v=0 g=0 t=0";           // Geometry
            pto_stream << " Va=0 Vb=0 Vc=0 Vd=0 Vx=0 Vy=0";             // Vignetting
        }
        pto_stream << " n\"" << url.preprocessedUrl.toLocalFile() << '"';
        pto_stream << endl;

        i++;
    }

    // 3. Variables to optimize
    pto_stream << endl;
    // Geometry optimization
    pto_stream << "v a0" << endl;
    pto_stream << "v b0" << endl;
    pto_stream << "v c0" << endl;
    pto_stream << "v d0" << endl;
    pto_stream << "v e0" << endl;
    pto_stream << "v Va0" << endl;
    pto_stream << "v Vb0" << endl;
    pto_stream << "v Vc0" << endl;
    pto_stream << "v Vd0" << endl;
    pto_stream << "v Vx0" << endl;
    pto_stream << "v Vy0" << endl;
    for (int j = 0; j < i; ++j)
    {
        // Colors optimization
        pto_stream << "v Ra" << j << endl;
        pto_stream << "v Rb" << j << endl;
        pto_stream << "v Rc" << j << endl;
        pto_stream << "v Rd" << j << endl;
        pto_stream << "v Re" << j << endl;
        pto_stream << "v Eev" << j << endl;
        pto_stream << "v Erv" << j << endl;
        pto_stream << "v Ebv" << j << endl;
        // Position optimization
        pto_stream << "v y" << j << endl;
        pto_stream << "v p" << j << endl;
        pto_stream << "v r" << j << endl;
    }

    switch (fileType)
    {
        case TIFF:
            pto_stream << "#hugin_outputImageType tif" << endl;
            pto_stream << "#hugin_outputImageTypeCompression LZW" << endl;
            break;
        case JPEG:
            pto_stream << "#hugin_outputImageType jpg" << endl;
            pto_stream << "#hugin_outputJPEGQuality 95" << endl;
            break;
    }

    pto.close();

    return true;
}

bool ActionThread::createMK(KUrl& ptoUrl, KUrl& mkUrl, KUrl& panoUrl, PanoramaFileType fileType, QString& errors)
{
    QFileInfo fi(ptoUrl.toLocalFile());
    mkUrl = d->preprocessingTmpDir->name();
    mkUrl.setFileName(fi.completeBaseName() + QString(".mk"));

    panoUrl = d->preprocessingTmpDir->name();
    switch (fileType)
    {
        case JPEG:
            panoUrl.setFileName(fi.completeBaseName() + QString(".jpg"));
            break;
        case TIFF:
            panoUrl.setFileName(fi.completeBaseName() + QString(".tif"));
            break;
    }

    d->pto2MkProcess = new KProcess();
    d->pto2MkProcess->clearProgram();
    d->pto2MkProcess->clearEnvironment();
    d->pto2MkProcess->setWorkingDirectory(d->preprocessingTmpDir->name());
    d->pto2MkProcess->setOutputChannelMode(KProcess::MergedChannels);

    QStringList args;
    args << "pto2mk";
    args << "-o";
    args << mkUrl.toLocalFile();
    args << "-p";
    args << fi.completeBaseName();
    args << ptoUrl.toLocalFile();

    d->pto2MkProcess->setProgram(args);

    kDebug() << "pto2mk command line: " << d->pto2MkProcess->program();

    d->pto2MkProcess->start();

    if (!d->pto2MkProcess->waitForFinished(-1) || d->pto2MkProcess->exitCode() != 0)
    {
        errors = getProcessError(d->pto2MkProcess);
        delete d->pto2MkProcess;
        d->pto2MkProcess = 0;
        return false;
    }

    delete d->pto2MkProcess;
    d->pto2MkProcess = 0;

    return true;
}

bool ActionThread::compileMK(KUrl& mkUrl, QString& errors)
{
    d->makeProcess = new KProcess();
    d->makeProcess->clearProgram();
    d->makeProcess->clearEnvironment();
    d->makeProcess->setWorkingDirectory(d->preprocessingTmpDir->name());
    d->makeProcess->setOutputChannelMode(KProcess::MergedChannels);

    QStringList args;
    args << "make";
    args << "-f";
    args << mkUrl.toLocalFile();

    d->makeProcess->setProgram(args);

    kDebug() << "make command line: " << d->makeProcess->program();

    d->makeProcess->start();

    if (!d->makeProcess->waitForFinished(-1) || d->makeProcess->exitCode() != 0)
    {
        errors = getProcessError(d->makeProcess);
        delete d->makeProcess;
        d->makeProcess = 0;
        return false;
    }

    delete d->makeProcess;
    d->makeProcess = 0;

    return true;
}

bool ActionThread::compileMKStepByStep(KUrl& mkUrl, const ItemUrlsMap& urlList, QString& errors)
{
    QFileInfo fi(mkUrl.toLocalFile());

    volatile bool error = false;

#pragma omp parallel for ordered
    for (int i = 0; i < urlList.size(); ++i)
    {
        if (d->cancel || error)
        {
            continue;
        }

        QPointer<KProcess> makeProcess = new KProcess;
        makeProcess->clearProgram();
        makeProcess->clearEnvironment();
        makeProcess->setWorkingDirectory(d->preprocessingTmpDir->name());
        makeProcess->setOutputChannelMode(KProcess::MergedChannels);

        {
            QMutexLocker lock(&d->todo_mutex);
            d->makeProcesses << makeProcess;
        }

        QString mkFile = fi.completeBaseName() + (i >= 10 ? (i >= 100 ? "0" : "00") : "000") + QString::number(i) + ".tif";
        QStringList args;
        args << "make";
        args << "-f";
        args << mkUrl.toLocalFile();
        args << mkFile;

        makeProcess->setProgram(args);
        kDebug() << "make command line: " << makeProcess->program();

        ActionData ad1;
        ad1.starting    = true;
        ad1.action      = NONAFILE;
        ad1.id          = i;
#pragma omp critical
        {
            emit starting(ad1);
        }

        makeProcess->start();

        if (!makeProcess->waitForFinished(-1) || makeProcess->exitCode() != 0)
        {
            {
                QMutexLocker lock(&d->todo_mutex);
                d->makeProcesses.removeAll(makeProcess);
            }

            error = true;
            ActionData ad2;
            ad2.action      = NONAFILE;
            ad2.success     = false;
            ad2.id          = i;
#pragma omp critical
            {
                errors = getProcessError(makeProcess);
                ad2.message = errors;
                emit finished(ad2);
            }
            continue;
        }

        {
            QMutexLocker lock(&d->todo_mutex);
            d->makeProcesses.removeAll(makeProcess);
        }

        ActionData ad2;
        ad2.action          = NONAFILE;
        ad2.success         = true;
        ad2.id              = i;
        ad2.outUrl          = KUrl(d->preprocessingTmpDir->name());
        ad2.outUrl.setFileName(mkFile);
        #pragma omp critical
        {
            emit finished(ad2);
        }
    }

    if (error)
        return false;

    ActionData ad1;
    ad1.action                  = STITCH;
    ad1.starting                = true;
    ad1.ptoUrl                  = mkUrl;

    emit starting(ad1);

    return compileMK(mkUrl, errors);
}

bool ActionThread::copyFiles(const KUrl& panoUrl, const KUrl& finalPanoUrl, const KUrl& ptoUrl,
                             const ItemUrlsMap& urlList, bool savePTO, QString& errors)
{
    QFile panoFile(panoUrl.toLocalFile());
    QFile finalPanoFile(finalPanoUrl.toLocalFile());

    QFileInfo fi(finalPanoUrl.toLocalFile());
    KUrl finalPTOUrl(finalPanoUrl);
    finalPTOUrl.setFileName(fi.completeBaseName() + ".pto");
    QFile ptoFile(ptoUrl.toLocalFile());
    QFile finalPTOFile(finalPTOUrl.toLocalFile());

    if (!panoFile.exists())
    {
        errors = i18n("Temporary panorama file does not exists!!");
        kDebug() << "Temporary panorama file does not exists: " + panoUrl.toLocalFile();
        return false;
    }
    if (finalPanoFile.exists())
    {
        errors = i18n("A file named %1 already exists!!", finalPanoUrl.fileName());
        kDebug() << "Final panorama file already exists: " + finalPanoUrl.toLocalFile();
        return false;
    }
    if (savePTO && !ptoFile.exists())
    {
        errors = i18n("Temporary project file does not exists!!");
        kDebug() << "Temporary project file does not exists: " + ptoUrl.toLocalFile();
        return false;
    }
    if (savePTO && finalPTOFile.exists())
    {
        errors = i18n("A file named %1 already exists!!", finalPTOUrl.fileName());
        kDebug() << "Final project file already exists: " + finalPTOUrl.toLocalFile();
        return false;
    }

    kDebug() << "Copying panorama file...";
    if (!panoFile.copy(finalPanoUrl.toLocalFile()) || !panoFile.remove())
    {
        errors = i18n("Cannot move panorama from %1 to %2!!",
                      panoUrl.toLocalFile(),
                      finalPanoUrl.toLocalFile());
        kDebug() << "Cannot move panorama: QFile errror = " + panoFile.error();
        return false;
    }

    if (savePTO)
    {
        kDebug() << "Copying project file...";
        if (!ptoFile.copy(finalPTOUrl.toLocalFile()))
        {
            errors = i18n("Cannot move project file from %1 to %2!!",
                          panoUrl.toLocalFile(),
                          finalPanoUrl.toLocalFile());
            return false;
        }

        kDebug() << "Copying converted RAW files...";
        for (ItemUrlsMap::iterator i = (ItemUrlsMap::iterator) urlList.begin(); i != urlList.end(); ++i)
        {
            if (isRawFile(i.key()))
            {
                KUrl finalImgUrl(finalPanoUrl);
                finalImgUrl.setFileName(i->preprocessedUrl.fileName());
                QFile imgFile(i->preprocessedUrl.toLocalFile());
                if (!imgFile.copy(finalImgUrl.toLocalFile()))
                {
                    errors = i18n("Cannot copy converted image file from %1 to %2!!",
                                  i->preprocessedUrl.toLocalFile(),
                                  finalImgUrl.toLocalFile());
                    return false;
                }
            }
        }
    }

    return true;
}

QString ActionThread::getProcessError(KProcess* proc) const
{
    if (!proc) return QString();

    QString std = proc->readAll();
    return (i18n("Cannot run %1:\n\n %2", proc->program()[0], std));
}

}  // namespace KIPIPanoramaPlugin
