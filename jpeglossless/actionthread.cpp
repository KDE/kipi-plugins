/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage JPEGLossLess plugin 
 *               actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2004-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

// C Ansi includes.

extern "C"
{
#include <unistd.h>
}

// Qt includes.

#include <QMutexLocker>
#include <QtDebug>
#include <QMutex>
#include <QWaitCondition>

// LibKipi includes.

#include <libkipi/interface.h>

// Local includes.

#include "utils.h"
#include "imagerotate.h"
#include "imageflip.h"
#include "convert2grayscale.h"
#include "actionthread.h"
#include "actionthread.moc"

namespace KIPIJPEGLossLessPlugin
{

class ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        interface = 0;
        running   = false;
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

    QMutex           mutex;

    QWaitCondition   condVar;

    QList<Task*>     todo;

    KIPI::Interface *interface;
};

ActionThread::ActionThread( KIPI::Interface* interface, QObject *parent)
            : QThread(parent)
{
    d = new ActionThreadPriv;
    d->interface = interface;
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
    for (KUrl::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        KIPI::ImageInfo info = d->interface->info( *it );

        /*
        Removing this code:
         - for JPEGs, jpegtransform is using the Exiv2Iface and matrix multiplication
           to compute the mathematically correct rotation (taking flip operations into account,
           the code below only angles). Metadata tag is reset.
         - For ImageMagick, no metadata is taken into account, and no metadata is changed!
           Angle from host application still applies.
        // Don't use the host angle in case of auto-rotation (Rot0)
        if (val != Rot0)
        {
            // If the image is being displayed rotaed in the host application, then rotate that
            // angle too.
            int angle = (info.angle() + 360) % 360;

            // When the image has been rotated on the disk we can assume that it
            // does not need to be rotated before being displayed.
            info.setAngle( 0 );
          
            if ( val == Rot90 )
                angle += 90;
            else if ( val == Rot180 )
                angle += 180;
            else if ( val == Rot270 )
                angle += 270;

            angle = (angle+360) % 360;
            if ( (90-45) <= angle && angle < (90+45) )
                val = Rot90;
            else if ( (180-45) <= angle && angle < (180+45) )
                val = Rot180;
            else if ( (270-45) <= angle && angle < (270+45) )
                val = Rot270;
            else
                val = Rot0;
        }
        */

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
    for (KUrl::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        KIPI::ImageInfo info = d->interface->info( *it );
        int angle = (info.angle() + 360) % 360;

        if ( (90-45 <= angle && angle < 90+45) ||
             (270-45) < angle && angle < (270+45) ) 
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
    for (KUrl::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
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
                    result = imageRotate.rotate(t->filePath, t->rotAction, errString);

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
                    bool result = imageFlip.flip(t->filePath, t->flipAction, errString);

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
                    bool result = imageGrayScale.image2GrayScale(t->filePath, errString);

                    if (result)
                        emit finished(t->filePath, GrayScale);
                    else
                        emit failed(t->filePath, GrayScale, errString);
                    break;
                }
                default:
                {
                    qCritical() << "KIPIJPEGLossLessPlugin:ActionThread: "
                                << "Unknown action specified"
                                << endl;
                }
            }

            delete t;
        }
    }
}

}  // NameSpace KIPIJPEGLossLessPlugin
