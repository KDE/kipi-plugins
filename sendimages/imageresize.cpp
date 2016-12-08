/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-09
 * Description : a class to resize image in a separate thread.
 *
 * Copyright (C) 2007-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QPointer>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>
#include <QTemporaryDir>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/PluginLoader>

// Local includes

#include "kpversion.h"
#include "kipiplugins_debug.h"

using namespace KIPIPlugins;

namespace KIPISendimagesPlugin
{

Task::Task(int* count)
    : KPJob()
{
    m_count = count;
    m_iface = 0;

    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
    }
}

Task::~Task()
{
}

void Task::run()
{
    emit signalStarted();

    QString errString;

    emit startingResize(m_orgUrl);

    m_mutex.lock();
    (*m_count)++;
    m_mutex.unlock();

    int percent = (int)(((float)(*m_count)/(float)m_settings.itemsList.count())*100.0);

    if (imageResize(m_settings, m_orgUrl, m_destName, errString))
    {
        QUrl emailUrl(QUrl::fromLocalFile(m_destName));
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

    emit signalDone();
}

bool Task::imageResize(const EmailSettings& settings, const QUrl& orgUrl,
                       const QString& destName, QString& err)
{
    EmailSettings emailSettings = settings;
    QFileInfo fi(orgUrl.toLocalFile());

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

    if (m_iface)
    {
        img = m_iface->preview(orgUrl);
    }

    if (img.isNull())
    {
        img.load(orgUrl.toLocalFile());
    }

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

        if (m_iface)
        {
            QPointer<MetadataProcessor> meta = m_iface->createMetadataProcessor();

            if (meta && meta->load(orgUrl))
            {
                meta->setImageProgramId(QLatin1String("Kipi-plugins"), QLatin1String(kipiplugins_version));
                meta->setImageDimensions(img.size());

                if (emailSettings.format() == QLatin1String("JPEG"))
                {
                    if ( !img.save(destPath, emailSettings.format().toLatin1().constData(), emailSettings.imageCompression) )
                    {
                        err = i18n("Cannot save resized image (JPEG). Aborting.");
                        return false;
                    }
                    else
                    {
                        meta->save(QUrl::fromLocalFile(destPath), true);
                    }
                }
                else if (emailSettings.format() == QLatin1String("PNG"))
                {
                    if ( !img.save(destPath, emailSettings.format().toLatin1().constData()) )
                    {
                        err = i18n("Cannot save resized image (PNG). Aborting.");
                        return false;
                    }
                    else
                    {
                        meta->save(QUrl::fromLocalFile(destPath), true);
                    }
                }

                return true;
            }
        }
    }

    return false;
}

// ----------------------------------------------------------------------------------------------------

ImageResize::ImageResize(QObject* const parent)
    : KPThreadManager(parent)
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
    KPJobCollection collection;
    *m_count = 0;
    int i    = 1;

    for (QList<EmailItem>::const_iterator it = settings.itemsList.constBegin();
         it != settings.itemsList.constEnd(); ++it)
    {
        Task* const t = new Task(m_count);
        t->m_orgUrl   = (*it).orgUrl;
        t->m_settings = settings;

        QTemporaryDir tmpDir(t->m_settings.tempPath);
        tmpDir.setAutoRemove(false);

        QFileInfo fi(t->m_orgUrl.fileName());
        t->m_destName = tmpDir.path() + QLatin1Char('/') +
                        QString::fromUtf8("%1.%2").arg(fi.baseName()).arg(t->m_settings.format().toLower());

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
    KPThreadManager::cancel();
}

}  // namespace KIPISendimagesPlugin
