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

#include <qapplication.h>
#include <qdir.h>
#include <qdeepcopy.h>

// KDE includes.

#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes.

#include "utils.h"
#include "imagerotate.h"
#include "imageflip.h"
#include "convert2grayscale.h"
#include "actionthread.h"

namespace KIPIJPEGLossLessPlugin
{

ActionThread::ActionThread( KIPI::Interface* interface, QObject *parent)
            : QThread(), m_parent(parent), m_interface( interface )
{
    // Create a KIPI JPEGLossLess plugin temporary folder in KDE tmp directory.
    KStandardDirs dir;
    m_tmpFolder = dir.saveLocation("tmp", "kipi-jpeglosslessplugin-" +
                                   QString::number(getpid()) + "/");
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // delete the temporary folder
    Utils::deleteDir(m_tmpFolder);
    // wait for the thread to finish
    wait();
}

void ActionThread::rotate(const KURL::List& urlList, RotateAction val)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        KIPI::ImageInfo info = m_interface->info( *it );

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

        Task *t      = new Task;
        t->filePath  = QDeepCopy<QString>((*it).path()); //deep copy
        t->action    = Rotate;
        t->rotAction = val;
        m_taskQueue.enqueue(t);
    }
}

void ActionThread::flip(const KURL::List& urlList, FlipAction val)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        KIPI::ImageInfo info = m_interface->info( *it );
        int angle = (info.angle() + 360) % 360;
    
        if ( (90-45 <= angle && angle < 90+45) || (270-45) < angle && angle < (270+45) ) 
        {
            // The image is rotated 90 or 270 degrees, which means that the flip operations 
            // must be switched to gain the effect the user expects.
            // Note: this will only work if the angles is one of 90,180,270.
            val = (FlipAction) !val;
        }

        Task *t       = new Task;
        t->filePath   = QDeepCopy<QString>((*it).path()); //deep copy
        t->action     = Flip;
        t->flipAction = val;
        m_taskQueue.enqueue(t);
    }
}

void ActionThread::convert2grayscale(const KURL::List& urlList)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        Task *t     = new Task;
        t->filePath = QDeepCopy<QString>((*it).path()); //deep copy
        t->action   = GrayScale;
        m_taskQueue.enqueue(t);
    }
}

void ActionThread::cancel()
{
    m_taskQueue.flush();
}

void ActionThread::run()
{
    while (!m_taskQueue.isEmpty()) 
    {
        Task *t = m_taskQueue.dequeue();
        if (!t) continue;

        QString errString;

        EventData *d = new EventData;

        switch (t->action) 
        {
            case Rotate : 
            {
                d->action   = Rotate;
                d->fileName = t->filePath;
                d->starting = true;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));
    
                bool result = true;
                ImageRotate imageRotate;
                result = imageRotate.rotate(t->filePath, t->rotAction, m_tmpFolder, errString);
    
                EventData *r = new EventData;
                r->action    = Rotate;
                r->fileName  = t->filePath;
                r->success   = result;
                r->errString = errString;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, r));
                break;
            }
            case Flip: 
            {
                d->action   = Flip;
                d->fileName = t->filePath;
                d->starting = true;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));
                
                ImageFlip imageFlip;
                bool result = imageFlip.flip(t->filePath, t->flipAction, m_tmpFolder, errString);
    
                EventData *r = new EventData;
                r->action    = Flip;
                r->fileName  = t->filePath;
                r->success   = result;
                r->errString = errString;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, r));
                break;
            }
            case GrayScale: 
            {
                d->action   = GrayScale;
                d->fileName = t->filePath;
                d->starting = true;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));
    
                ImageGrayScale imageGrayScale;
                bool result = imageGrayScale.image2GrayScale(t->filePath, m_tmpFolder, errString);
    
                EventData *r = new EventData;
                r->action    = GrayScale;
                r->fileName  = t->filePath;
                r->success   = result;
                r->errString = errString;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, r));
                break;
            }
            default: 
            {
                qDebug("KIPIJPEGLossLessPlugin:ActionThread: "
                       "Unknown action specified");
                delete d;
            }
        }

        delete t;
    }
}

}  // NameSpace KIPIJPEGLossLessPlugin
