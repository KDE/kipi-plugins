/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-09
 * Description : a class to resize image in a separate thread.
 *
 * Copyright (C) 2007-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "imageresize.h"

// Qt includes

#include <QDir>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>
#include <QTemporaryDir>

// KDE includes

#include <klocalizedstring.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw_version.h>
#include <kdcraw.h>

// Local includes

#include "kpversion.h"
#include "kpwriteimage.h"
#include "kpmetadata.h"
#include "kipiplugins_debug.h"

using namespace KIPIPlugins;

namespace KIPISendimagesPlugin
{

Task::Task(int* count)
    : RActionJob()
{
    m_count = count;
}

Task::~Task()
{
}

void Task::run()
{
    QString errString;

    emit startingResize(m_orgUrl);

    m_mutex.lock();
    (*m_count)++;
    m_mutex.unlock();

    int percent = (int)(((float)(*m_count)/(float)m_settings.itemsList.count())*100.0);

    if (imageResize(m_settings, m_orgUrl, m_destName, errString))
    {
        QUrl emailUrl(m_destName);
        emit finishedResize(m_orgUrl, emailUrl, percent);
    }
    else
    {
        emit failedResize(m_orgUrl, errString, percent);
    }

    if (m_settings.itemsList.count() == *m_count)
    {
        m_mutex.lock();
        *m_count = 0;
        m_mutex.unlock();
    }
}

bool Task::imageResize(const EmailSettings& settings, const QUrl& orgUrl,
                       const QString& destName, QString& err)
{
    EmailSettings emailSettings = settings;
    QFileInfo fi(orgUrl.path());

    if (!fi.exists() || !fi.isReadable())
    {
        err = i18n("Error opening input file");
        return false;
    }

    QFileInfo tmp(destName);
    QFileInfo tmpDir(tmp.dir().absolutePath());

    qCDebug(KIPIPLUGINS_LOG) << "tmpDir: " << tmp.dir().absolutePath();

    if (!tmpDir.exists() || !tmpDir.isWritable())
    {
        err = i18n("Error opening temporary folder");
        return false;
    }

    QImage img;

    // Check if RAW file.
    if (KPMetadata::isRawFile(orgUrl))
        KDcraw::loadRawPreview(img, orgUrl.path());
    else
        img.load(orgUrl.path());

    int sizeFactor = emailSettings.size();

    if ( !img.isNull() )
    {
        int w = img.width();
        int h = img.height();

        if ( w > sizeFactor || h > sizeFactor )
        {
            if ( w > h )
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

            const QImage scaledImg(img.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

            if ( scaledImg.width() != w || scaledImg.height() != h )
            {
                err = i18n("Cannot resize image. Aborting.");
                return false;
            }

            img = scaledImg;
        }

        QString destPath = destName;

        KPMetadata meta;
        meta.load(orgUrl.path());
        meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
        meta.setImageDimensions(img.size());

        if (emailSettings.format() == QString("JPEG"))
        {
            if ( !img.save(destPath,
                           emailSettings.format().toLatin1().constData(),
                           emailSettings.imageCompression) )
            {
                err = i18n("Cannot save resized image (JPEG). Aborting.");
                return false;
            }
            else
            {
                meta.save(destPath);
            }
        }
        else if (emailSettings.format() == QString("PNG"))
        {
            QByteArray data((const char*)img.bits(), img.numBytes());
            KPWriteImage wImageIface;
            wImageIface.setImageData(data, img.width(), img.height(), false, true, QByteArray(), meta);

            if ( !wImageIface.write2PNG(destPath) )
            {
                err = i18n("Cannot save resized image (PNG). Aborting.");
                return false;
            }
        }

        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------------------------------

ImageResize::ImageResize(QObject* const parent)
    : RActionThreadBase(parent)
{
    m_count  = new int;
    *m_count = 0;
}

ImageResize::~ImageResize()
{
    delete m_count;
}

void ImageResize::resize(const EmailSettings& settings)
{
    RJobCollection collection;
    *m_count = 0;
    int i    = 1;

    for (QList<EmailItem>::const_iterator it = settings.itemsList.constBegin();
         it != settings.itemsList.constEnd(); ++it)
    {
        QString tmp;

        Task* const t = new Task(m_count);
        t->m_orgUrl   = (*it).orgUrl;
        t->m_settings = settings;

        QTemporaryDir tmpDir(KStandardDirs::locateLocal("tmp", t->m_settings.tempFolderName + t->m_settings.tempPath));
        tmpDir.setAutoRemove(false);
        QFileInfo fi(t->m_orgUrl.fileName());
        t->m_destName = tmpDir.path() + QString("%1.%2").arg(fi.baseName()).arg(t->m_settings.format().toLower());

        connect(t, SIGNAL(startingResize(QUrl)),
                this, SIGNAL(startingResize(QUrl)));

        connect(t, SIGNAL(finishedResize(QUrl,QUrl,int)),
                this, SIGNAL(finishedResize(QUrl,QUrl,int)));

        connect(t, SIGNAL(failedResize(QUrl,QString,int)),
                this, SIGNAL(failedResize(QUrl,QString,int)));

        collection.insert(t, 0);
        i++;
    }

    appendJobs(collection);
}

void ImageResize::cancel()
{
    *m_count   = 0;
    RActionThreadBase::cancel();
}

void ImageResize::slotFinished()
{
    emit completeResize();
}

}  // namespace KIPISendimagesPlugin
