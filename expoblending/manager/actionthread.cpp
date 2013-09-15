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
//#include <QDebug>
#include <qdebug.h>

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
#include <threadweaver/JobSequence.h>


// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "task.h"
#include "tasks.h"
#include "kpmetadata.h"
#include "kpversion.h"
#include "kpwriteimage.h"

using namespace ThreadWeaver;
using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class ActionThread::ActionThreadPriv
{
  
public:

    ActionThreadPriv()
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

    KUrl::List                       urls;
    EvUrlsMap                        exposureValuesMapt;
    
    QString                          name;
    
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

ActionThread::~ActionThread()
{

    kDebug() << "ActionThread shutting down."
             << "Canceling all actions and waiting for them";

    d->cleanAlignTmpDir();        

    delete d;
}

void ActionThread::cleanUpResultFiles()
{}

void ActionThread::setEnfuseVersion(const double version)
{
    d->enfuseVersion4x = (version >= 4.0);
}

void ActionThread::setPreProcessingSettings(bool align, const RawDecodingSettings& settings)
{
    d->align               = align;
    d->rawDecodingSettings = settings;
}

void ActionThread::identifyFiles(const KUrl::List& urlList, EvUrlsMap& exposureValuesMap)
{
    JobCollection* const jobs = new JobCollection();
    
    foreach(const KUrl& url, urlList)    
    {
        d->urls.clear();
        d->urls.append(url);

	exposureValuesMap.insert(url, EvValueUrls());

        GenericTask* const t = new GenericTask(this, d->urls, IDENTIFY, exposureValuesMap[url]);
         
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

void ActionThread::preProcessFiles(const KUrl::List& urlList, const QString& alignPath)
{
    JobCollection* const jobs = new JobCollection();
    d->urls = urlList;
    
    PreProcessTask* const t = new PreProcessTask(this, d->urls, d->rawDecodingSettings, d->align, alignPath);

    connect (t, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
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

void ActionThread::loadProcessed(const KUrl& url)
{
  
    JobCollection* const jobs = new JobCollection();
    KUrl::List tempList;
    
    tempList.append(url);
    
    GenericTask* const t = new GenericTask(this, tempList , LOAD);
   
    connect (t, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
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
   
    EnfusePreviewTask* const t = new EnfusePreviewTask(this, alignedUrls,outputUrl, 
						       settings, enfusePath, d->enfuseVersion4x); 
     
    
    connect (t, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
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
    
    EnfuseFinalTask* const t = new EnfuseFinalTask(this, alignedUrls, outputUrl, 
						   settings, enfusePath, d->enfuseVersion4x); 
    
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

void ActionThread::hdrGen(const KUrl::List& urlList, QString& name, const PfsHdrSettings& settings, int option)
{                     
    JobCollection   *jobs           = new JobCollection();
    
    HdrGenTask* const pfshdrscript = new HdrGenTask(this, urlList, name, settings, option);

    connect(pfshdrscript, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)));  
    connect(pfshdrscript, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(pfshdrscript, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)));
    connect(pfshdrscript, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    jobs->addJob(pfshdrscript);
    appendJob(jobs);
}

void ActionThread::hdrCalibrate(const KUrl::List& urlList, const QString& name, const PfsHdrSettings& settings, int option)
{
    JobCollection   *jobs  = new JobCollection();
    
    CameraResponseTask* responsecurve = new CameraResponseTask(this, urlList, name, settings, option);

    connect(responsecurve, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)));  
    connect(responsecurve, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(responsecurve, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)));
    connect(responsecurve, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    jobs->addJob(responsecurve);
    appendJob(jobs);
}

void ActionThread::hdrOutExrPreview(const KUrl::List& urlList,const QString& name, 
				    const KUrl& outputUrl, const PfsHdrSettings& settings)
{
    JobCollection   *jobs  = new JobCollection();
    
    HdrCalibratePreviewTask* calibratepreview = new HdrCalibratePreviewTask(this, urlList, name, settings, outputUrl);
     
    connect(calibratepreview, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)));    
    connect(calibratepreview, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(calibratepreview, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)));
    connect(calibratepreview, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));
    
    jobs->addJob(calibratepreview);
    
    appendJob(jobs);
}

void ActionThread::hdrOutExrFinal(const KUrl::List& urlList,const QString& name, 
				  const KUrl& outputUrl, const PfsHdrSettings& settings)
{
    JobCollection   *jobs  = new JobCollection();
    
    HdrCalibrateFinalTask* calibratefinal = new HdrCalibrateFinalTask(this, urlList, name, settings, outputUrl);
     
    connect(calibratefinal, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)));    
    connect(calibratefinal, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(calibratefinal, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)));
    connect(calibratefinal, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));
    
    jobs->addJob(calibratefinal);
    
    appendJob(jobs);
}

void ActionThread::slotStarting(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = true;
    ad.action       = t->action;
    ad.id	    = -1;
    
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
    
    emit stepFinished(ad);

    ((QObject*) t)->deleteLater();
}

void ActionThread::slotDone(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = false;
    ad.action       = t->action;
    ad.success      = t->success();
    ad.message      = t->errString;
    
    emit finished(ad);

    ((QObject*) t)->deleteLater();
}

void ActionThread::cancel()
{
    d->cancel = true;  
}


}  // namespace KIPIExpoBlendingPlugin
