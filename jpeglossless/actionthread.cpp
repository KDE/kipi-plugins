/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage JPEGLossLess plugin
 *               actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "utils.h"
#include "imagerotate.h"
#include "imageflip.h"
#include "convert2grayscale.h"

namespace KIPIJPEGLossLessPlugin
{

class ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        interface           = 0;
        running             = false;
        updateFileTimeStamp = false;
    }

    class Task
    {
        public:

            QString      filePath;
            Action       action;
            RotateAction rotAction;
            FlipAction   flipAction;
    };

    bool             running;
    bool             updateFileTimeStamp;

    QMutex           mutex;

    QWaitCondition   condVar;

    QList<Task*>     todo;

    KIPI::Interface *interface;
};

ActionThread::ActionThread(KIPI::Interface* interface, QObject *parent)
            : QThread(parent), d(new ActionThreadPriv)
{
    d->interface = interface;
    if (d->interface)
        d->updateFileTimeStamp = d->interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool();
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void ActionThread::rotate(const KUrl::List& urlList, RotateAction val)
{
    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        KIPI::ImageInfo info = d->interface->info( *it );

        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->filePath               = (*it).path();
        t->action                 = Rotate;
        t->rotAction              = val;

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
    }
}

void ActionThread::flip(const KUrl::List& urlList, FlipAction val)
{
    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        KIPI::ImageInfo info = d->interface->info( *it );
        int angle = (info.angle() + 360) % 360;

        if ( ((90-45) <= angle && angle < (90+45)) ||
             ((270-45) < angle && angle < (270+45)) )
        {
            // The image is rotated 90 or 270 degrees, which means that the flip operations
            // must be switched to gain the effect the user expects.
            // Note: this will only work if the angles is one of 90,180,270.
            val = (FlipAction) !val;
        }

        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->filePath               = (*it).path();
        t->action                 = Flip;
        t->flipAction             = val;

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
    }
}

void ActionThread::convert2grayscale(const KUrl::List& urlList)
{
    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->filePath               = (*it).path();
        t->action                 = GrayScale;

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
    }
}

void ActionThread::cancel()
{
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->running = false;
    d->condVar.wakeAll();
}

void ActionThread::run()
{
    d->running = true;
    while (d->running)
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
            QString errString;

            switch (t->action)
            {
                case Rotate:
                {
                    emit starting(t->filePath, Rotate);

                    bool result = true;
                    ImageRotate imageRotate;
                    result = imageRotate.rotate(t->filePath, t->rotAction, errString, d->updateFileTimeStamp);

                    if (result)
                        emit finished(t->filePath, Rotate);
                    else
                        emit failed(t->filePath, Rotate, errString);
                    break;
                }
                case Flip:
                {
                    emit starting(t->filePath, Flip);

                    ImageFlip imageFlip;
                    bool result = imageFlip.flip(t->filePath, t->flipAction, errString, d->updateFileTimeStamp);

                    if (result)
                        emit finished(t->filePath, Flip);
                    else
                        emit failed(t->filePath, Flip, errString);
                    break;
                }
                case GrayScale:
                {
                    emit starting(t->filePath, GrayScale);

                    ImageGrayScale imageGrayScale;
                    bool result = imageGrayScale.image2GrayScale(t->filePath, errString, d->updateFileTimeStamp);

                    if (result)
                        emit finished(t->filePath, GrayScale);
                    else
                        emit failed(t->filePath, GrayScale, errString);
                    break;
                }
                default:
                {
                    kError() << "KIPIJPEGLossLessPlugin:ActionThread: "
                                    << "Unknown action specified";
                }
            }

            delete t;
        }
    }
}

}  // namespace KIPIJPEGLossLessPlugin
