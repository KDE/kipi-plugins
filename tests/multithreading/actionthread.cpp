/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-12-28
 * Description : re-implementation of action thread using threadweaver
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "actionthread.moc"

// Qt includes

#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>

// KDE includes

#include <kdebug.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>

// Local includes

#include "imagerotate.h"
#include "convert2grayscale.h"
#include "weaverobservertest.h"

using namespace ThreadWeaver;

class ActionThread::ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        running       = false;
        weaverRunning = false;
        log           = 0;
    }

    bool                  running;
    bool                  weaverRunning;

    QMutex                mutex;

    QWaitCondition        condVar;
    QWaitCondition        condVarJobs;

    QList<JobCollection*> todo;

    Weaver                weaver;
    WeaverObserverTest*   log;
};

class Task : public Job
{
public:

    Task(QObject* parent = 0)
        :Job(parent)
    {
    }

    KUrl                                 fileUrl;
    QString                              errString;
    KIPIJPEGLossLessPlugin::Action       action;
    KIPIJPEGLossLessPlugin::RotateAction rotAction;

protected:

    void run()
    {
        switch (action)
        {
            case KIPIJPEGLossLessPlugin::Rotate:
            {
                KIPIJPEGLossLessPlugin::ImageRotate imageRotate;
                imageRotate.rotate(fileUrl.toLocalFile(), rotAction, errString, false);

                break;
            }
            case KIPIJPEGLossLessPlugin::Flip:
            {
                kDebug() << "Flip action not managed in this test program...";

                break;
            }
            case KIPIJPEGLossLessPlugin::GrayScale:
            {
                KIPIJPEGLossLessPlugin::ImageGrayScale imageGrayScale;
                imageGrayScale.image2GrayScale(fileUrl.toLocalFile(), errString, false);

                break;
            }
        }
    }
};

ActionThread::ActionThread(QObject* parent)
    : QThread(parent),d(new ActionThreadPriv)
{
    const int maximumNumberOfThreads = 4;
    d->log                           = new WeaverObserverTest(this);
    d->weaver.registerObserver(d->log);
    d->weaver.setMaximumNumberOfThreads(maximumNumberOfThreads);
    kDebug() << "Starting Main Thread";
}

ActionThread::~ActionThread()
{
    kDebug() << "calling action thread destructor";
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d->log;
    delete d;

    kDebug() << "Ending Main Thread";
}

void ActionThread::slotFinished()
{
    d->weaverRunning = false;
    d->condVarJobs.wakeAll();
}

void ActionThread::slotJobDone(ThreadWeaver::Job *job)
{
    Task* task = static_cast<Task*>(job);

    if(task->errString.isEmpty())
    {
        kDebug() << "Job done:" << task->fileUrl.toLocalFile() << endl;
        emit signalEndToProcess(task->fileUrl, true);
    }
    else
    {
        kDebug() << "could n't complete the job: " << task->fileUrl.toLocalFile() << " Error: " << task->errString << endl;
        emit signalEndToProcess(task->fileUrl, false);
    }

    delete job;
}

void ActionThread::slotJobStarted(ThreadWeaver::Job *job)
{
    Task* task = static_cast<Task*>(job);
    kDebug() << "Job Started:" << task->fileUrl.toLocalFile() << endl;
    emit signalStartToProcess(task->fileUrl);
}

void ActionThread::rotate(const KUrl::List& urlList, KIPIJPEGLossLessPlugin::RotateAction val)
{
    JobCollection* collection = new JobCollection(this);

    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        Task* t      = new Task;
        t->fileUrl   = *it;
        t->action    = KIPIJPEGLossLessPlugin::Rotate;
        t->rotAction = val;

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotJobStarted(ThreadWeaver::Job*)));

        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotJobDone(ThreadWeaver::Job*)));

        collection->addJob(t);
    }

    QMutexLocker lock(&d->mutex);
    d->todo << collection;
    d->condVar.wakeAll();
}

void ActionThread::convert2grayscale(const KUrl::List& urlList)
{
    JobCollection* collection = new JobCollection(this);

    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        Task* t    = new Task;
        t->fileUrl = *it;
        t->action  = KIPIJPEGLossLessPlugin::GrayScale;

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotJobStarted(ThreadWeaver::Job*)));

        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotJobDone(ThreadWeaver::Job*)));

        collection->addJob(t);
    }

    QMutexLocker lock(&d->mutex);
    d->todo << collection;
    d->condVar.wakeAll();
}

void ActionThread::cancel()
{
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->running       = false;
    d->weaverRunning = true;
    d->weaver.dequeue();
    d->condVar.wakeAll();
    d->condVarJobs.wakeAll();
}

void ActionThread::run()
{
    d->running = true;
    kDebug() << "In action thread Run";

    while (d->running)
    {
        JobCollection* t = 0;
        {
            QMutexLocker lock(&d->mutex);
            if (!d->todo.isEmpty())
            {
                if (!d->weaverRunning)
                {
                    t = d->todo.takeFirst();
                }
                else
                {
                    d->condVarJobs.wait(&d->mutex);
                }
            }
            else
            {
                d->condVar.wait(&d->mutex);
            }
        }

        if (t)
        {
            connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                    this, SLOT(slotFinished()));

            connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                    t, SLOT(deleteLater()));

            d->weaver.enqueue(t);
            d->weaverRunning = true;
        }
    }

    d->weaver.finish();
    kDebug() << "Exiting Action Thread";
}
