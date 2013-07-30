/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "actionthread.moc"

// Qt includes

#include <QDateTime>
#include <QPointer>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ktempdir.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <threadweaver/DependencyPolicy.h>

// Local includes

#include "tasks.h"

using namespace ThreadWeaver;

namespace KIPIPanoramaPlugin
{

struct ActionThread::Private
{
    Private()
         : preprocessingTmpDir(0)
    {}

    ~Private()
    {
        cleanPreprocessingTmpDir();
    }

    KTempDir*                       preprocessingTmpDir;

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
    : RActionThreadBase(parent), d(new Private)
{
    qRegisterMetaType<ActionData>();
}

ActionThread::~ActionThread()
{
    delete d;
}

void ActionThread::preProcessFiles(const KUrl::List& urlList, ItemUrlsMap& preProcessedMap,
                                   KUrl& baseUrl, KUrl& cpFindPtoUrl, KUrl& cpCleanPtoUrl,
                                   bool celeste, bool hdr, PanoramaFileType fileType,
                                   const RawDecodingSettings& rawSettings,
                                   const QString& cpCleanPath, const QString& cpFindPath)
{
    d->cleanPreprocessingTmpDir();

    QString prefix = KStandardDirs::locateLocal("tmp", QString("kipi-panorama-tmp-") +
                                                       QString::number(QDateTime::currentDateTime().toTime_t()));

    d->preprocessingTmpDir = new KTempDir(prefix);

    JobCollection       *jobs           = new JobCollection();

    // TODO: try to append these jobs as a JobCollection inside a JobSequence
    int id = 0;
    QVector<PreProcessTask*> preProcessingTasks;
    foreach (const KUrl& file, urlList)
    {
        preProcessedMap.insert(file, ItemPreprocessedUrls());

        PreProcessTask *t = new PreProcessTask(d->preprocessingTmpDir->name(),
                                               id++,
                                               preProcessedMap[file],
                                               file,
                                               rawSettings);

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotStarting(ThreadWeaver::Job*)));
        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotStepDone(ThreadWeaver::Job*)));

        preProcessingTasks.append(t);
        jobs->addJob(t);
    }

    CreatePtoTask *pto = new CreatePtoTask(d->preprocessingTmpDir->name(),
                                           fileType,
                                           hdr,
                                           baseUrl,
                                           urlList,
                                           preProcessedMap);

    connect(pto, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(pto, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    foreach (PreProcessTask *t, preProcessingTasks)
    {
        DependencyPolicy::instance().addDependency(pto, t);
    }
    jobs->addJob(pto);

    CpFindTask *cpFind = new CpFindTask(d->preprocessingTmpDir->name(),
                                        baseUrl,
                                        cpFindPtoUrl,
                                        celeste,
                                        cpFindPath);

    connect(cpFind, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(cpFind, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    jobs->addJob(cpFind);
    DependencyPolicy::instance().addDependency(cpFind, pto);

    CpCleanTask *cpClean = new CpCleanTask(d->preprocessingTmpDir->name(),
                                           cpFindPtoUrl,
                                           cpCleanPtoUrl,
                                           cpCleanPath);

    connect(cpClean, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(cpClean, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotDone(ThreadWeaver::Job*)));

    jobs->addJob(cpClean);
    DependencyPolicy::instance().addDependency(cpClean, cpFind);

    appendJob(jobs);
}

void ActionThread::optimizeProject(KUrl& ptoUrl, KUrl& optimizePtoUrl, bool levelHorizon,
                                   bool optimizeProjectionAndSize, const QString& autooptimiserPath)
{
    JobCollection       *jobs                       = new JobCollection();

    OptimisationTask *t = new OptimisationTask(d->preprocessingTmpDir->name(),
                                               ptoUrl,
                                               optimizePtoUrl,
                                               levelHorizon,
                                               optimizeProjectionAndSize,
                                               autooptimiserPath);

    connect(t, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(t, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotDone(ThreadWeaver::Job*)));

    jobs->addJob(t);

    appendJob(jobs);
}

void ActionThread::generatePanoramaPreview(const KUrl& ptoUrl, KUrl& previewPtoUrl, KUrl& previewMkUrl, KUrl& previewUrl,
                                           const ItemUrlsMap& preProcessedUrlsMap,
                                           const QString& makePath, const QString& pto2mkPath,
                                           const QString& enblendPath, const QString& nonaPath)
{
    JobCollection   *jobs                                   = new JobCollection();

    CreatePreviewTask *ptoTask = new CreatePreviewTask(d->preprocessingTmpDir->name(),
                                                       ptoUrl,
                                                       previewPtoUrl,
                                                       preProcessedUrlsMap);

    connect(ptoTask, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(ptoTask, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    jobs->addJob(ptoTask);

    appendStitchingJobs(ptoTask,
                        jobs,
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

    appendJob(jobs);
}

void ActionThread::compileProject(const PTOType& basePtoData, KUrl& panoPtoUrl, KUrl& mkUrl, KUrl& panoUrl,
                                  const ItemUrlsMap& preProcessedUrlsMap,
                                  PanoramaFileType fileType, const QRect& crop,
                                  const QString& makePath, const QString& pto2mkPath,
                                  const QString& enblendPath, const QString& nonaPath)
{
    JobCollection *jobs = new JobCollection();

    CreateFinalPtoTask *ptoTask = new CreateFinalPtoTask(d->preprocessingTmpDir->name(),
                                                         basePtoData,
                                                         panoPtoUrl,
                                                         crop);

    connect(ptoTask, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(ptoTask, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    jobs->addJob(ptoTask);

    appendStitchingJobs(ptoTask,
                        jobs,
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

    appendJob(jobs);
}

void ActionThread::copyFiles(const KUrl& ptoUrl, const KUrl& panoUrl, const KUrl& finalPanoUrl,
                             const ItemUrlsMap& preProcessedUrlsMap, bool savePTO)
{
    JobCollection   *jobs           = new JobCollection();

    CopyFilesTask *t = new CopyFilesTask(d->preprocessingTmpDir->name(),
                                         panoUrl,
                                         finalPanoUrl,
                                         ptoUrl,
                                         preProcessedUrlsMap,
                                         savePTO);

    connect(t, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(t, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotDone(ThreadWeaver::Job*)));

    jobs->addJob(t);

    appendJob(jobs);
}

void ActionThread::slotStarting(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = true;
    ad.action       = t->action;
    ad.id           = -1;

    if (t->action == NONAFILE)
    {
        CompileMKStepTask* c =  static_cast<CompileMKStepTask*>(j);
        ad.id = c->id;
    }
    else if (t->action == PREPROCESS_INPUT)
    {
        PreProcessTask* p = static_cast<PreProcessTask*>(j);
        ad.id = p->id;
    }

    emit starting(ad);
}

void ActionThread::slotStepDone(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = false;
    ad.action       = t->action;
    ad.id           = -1;
    ad.success      = t->success();
    ad.message      = t->errString;

    if (t->action == NONAFILE)
    {
        CompileMKStepTask* c =  static_cast<CompileMKStepTask*>(j);
        ad.id = c->id;
    }
    else if (t->action == PREPROCESS_INPUT)
    {
        PreProcessTask* p = static_cast<PreProcessTask*>(j);
        ad.id = p->id;
    }

    emit stepFinished(ad);

    ((QObject*) t)->deleteLater();
}

void ActionThread::slotDone(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = false;
    ad.action       = t->action;
    ad.id           = -1;
    ad.success      = t->success();
    ad.message      = t->errString;

    if (t->action == NONAFILE)
    {
        CompileMKStepTask* c =  static_cast<CompileMKStepTask*>(j);
        ad.id = c->id;
    }
    else if (t->action == PREPROCESS_INPUT)
    {
        PreProcessTask* p = static_cast<PreProcessTask*>(j);
        ad.id = p->id;
    }

    emit finished(ad);

    ((QObject*) t)->deleteLater();
}

void ActionThread::appendStitchingJobs(Job* prevJob, JobCollection* jc, const KUrl& ptoUrl, KUrl& mkUrl,
                                       KUrl& outputUrl, const ItemUrlsMap& preProcessedUrlsMap,
                                       PanoramaFileType fileType,
                                       const QString& makePath, const QString& pto2mkPath,
                                       const QString& enblendPath, const QString& nonaPath, bool preview)
{
    CreateMKTask *createMKTask = new CreateMKTask(d->preprocessingTmpDir->name(),
                                                  ptoUrl,
                                                  mkUrl,
                                                  outputUrl,
                                                  fileType,
                                                  pto2mkPath,
                                                  preview);

    connect(createMKTask, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(createMKTask, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    if (prevJob != 0)
    {
        DependencyPolicy::instance().addDependency(createMKTask, prevJob);
    }
    jc->addJob(createMKTask);

    QVector<CompileMKStepTask*> tasks;
    for (int i = 0; i < preProcessedUrlsMap.size(); i++)
    {
        CompileMKStepTask *t = new CompileMKStepTask(d->preprocessingTmpDir->name(),
                                                     i,
                                                     mkUrl,
                                                     nonaPath,
                                                     enblendPath,
                                                     makePath,
                                                     preview);

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotStarting(ThreadWeaver::Job*)));
        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotStepDone(ThreadWeaver::Job*)));

        DependencyPolicy::instance().addDependency(t, createMKTask);

        tasks.append(t);
        jc->addJob(t);
    }

    CompileMKTask *compileMKTask = new CompileMKTask(d->preprocessingTmpDir->name(),
                                                     mkUrl,
                                                     outputUrl,
                                                     nonaPath,
                                                     enblendPath,
                                                     makePath,
                                                     preview);

    foreach (CompileMKStepTask* t, tasks)
    {
        DependencyPolicy::instance().addDependency(compileMKTask, t);
    }

    connect(compileMKTask, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(compileMKTask, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotDone(ThreadWeaver::Job*)));

    jc->addJob(compileMKTask);
}

}  // namespace KIPIPanoramaPlugin
