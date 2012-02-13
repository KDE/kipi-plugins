/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage JPEGLossLess plugin
 *               actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>

// KDE includes

#include <kdebug.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kphostsettings.h"
#include "utils.h"
#include "imagerotate.h"
#include "imageflip.h"
#include "convert2grayscale.h"

using namespace KIPIPlugins;

namespace KIPIJPEGLossLessPlugin
{

class ActionThread::Task : public ThreadWeaver::Job
{
public:

    Task(QObject* parent = 0, bool updateFileStamp = true)
        :Job(parent)
    {
        this->updateFileStamp = updateFileStamp;
    }

    bool         updateFileStamp;

    QString      errString;

    KUrl         fileUrl;

    Action       action;
    RotateAction rotAction;
    FlipAction   flipAction;

protected:

    void run()
    {
        switch (action)
        {
            case KIPIJPEGLossLessPlugin::Rotate:
            {
                KIPIJPEGLossLessPlugin::ImageRotate imageRotate;
                imageRotate.rotate(fileUrl.toLocalFile(), rotAction, errString, updateFileStamp);

                break;
            }
            case KIPIJPEGLossLessPlugin::Flip:
            {

                ImageFlip imageFlip;
                imageFlip.flip(fileUrl.toLocalFile(), flipAction, errString, updateFileStamp);

            }
            case KIPIJPEGLossLessPlugin::GrayScale:
            {

                KIPIJPEGLossLessPlugin::ImageGrayScale imageGrayScale;
                imageGrayScale.image2GrayScale(fileUrl.toLocalFile(), errString, updateFileStamp);


                break;
            }
            default:
            {
                kError() << "KIPIJPEGLossLessPlugin:ActionThread: "
                         << "Unknown action specified";
            }
        }
    }
};

ActionThread::ActionThread(KIPI::Interface* interface, QObject* parent)
    : ActionThreadBase(parent)
{
    interface = interface;
    if (interface)
    {
        KPHostSettings hSettings(interface);
        updateFileStamp = hSettings.metadataSettings().updateFileTimeStamp;
    }
}

ActionThread::~ActionThread()
{
}

void ActionThread::rotate(const KUrl::List& urlList, RotateAction val)
{

    ThreadWeaver::JobCollection* collection = new ThreadWeaver::JobCollection(this);

    for (KUrl::List::const_iterator it = urlList.constBegin();
            it != urlList.constEnd(); ++it )
    {
        Task* t      = new Task(this,updateFileStamp);
        t->fileUrl   = *it;
        t->action    = Rotate;
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

void ActionThread::flip(const KUrl::List& urlList, FlipAction val)
{
    ThreadWeaver::JobCollection* collection = new ThreadWeaver::JobCollection(this);

    for (KUrl::List::const_iterator it = urlList.constBegin();
            it != urlList.constEnd(); ++it )
    {
        Task* t      = new Task(this,updateFileStamp);
        t->fileUrl   = *it;
        t->action    = Flip;
        t->flipAction = val;

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
    ThreadWeaver::JobCollection* collection = new ThreadWeaver::JobCollection(this);

    for (KUrl::List::const_iterator it = urlList.constBegin();
            it != urlList.constEnd(); ++it )
    {
        ActionThread::Task* t    = new Task(this,updateFileStamp);
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

void ActionThread::slotJobDone(ThreadWeaver::Job *job)
{
    Task* task = static_cast<Task*>(job);

    if (task->errString.isEmpty())
    {
        kDebug() << "Job done:" << task->fileUrl.toLocalFile() << endl;
        emit finished(task->fileUrl.toLocalFile(),task->action);
    }
    else
    {
        kDebug() << "could n't complete the job: " << task->fileUrl.toLocalFile() << " Error: " << task->errString << endl;
        emit failed(task->fileUrl.toLocalFile(),task->action,task->errString);
    }

    delete job;
}

void ActionThread::slotJobStarted(ThreadWeaver::Job *job)
{
    Task* task = static_cast<Task*>(job);
    kDebug() << "Job Started:" << task->fileUrl.toLocalFile() << endl;
    emit starting(task->fileUrl.toLocalFile(),task->action);
}

}  // namespace KIPIJPEGLossLessPlugin
