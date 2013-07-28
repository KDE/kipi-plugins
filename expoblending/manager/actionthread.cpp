/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
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

#include "actionthread.moc"


// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QPointer>
#include <QLabel>
#include <QPixmap>
#include <QGroupBox>

// KDE includes
#include <kstandarddirs.h>
#include <kvbox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ktempdir.h>

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <threadweaver/DependencyPolicy.h>


// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "task.h"
#include "tasks.h"
#include "kpmetadata.h"
#include "kpversion.h"
#include "kpwriteimage.h"

#include <iostream>

using namespace std;
using namespace ThreadWeaver;
using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class ActionThread::ActionThreadPriv
{
  
public:

    ActionThreadPriv()
        : preprocessingTmpDir(0)
    {
        align               = true;
        cancel              = false;
        enfuseVersion4x     = true;
        preprocessingTmpDir = 0;
    }
  
    bool                             cancel;
    bool                             align;
    bool                             enfuseVersion4x;

    RawDecodingSettings 	     rawDecodingSettings;
    
    KTempDir*                        preprocessingTmpDir;

  
    /**
     * List of results files produced by enfuse that may need cleaning.
     * Only access this through the provided mutex.
     */
    KUrl::List                       enfuseTmpUrls;
    KUrl::List                       urls;
    QMutex                           enfuseTmpUrlsMutex;
    QMutex 			     mutex;
    QWaitCondition                   condVar;

    QList<Task*>                     todo;

    KProcess*                        enfuseProcess;
    KProcess*                        alignProcess;
   
    void cleanPreprocessingTmpDir()
    {
        if (preprocessingTmpDir)
        {
            preprocessingTmpDir->unlink();
            delete preprocessingTmpDir;
            preprocessingTmpDir = 0;
        }
    }
    
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


ActionThread::ActionThread(QObject* const parent)
    : RActionThreadBase(parent), d(new ActionThreadPriv)
{
    qRegisterMetaType<ActionData>();
}    
/*
ActionThread::~ActionThread()
{

    kDebug() << "ActionThread shutting down."
             << "Canceling all actions and waiting for them";

    //cancel the thread
    cancel();
    //wait for the thread to finish
    wait();
    
    kDebug() << "Thread finished";

    d->cleanAlignTmpDir();

    cleanUpResultFiles();         

    delete d;
}
*/
void ActionThread::setEnfuseVersion(const double version)
{
    d->enfuseVersion4x = (version >= 4.0);
}

void ActionThread::setPreProcessingSettings(bool align, const RawDecodingSettings& settings)
{
    d->align               = align;
    d->rawDecodingSettings = settings;
}

void ActionThread::identifyFiles(const KUrl::List& urlList)
{
    JobCollection* const jobs = new JobCollection();
    
    foreach(const KUrl& url, urlList)    
    {
        d->urls.clear();
	d->urls.append(url);
	
        GenericTask* const t = new GenericTask(this, d->urls, IDENTIFY);

	connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotStarting(ThreadWeaver::Job*)));
 
	connect(t, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
                this, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)));

	connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotStepDone(ThreadWeaver::Job*)));
      
        jobs->addJob(t);
    }
    appendJob(jobs);
    
}

void ActionThread::loadProcessed(const KUrl& url)
{
  
    JobCollection* const jobs = new JobCollection();
    KUrl::List tempList;
    
    tempList.append(url);
    
    GenericTask* const t = new GenericTask(this, tempList , LOAD);
   
    connect(t, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)));
    
    connect(t, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    
    connect(t, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)));

    connect(t, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));
   
    
    jobs->addJob(t);
    appendJob(jobs);
}

void ActionThread::enfusePreview(const KUrl::List& alignedUrls, const KUrl& outputUrl,
                                 const EnfuseSettings& settings, const QString& enfusePath)
{
  
    JobCollection   *jobs  = new JobCollection();
   
    GenericTask* const t = new GenericTask(this, alignedUrls, ENFUSEPREVIEW,
					   outputUrl, settings, enfusePath, d->enfuseVersion4x); 
     

    connect(t, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)));
    
    connect(t, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
  
    connect(t, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)));
    
    connect(t, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));
   
    
    jobs->addJob(t);

    appendJob(jobs);
}

void ActionThread::enfuseFinal(const KUrl::List& alignedUrls, const KUrl& outputUrl,
                               const EnfuseSettings& settings, const QString& enfusePath)
{
  
    JobCollection   *jobs  = new JobCollection();
    
    GenericTask* const t = new GenericTask(this, alignedUrls, ENFUSEFINAL,
					   outputUrl, settings, enfusePath, d->enfuseVersion4x); 
    
    connect(t, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)));
    
    connect(t, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));

    connect(t, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)));

    connect(t, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));
   
    
    jobs->addJob(t);

    appendJob(jobs);
   
}

void ActionThread::preProcessFiles(const KUrl::List& urlList, const QString& alignPath)
{
    startPreProcessing( urlList, d->align, d->rawDecodingSettings, alignPath);
}

void ActionThread::startPreProcessing(const KUrl::List& inUrls,
                                      bool align, const RawDecodingSettings& rawSettings,
                                      const QString& alignPath)
{
    d->cleanPreprocessingTmpDir();

    QString prefix = KStandardDirs::locateLocal("tmp", QString("kipi-expoblending-tmp-") +
                                                       QString::number(QDateTime::currentDateTime().toTime_t()));

    d->preprocessingTmpDir = new KTempDir(prefix);

    ItemUrlsMap preProcessedMap;
    JobCollection       *jobs           = new JobCollection();

    // TODO: try to append these jobs as a JobCollection inside a JobSequence
    int id = 0;
    
    QVector<PreProcessTask*> preProcessingTasks;
    
    foreach (const KUrl& file, inUrls)
    {
        preProcessedMap.insert(file, ItemPreprocessedUrls());

        PreProcessTask *t = new PreProcessTask(d->preprocessingTmpDir->name(),
                                               id++,
                                               preProcessedMap[file],
                                               file,
                                               rawSettings,
					       inUrls,
					       alignPath,
					       align);

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotStarting(ThreadWeaver::Job*)));
	
	connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotStepDone(ThreadWeaver::Job*)));

        preProcessingTasks.append(t);
        jobs->addJob(t);
    }
}
void ActionThread::slotStarting(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = true;
    ad.action       = t->action;

    emit starting(ad);
}

void ActionThread::slotStepDone(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = false;
    ad.action       = t->action;
    
    emit stepFinished(ad);

    ((QObject*) t)->deleteLater();
}

void ActionThread::slotDone(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = false;
    ad.action       = t->action;
   
    emit finished(ad);

    ((QObject*) t)->deleteLater();
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

    
    d->condVar.wakeAll();
}

void ActionThread::cleanUpResultFiles()
{
    // Cleanup all tmp files created by Enfuse process.
    QMutexLocker(&d->enfuseTmpUrlsMutex);
    foreach(const KUrl& url, d->enfuseTmpUrls)
    {
        kDebug() << "Removing temp file " << url.toLocalFile();
        KTempDir::removeDir(url.toLocalFile());
    }
    d->enfuseTmpUrls.clear();
}


}  // namespace KIPIExpoBlendingPlugin
