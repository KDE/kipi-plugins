/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-09
 * Description : a class to manage actions using threads
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QImageReader>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>
#include <QtDebug>
#include <QMutex>
#include <QWaitCondition>

// KDE includes.

#include <klocale.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/kdcraw.h>

// Local includes.

#include "pluginsversion.h"
#include "actionthread.h"
#include "actionthread.moc"

namespace KIPISendimagesPlugin
{

class ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        count   = 0;
        running = false;
    }

    class Task
    {
        public:

            KUrl                   fileUrl;
            QString                destName;
            EmailSettingsContainer settings;
            
    };

    bool             running;
    
    int              count;

    QMutex           mutex;

    QWaitCondition   condVar;

    QList<Task*>     todo;
};

ActionThread::ActionThread(QObject *parent)
            : QThread(parent)
{
    d = new ActionThreadPriv;
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void ActionThread::resize(const EmailSettingsContainer& settings)
{
    d->count = 0;
    int i    = 1;

    for (QList<EmailItem>::const_iterator it = settings.itemsList.begin();
         it != settings.itemsList.end(); ++it) 
    {
        QString tmp;

        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->fileUrl                = (*it).url; 
        t->settings               = settings;
        t->destName               = QString("%1.%2").arg(tmp.sprintf("%03i", i)).arg(t->settings.format().toLower());

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
        i++;
    }
}

void ActionThread::cancel()
{
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->running = false;
    d->count   = 0;
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

            emit startingResize(t->fileUrl);

            if (imageResize(t->settings, t->fileUrl, t->destName, errString))
            {
                QString resizedImgPath = t->settings.tempPath + t->destName;
                emit finishedResize(t->fileUrl, resizedImgPath);
            }
            else
            {
                emit failedResize(t->fileUrl, errString);
            }

            d->count++;
            
            if (t->settings.itemsList.count() == d->count)
            {
                emit completeResize();
                d->count = 0;
            }

            delete t;
        }
    }
}

bool ActionThread::imageResize(const EmailSettingsContainer& settings,
                               const KUrl& src, const QString& destName, QString& err)
{
    EmailSettingsContainer emailSettings = settings;
    QFileInfo fi(src.path());

    if (!fi.exists() || !fi.isReadable()) 
    {
        err = i18n("Error in opening input file");
        return false;
    }

    QFileInfo tmp(emailSettings.tempPath);

    if (!tmp.exists() || !tmp.isWritable())
    {
        err = i18n("Error in opening temporary folder");
        return false;
    }

    QImage img;

    // Check if RAW file.
    QString rawFilesExt(raw_file_extentions);
    if (rawFilesExt.toUpper().contains( fi.suffix().toUpper() ))
        KDcrawIface::KDcraw::loadDcrawPreview(img, src.path());
    else
        img.load(src.path());

    int sizeFactor = emailSettings.size();

    if ( !img.isNull() )
    {
        int w = img.width();
        int h = img.height();

        if( w > sizeFactor || h > sizeFactor )
        {
            if( w > h )
            {
                h = (int)( (double)( h * sizeFactor ) / w );
    
                if ( h == 0 ) h = 1;
    
                w = sizeFactor;
                Q_ASSERT( h <= sizeFactor );
            }
            else
            {
                w = (int)( (double)( w * sizeFactor ) / h );
    
                if ( w == 0 ) w = 1;
    
                h = sizeFactor;
                Q_ASSERT( w <= sizeFactor );
            }
    
            const QImage scaledImg(img.scaled(w, h));
    
            if ( scaledImg.width() != w || scaledImg.height() != h )
            {
                err = i18n("Cannot resizing image. Aborting.");
                return false;
            }
    
            img = scaledImg;
        }
    
        QString destPath = emailSettings.tempPath + destName;

        if ( !img.save(destPath, emailSettings.format().toLatin1(), emailSettings.imageCompression) )
        {
            err = i18n("Cannot save resized image. Aborting.");
            return false;
        }
    
        // Only try to write Exif if both src and destination are JPEG files.
    
        if (QString(QImageReader::imageFormat(destPath)).toUpper() == "JPEG" && 
            emailSettings.format().toUpper() == "JPEG")
        {
            KExiv2Iface::KExiv2 meta;
    
            if (meta.load(destPath))
            {
                meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
                meta.setImageDimensions(img.size());
                meta.save(destPath);
            }
            else
            {
                err = i18n("Cannot update metadata to resized image. Aborting.");
                return false;
            }
        }

        return true;
    }

    return false;
}

}  // NameSpace KIPISendimagesPlugin
