/* ============================================================
 * File  : actionthread.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-12-04
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <kdebug.h>
#include <kstandarddirs.h>
#include <qapplication.h>
#include <qdir.h>

extern "C"
{
#include <unistd.h>
}

#include "imagerotate.h"
#include "imageflip.h"
#include "convert2grayscale.h"
#include "actionthread.h"

namespace JPEGLossLess
{

ActionThread::ActionThread( KIPI::Interface* interface, QObject *parent)
    : QThread(), parent_(parent), interface_( interface )
{
    // Create a JPEGLossLess plugin temporary folder in KDE tmp directory.
    KStandardDirs dir;
    tmpFolder_ = dir.saveLocation("tmp", "kipi-jpeglossless-" +
                                  QString::number(getpid()) + "/");
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // delete the temporary folder
    deleteDir(tmpFolder_);
    // wait for the thread to finish
    wait();
}

void ActionThread::rotate(const KURL::List& urlList, RotateAction val)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) {
        KIPI::ImageInfo info = interface_->info( *it );

        // If the image is being displayed rotaed in the host application, then rotate that
        // angle too.
        int angle = (info.angle() + 360) % 360;

        // When the image has been rotated on the disk we can assume that it
        // does not need to be rotated before being displayed.
        info.setAngle( 0 );

        if ( val == Rot90 )
            angle +=90;
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

        Task *t      = new Task;
        t->filePath  = (*it).path().latin1(); //deep copy
        t->action    = Rotate;
        t->rotAction = val;
        taskQueue_.enqueue(t);
    }
}

void ActionThread::flip(const KURL::List& urlList, FlipAction val)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) {
        KIPI::ImageInfo info = interface_->info( *it );
        int angle = (info.angle() + 360) % 360;
        if ( (90-45 <= angle && angle < 90+45) || (270-45) < angle && angle < (270+45) ) {
            // The image is rotated 90 or 270 degrees, which means that the flip operations must be switched to
            // gain the effect the user expects.
            // Note this will only work if the angles is one of 90,180,270.
            val = (FlipAction) !val;
        }

        Task *t       = new Task;
        t->filePath   = (*it).path().latin1(); //deep copy
        t->action     = Flip;
        t->flipAction = val;
        taskQueue_.enqueue(t);
    }
}

void ActionThread::convert2grayscale(const KURL::List& urlList)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) {
        Task *t      = new Task;
        t->filePath  = (*it).path().latin1(); //deep copy
        t->action    = GrayScale;
        taskQueue_.enqueue(t);
    }
}


void ActionThread::cancel()
{
    taskQueue_.flush();
}

void ActionThread::run()
{
    while (!taskQueue_.isEmpty()) {

        Task *t = taskQueue_.dequeue();
        if (!t) continue;

        QString errString;

        EventData *d = new EventData;

        switch (t->action) {
        case(Rotate): {
            d->action   = Rotate;
            d->fileName = t->filePath;
            d->starting = true;
            QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));

            bool result = true;
            result = JPEGLossLess::rotate(t->filePath, t->rotAction,
                                              tmpFolder_, errString);

            EventData *r = new EventData;
            r->action    = Rotate;
            r->fileName  = t->filePath;
            r->success   = result;
            r->errString = errString;
            QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, r));
            break;
        }
        case(Flip): {
            d->action   = Flip;
            d->fileName = t->filePath;
            d->starting = true;
            QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));

            bool result = JPEGLossLess::flip(t->filePath, t->flipAction,
                                               tmpFolder_, errString);

            EventData *r = new EventData;
            r->action    = Flip;
            r->fileName  = t->filePath;
            r->success   = result;
            r->errString = errString;
            QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, r));
            break;
        }
        case(GrayScale): {
            d->action   = GrayScale;
            d->fileName = t->filePath;
            d->starting = true;
            QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));

            bool result = JPEGLossLess::image2GrayScale(t->filePath,
                                                        tmpFolder_, errString);

            EventData *r = new EventData;
            r->action    = GrayScale;
            r->fileName  = t->filePath;
            r->success   = result;
            r->errString = errString;
            QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, r));
            break;
        }

        default: {
            kdWarning( 51000 ) << "JPEGLossLess:ActionThread: "
                        << "Unknown action specified"
                        << endl;
            delete d;
        }
        }

        delete t;
    }

}

void ActionThread::deleteDir(const QString& dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) return;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);

    const QFileInfoList* infoList = dir.entryInfoList();
    if (!infoList) return;
    QFileInfoListIterator it(*infoList);
    QFileInfo* fi;

    while( (fi = it.current()) ) {
        ++it;
        if(fi->fileName() == "." || fi->fileName() == ".." )
            continue;

        if( fi->isDir() ) {
            deleteDir(fi->absFilePath());
        }
        else if( fi->isFile() )
            dir.remove(fi->absFilePath());

    }

    dir.rmdir(dir.absPath());
}

}
