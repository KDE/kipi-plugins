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
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QApplication>
#include <QDir>
#include <QMutexLocker>

// KDE includes.

#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes.

#include "utils.h"
#include "imagerotate.h"
#include "imageflip.h"
#include "convert2grayscale.h"
#include "actionthread.h"
#include "actionthread.moc"

namespace KIPIJPEGLossLessPlugin
{

ActionThread::ActionThread( KIPI::Interface* interface, QObject *parent)
            : QThread(parent), m_interface( interface )
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

void ActionThread::rotate(const KUrl::List& urlList, RotateAction val)
{
    for (KUrl::List::const_iterator it = urlList.begin();
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
        t->filePath  = (*it).path(); 
        t->action    = Rotate;
        t->rotAction = val;

        QMutexLocker lock(&m_mutex);
        m_todo << t;
        m_condVar.wakeAll();
    }
}

void ActionThread::flip(const KUrl::List& urlList, FlipAction val)
{
    for (KUrl::List::const_iterator it = urlList.begin();
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
        t->filePath   = (*it).path();
        t->action     = Flip;
        t->flipAction = val;

        QMutexLocker lock(&m_mutex);
        m_todo << t;
        m_condVar.wakeAll();
    }
}

void ActionThread::convert2grayscale(const KUrl::List& urlList)
{
    for (KUrl::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        Task *t     = new Task;
        t->filePath = (*it).path();
        t->action   = GrayScale;

        QMutexLocker lock(&m_mutex);
        m_todo << t;
        m_condVar.wakeAll();
    }
}

void ActionThread::cancel()
{
    QMutexLocker lock(&m_mutex);
    m_todo.clear();
    m_running = false;
    m_condVar.wakeAll();
}

void ActionThread::run()
{
    m_running = true;
    while (m_running)
    {
        Task *t = 0;
        {
            QMutexLocker lock(&m_mutex);
            if (!m_todo.isEmpty())
                t = m_todo.takeFirst();
            else
                m_condVar.wait(&m_mutex);
        }

        if (t)
        {
            QString errString;

            switch (t->action)
            {
                case Rotate:
                {
                    kDebug() << "Emitting starting" << endl;
                    emit starting(t->filePath, Rotate);

                    bool result = true;
                    ImageRotate imageRotate;
                    result = imageRotate.rotate(t->filePath, t->rotAction, m_tmpFolder, errString);

                    kDebug() << "Emitting finished/failed " << result << endl;
                    if (result)
                        emit finished(t->filePath, Rotate);
                    else
                        emit failed(t->filePath, Rotate, errString);
                    break;
                }
                case Flip:
                {
                    kDebug() << "Emitting starting" << endl;
                    emit starting(t->filePath, Flip);

                    ImageFlip imageFlip;
                    bool result = imageFlip.flip(t->filePath, t->flipAction, m_tmpFolder, errString);

                    kDebug() << "Emitting finished/failed " << result << endl;
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
                    bool result = imageGrayScale.image2GrayScale(t->filePath, m_tmpFolder, errString);

                    if (result)
                        emit finished(t->filePath, GrayScale);
                    else
                        emit failed(t->filePath, GrayScale, errString);
                    break;
                }
                default:
                {
                    kWarning( 51000 ) << "KIPIJPEGLossLessPlugin:ActionThread: "
                            << "Unknown action specified"
                            << endl;
                }
            }

            delete t;
        }
    }
}

}  // NameSpace KIPIJPEGLossLessPlugin
