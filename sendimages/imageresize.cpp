/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-09
 * Description : a class to resize image in a separate thread.
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include "imageresize.moc"

// Qt includes

#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>

// KDE includes

#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// Local includes

#include "pluginsversion.h"
#include "kpwriteimage.h"

namespace KIPISendimagesPlugin
{

class ImageResizePriv
{
public:

    ImageResizePriv()
    {
        count   = 0;
        running = false;
    }

    class Task
    {
        public:

            KUrl                   orgUrl;
            QString                destName;
            EmailSettingsContainer settings;
    };

    bool           running;

    int            count;

    QMutex         mutex;

    QWaitCondition condVar;

    QList<Task*>   todo;
};

ImageResize::ImageResize(QObject *parent)
           : QThread(parent), d(new ImageResizePriv)
{
}

ImageResize::~ImageResize()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void ImageResize::resize(const EmailSettingsContainer& settings)
{
    d->count = 0;
    int i    = 1;

    for (QList<EmailItem>::const_iterator it = settings.itemsList.constBegin();
         it != settings.itemsList.constEnd(); ++it)
    {
        QString tmp;

        ImageResizePriv::Task *t = new ImageResizePriv::Task;
        t->orgUrl                = (*it).orgUrl;
        t->settings              = settings;
        t->destName              = QString("%1.%2").arg(tmp.sprintf("%03i", i)).arg(t->settings.format().toLower());

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
        i++;
    }
}

void ImageResize::cancel()
{
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->running = false;
    d->count   = 0;
    d->condVar.wakeAll();
}

void ImageResize::run()
{
    d->running = true;
    while (d->running)
    {
        ImageResizePriv::Task *t = 0;
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

            emit startingResize(t->orgUrl);
            d->count++;
            int percent = (int)(((float)d->count/(float)t->settings.itemsList.count())*100.0);

            if (imageResize(t->settings, t->orgUrl, t->destName, errString))
            {
                KUrl emailUrl(t->settings.tempPath + t->destName);
                emit finishedResize(t->orgUrl, emailUrl, percent);
            }
            else
            {
                emit failedResize(t->orgUrl, errString, percent);
            }

            if (t->settings.itemsList.count() == d->count)
            {
                emit completeResize();
                d->count = 0;
            }

            delete t;
        }
    }
}

bool ImageResize::imageResize(const EmailSettingsContainer& settings,
                              const KUrl& orgUrl, const QString& destName, QString& err)
{
    EmailSettingsContainer emailSettings = settings;
    QFileInfo fi(orgUrl.path());

    if (!fi.exists() || !fi.isReadable())
    {
        err = i18n("Error opening input file");
        return false;
    }

    QFileInfo tmp(emailSettings.tempPath);

    if (!tmp.exists() || !tmp.isWritable())
    {
        err = i18n("Error opening temporary folder");
        return false;
    }

    QImage img;

    // Check if RAW file.
#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    if (rawFilesExt.toUpper().contains( fi.suffix().toUpper() ))
        KDcrawIface::KDcraw::loadDcrawPreview(img, orgUrl.path());
    else
        img.load(orgUrl.path());

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

            const QImage scaledImg(img.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

            if ( scaledImg.width() != w || scaledImg.height() != h )
            {
                err = i18n("Cannot resize image. Aborting.");
                return false;
            }

            img = scaledImg;
        }

        QString destPath = emailSettings.tempPath + destName;

        KExiv2Iface::KExiv2 meta;

        meta.load(orgUrl.path());
        meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
        meta.setImageDimensions(img.size());

        if (emailSettings.format() == QString("JPEG"))
        {
            if ( !img.save(destPath, emailSettings.format().toLatin1(), emailSettings.imageCompression) )
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
            KIPIPlugins::KPWriteImage wImageIface;
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

}  // namespace KIPISendimagesPlugin
