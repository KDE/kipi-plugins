/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "actionthread.h"

// Qt includes

#include <QDateTime>
#include <QSharedPointer>
#include <QString>

// KDE includes

#include <klocale.h>
#include <kstandarddirs.h>
#include <ktempdir.h>
#include <ThreadWeaver/Queue>
#include <ThreadWeaver/Sequence>
#include <ThreadWeaver/QObjectDecorator>
#include <threadweaver/debuggingaids.h>


// Local includes

#include <kipiplugins_debug.h>
#include "tasks.h"

using namespace ThreadWeaver;

namespace KIPIPanoramaPlugin
{

struct ActionThread::Private
{
    Private(QObject* parent = 0)
         : preprocessingTmpDir(0), threadQueue(new Queue(parent))
    {
        ThreadWeaver::setDebugLevel(true, 10);
    }

    ~Private()
    {
        cleanPreprocessingTmpDir();
    }

    KTempDir*                       preprocessingTmpDir;
    QSharedPointer<Queue>           threadQueue;


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

ActionThread::ActionThread(QObject* const parent)
    : QObject(parent), d(new Private(this))
{
    // ActionThread init
    qRegisterMetaType<ActionData>();

    d->threadQueue->setMaximumNumberOfThreads(qMax(QThread::idealThreadCount(), 1));
    qCDebug(KIPIPLUGINS_LOG) << "Starting Main Thread";
}

ActionThread::~ActionThread()
{
    qCDebug(KIPIPLUGINS_LOG) << "Calling action thread destructor";

    delete d;
}

void ActionThread::cancel()
{
    qCDebug(KIPIPLUGINS_LOG) << "Cancel Main Thread";
    d->threadQueue->dequeue();
    d->threadQueue->requestAbort();
}

void ActionThread::finish()
{
    // Wait for all queued jobs to finish
    d->threadQueue->finish();
}


void ActionThread::preProcessFiles(const KUrl::List& urlList, ItemUrlsMap& preProcessedMap,
                                   KUrl& baseUrl, KUrl& cpFindPtoUrl, KUrl& cpCleanPtoUrl,
                                   bool celeste, PanoramaFileType fileType, bool gPano,
                                   const RawDecodingSettings& rawSettings, const QString& huginVersion,
                                   const QString& cpCleanPath, const QString& cpFindPath)
{
    d->cleanPreprocessingTmpDir();

    QString prefix = KStandardDirs::locateLocal("tmp", QString("kipi-panorama-tmp-") +
                                                       QString::number(QDateTime::currentDateTime().toTime_t()));

    d->preprocessingTmpDir = new KTempDir(prefix);

    QSharedPointer<Sequence> jobSeq(new Sequence());

    QSharedPointer<Collection> preprocessJobs(new Collection());

    int id = 0;
    foreach (const KUrl& file, urlList)
    {
        preProcessedMap.insert(file, ItemPreprocessedUrls());

        QObjectDecorator* t = new QObjectDecorator(new PreProcessTask(d->preprocessingTmpDir->name(),
                                                                      id++,
                                                                      preProcessedMap[file],
                                                                      file,
                                                                      rawSettings));

        connect(t, SIGNAL(started(ThreadWeaver::JobPointer)),
                this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
        connect(t, SIGNAL(done(ThreadWeaver::JobPointer)),
                this, SLOT(slotStepDone(ThreadWeaver::JobPointer)));

        (*preprocessJobs) << JobPointer(t);
    }
    (*jobSeq) << preprocessJobs;

    QObjectDecorator* pto = new QObjectDecorator(new CreatePtoTask(d->preprocessingTmpDir->name(),
                                                                   fileType,
                                                                   baseUrl,
                                                                   urlList,
                                                                   preProcessedMap,
                                                                   gPano,
                                                                   huginVersion));

    connect(pto, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(pto, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotStepDone(ThreadWeaver::JobPointer)));

    (*jobSeq) << JobPointer(pto);

    QObjectDecorator* cpFind = new QObjectDecorator(new CpFindTask(d->preprocessingTmpDir->name(),
                                                                   baseUrl,
                                                                   cpFindPtoUrl,
                                                                   celeste,
                                                                   cpFindPath));

    connect(cpFind, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(cpFind, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotStepDone(ThreadWeaver::JobPointer)));

    (*jobSeq) << JobPointer(cpFind);

    QObjectDecorator* cpClean = new QObjectDecorator(new CpCleanTask(d->preprocessingTmpDir->name(),
                                                                     cpFindPtoUrl,
                                                                     cpCleanPtoUrl,
                                                                     cpCleanPath));

    connect(cpClean, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(cpClean, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotDone(ThreadWeaver::JobPointer)));

    (*jobSeq) << JobPointer(cpClean);

    d->threadQueue->enqueue(jobSeq);
}

void ActionThread::optimizeProject(KUrl& ptoUrl, KUrl& optimizePtoUrl, KUrl& viewCropPtoUrl,
                                   bool levelHorizon, bool buildGPano,
                                   const QString& autooptimiserPath, const QString& panoModifyPath)
{
    QSharedPointer<Sequence> jobs(new Sequence());

    QObjectDecorator* ot = new QObjectDecorator(new OptimisationTask(d->preprocessingTmpDir->name(),
                                                                     ptoUrl,
                                                                     optimizePtoUrl,
                                                                     levelHorizon,
                                                                     buildGPano,
                                                                     autooptimiserPath));

    connect(ot, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(ot, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotStepDone(ThreadWeaver::JobPointer)));

    (*jobs) << ot;

    QObjectDecorator* act = new QObjectDecorator(new AutoCropTask(d->preprocessingTmpDir->name(),
                                                                  optimizePtoUrl,
                                                                  viewCropPtoUrl,
                                                                  buildGPano,
                                                                  panoModifyPath));

    connect(act, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(act, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotDone(ThreadWeaver::JobPointer)));

    (*jobs) << act;

    d->threadQueue->enqueue(jobs);
}

void ActionThread::generatePanoramaPreview(const PTOType& ptoData, KUrl& previewPtoUrl, KUrl& previewMkUrl, KUrl& previewUrl,
                                           const ItemUrlsMap& preProcessedUrlsMap,
                                           const QString& makePath, const QString& pto2mkPath,
                                           const QString& enblendPath, const QString& nonaPath)
{
    QSharedPointer<Sequence> jobs(new Sequence());

    QObjectDecorator* ptoTask = new QObjectDecorator(new CreatePreviewTask(d->preprocessingTmpDir->name(),
                                                                           ptoData,
                                                                           previewPtoUrl,
                                                                           preProcessedUrlsMap));

    connect(ptoTask, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(ptoTask, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotStepDone(ThreadWeaver::JobPointer)));

    (*jobs) << ptoTask;

    appendStitchingJobs(jobs,
                        previewPtoUrl,
                        previewMkUrl,
                        previewUrl,
                        preProcessedUrlsMap,
                        JPEG,
                        makePath,
                        pto2mkPath,
                        enblendPath,
                        nonaPath,
                        true);

    d->threadQueue->enqueue(jobs);
}

void ActionThread::compileProject(const PTOType& basePtoData, KUrl& panoPtoUrl, KUrl& mkUrl, KUrl& panoUrl,
                                  const ItemUrlsMap& preProcessedUrlsMap,
                                  PanoramaFileType fileType, const QRect& crop,
                                  const QString& makePath, const QString& pto2mkPath,
                                  const QString& enblendPath, const QString& nonaPath)
{
    QSharedPointer<Sequence> jobs(new Sequence());

    QObjectDecorator* ptoTask = new QObjectDecorator(new CreateFinalPtoTask(d->preprocessingTmpDir->name(),
                                                                            basePtoData,
                                                                            panoPtoUrl,
                                                                            crop));

    connect(ptoTask, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(ptoTask, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotStepDone(ThreadWeaver::JobPointer)));

    (*jobs) << ptoTask;

    appendStitchingJobs(jobs,
                        panoPtoUrl,
                        mkUrl,
                        panoUrl,
                        preProcessedUrlsMap,
                        fileType,
                        makePath,
                        pto2mkPath,
                        enblendPath,
                        nonaPath,
                        false);

    d->threadQueue->enqueue(jobs);
}

void ActionThread::copyFiles(const KUrl& ptoUrl, const KUrl& panoUrl, const KUrl& finalPanoUrl,
                             const ItemUrlsMap& preProcessedUrlsMap, bool savePTO, bool addGPlusMetadata)
{
    QObjectDecorator* t = new QObjectDecorator(new CopyFilesTask(d->preprocessingTmpDir->name(),
                                                                 panoUrl,
                                                                 finalPanoUrl,
                                                                 ptoUrl,
                                                                 preProcessedUrlsMap,
                                                                 savePTO,
                                                                 addGPlusMetadata));

    connect(t, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));

    connect(t, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotDone(ThreadWeaver::JobPointer)));

