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
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Local includes

#include "kphostsettings.h"
#include "utils.h"
#include "imagerotate.h"
#include "imageflip.h"
#include "convert2grayscale.h"

using namespace ThreadWeaver;

namespace KIPIJPEGLossLessPlugin
{

class ActionThread::Task : public Job
{
public:

    Task(QObject* const parent = 0)
        :Job(parent)
    {
        action     = Rotate;
        rotAction  = Rot0;
        flipAction = FlipHorizontal;
    }

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
            case Rotate:
            {
                ImageRotate imageRotate;
                imageRotate.rotate(fileUrl.toLocalFile(), rotAction, errString);

                break;
            }
            case Flip:
            {

                ImageFlip imageFlip;
                imageFlip.flip(fileUrl.toLocalFile(), flipAction, errString);

                break;
            }
            case GrayScale:
            {

                ImageGrayScale imageGrayScale;
                imageGrayScale.image2GrayScale(fileUrl.toLocalFile(), errString);

                break;
            }
            default:
            {
                kError() << "Unknown action specified";
                break;
            }
        }
    }
};

// ----------------------------------------------------------------------------------------------------

ActionThread::ActionThread(QObject* const parent)
    : RActionThreadBase(parent)
{
}

ActionThread::~ActionThread()
{
}

void ActionThread::rotate(const KUrl::List& urlList, RotateAction val)
{
    JobCollection* const collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it )
    {
        Task* const t = new Task(this);
        t->fileUrl    = *it;
        t->action     = Rotate;
        t->rotAction  = val;

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotJobStarted(ThreadWeaver::Job*)));

        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotJobDone(ThreadWeaver::Job*)));

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::flip(const KUrl::List& urlList, FlipAction val)
{
    JobCollection* const collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it )
    {
        Task* const t = new Task(this);
        t->fileUrl    = *it;
        t->action     = Flip;
        t->flipAction = val;

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotJobStarted(ThreadWeaver::Job*)));

        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotJobDone(ThreadWeaver::Job*)));

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::convert2grayscale(const KUrl::List& urlList)
{
    JobCollection* const collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it )
    {
        ActionThread::Task* const t = new Task(this);
        t->fileUrl                  = *it;
        t->action                   = GrayScale;

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotJobStarted(ThreadWeaver::Job*)));

        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotJobDone(ThreadWeaver::Job*)));

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::slotJobDone(ThreadWeaver::Job* job)
{
    Task* const task = static_cast<Task*>(job);

    if (task->errString.isEmpty())
    {
        kDebug() << "Job done:" << task->fileUrl.toLocalFile();
        emit finished(task->fileUrl, task->action);
    }
    else
    {
        kDebug() << "Could not complete the job: " << task->fileUrl.toLocalFile() << " Error: " << task->errString;
        emit failed(task->fileUrl, task->action,task->errString);
    }

    delete job;
}

void ActionThread::slotJobStarted(ThreadWeaver::Job* job)
{
    Task* const task = static_cast<Task*>(job);
    kDebug() << "Job Started:" << task->fileUrl.toLocalFile();
    emit starting(task->fileUrl, task->action);
}

}  // namespace KIPIJPEGLossLessPlugin