    d->threadQueue->enqueue(JobPointer(t));
}

void ActionThread::slotStarting(JobPointer j)
{
    QSharedPointer<QObjectDecorator> dec = j.staticCast<QObjectDecorator>();
    Task* t = static_cast<Task*>(dec->job());

    ActionData ad;
    ad.starting     = true;
    ad.action       = t->action;
    ad.id           = -1;

    if (t->action == NONAFILE)
    {
        CompileMKStepTask* c = static_cast<CompileMKStepTask*>(t);
        ad.id = c->id;
    }
    else if (t->action == PREPROCESS_INPUT)
    {
        PreProcessTask* p = static_cast<PreProcessTask*>(t);
        ad.id = p->id;
    }

    emit starting(ad);
}

void ActionThread::slotStepDone(JobPointer j)
{
    QSharedPointer<QObjectDecorator> dec = j.staticCast<QObjectDecorator>();
    Task* t = static_cast<Task*>(dec->job());

    ActionData ad;
    ad.starting     = false;
    ad.action       = t->action;
    ad.id           = -1;
    ad.success      = t->success();
    ad.message      = t->errString;

    if (t->action == NONAFILE)
    {
        CompileMKStepTask* c = static_cast<CompileMKStepTask*>(t);
        ad.id = c->id;
    }
    else if (t->action == PREPROCESS_INPUT)
    {
        PreProcessTask* p = static_cast<PreProcessTask*>(t);
        ad.id = p->id;
    }

    emit stepFinished(ad);
}

void ActionThread::slotDone(JobPointer j)
{
    qCDebug(KIPIPLUGINS_LOG) << "Something is done...";
    QSharedPointer<QObjectDecorator> dec = j.staticCast<QObjectDecorator>();
    Task* t = static_cast<Task*>(dec->job());

    ActionData ad;
    ad.starting     = false;
    ad.action       = t->action;
    ad.id           = -1;
    ad.success      = t->success();
    ad.message      = t->errString;

    if (t->action == NONAFILE)
    {
        CompileMKStepTask* c = static_cast<CompileMKStepTask*>(t);
        ad.id = c->id;
    }
    else if (t->action == PREPROCESS_INPUT)
    {
        PreProcessTask* p = static_cast<PreProcessTask*>(t);
        ad.id = p->id;
    }

    emit jobCollectionFinished(ad);
}

void ActionThread::appendStitchingJobs(QSharedPointer<Sequence>& js, const KUrl& ptoUrl, KUrl& mkUrl,
                                       KUrl& outputUrl, const ItemUrlsMap& preProcessedUrlsMap,
                                       PanoramaFileType fileType,
                                       const QString& makePath, const QString& pto2mkPath,
                                       const QString& enblendPath, const QString& nonaPath, bool preview)
{
    QSharedPointer<Sequence> jobs(new Sequence());

    QObjectDecorator* createMKTask = new QObjectDecorator(new CreateMKTask(d->preprocessingTmpDir->name(),
                                                                           ptoUrl,
                                                                           mkUrl,
                                                                           outputUrl,
                                                                           fileType,
                                                                           pto2mkPath,
                                                                           preview));

    connect(createMKTask, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(createMKTask, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotStepDone(ThreadWeaver::JobPointer)));

    (*jobs) << createMKTask;

    for (int i = 0; i < preProcessedUrlsMap.size(); i++)
    {
        QObjectDecorator* t = new QObjectDecorator(new CompileMKStepTask(d->preprocessingTmpDir->name(),
                                                                         i,
                                                                         mkUrl,
                                                                         nonaPath,
                                                                         enblendPath,
                                                                         makePath,
                                                                         preview));

        connect(t, SIGNAL(started(ThreadWeaver::JobPointer)),
                this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
        connect(t, SIGNAL(done(ThreadWeaver::JobPointer)),
                this, SLOT(slotStepDone(ThreadWeaver::JobPointer)));

        (*jobs) << t;
    }

    QObjectDecorator* compileMKTask = new QObjectDecorator(new CompileMKTask(d->preprocessingTmpDir->name(),
                                                                             mkUrl,
                                                                             outputUrl,
                                                                             nonaPath,
                                                                             enblendPath,
                                                                             makePath,
                                                                             preview));

    connect(compileMKTask, SIGNAL(started(ThreadWeaver::JobPointer)),
            this, SLOT(slotStarting(ThreadWeaver::JobPointer)));
    connect(compileMKTask, SIGNAL(done(ThreadWeaver::JobPointer)),
            this, SLOT(slotDone(ThreadWeaver::JobPointer)));

    (*jobs) << compileMKTask;

    (*js) << jobs;
}

}  // namespace KIPIPanoramaPlugin
