/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-12-31
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "task.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
#include <utime.h>
}

// Qt includes

#include <QFileInfo>

// KDE includes

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <kdebug.h>
#include <kde_file.h>

// Local includes

#include "kpmetadata.h"
#include "kpimageinfo.h"
#include "myimagelist.h"
#include "actionthread.h"

namespace KIPITimeAdjustPlugin
{
    
class Task::Private
{
public:

    Private()
    {
        cancel    = false;
    }

    bool                  cancel;
    KUrl                  url;
    
    // Settings from GUI.
    TimeAdjustSettings    settings;

    // Map of item urls and Updated Timestamps.
    QMap<KUrl, QDateTime> itemsMap;
};

Task::Task(QObject* const parent, const KUrl& url)
    : Job(parent), d(new Private)
{
    d->url = url;
}

Task::~Task()
{
    slotCancel();
    delete d;
}

void Task::slotCancel()
{
    d->cancel = true;
}

void Task::setSettings(const TimeAdjustSettings& settings)
{
    d->settings = settings;
}

void Task::setItemsMap(QMap<KUrl, QDateTime> itemsMap)
{
    d->itemsMap = itemsMap;
}

void Task::run()
{
    if (d->cancel) return;

    QDateTime dt = d->itemsMap.value(d->url);

    if (!dt.isValid()) return;

    emit signalProcessStarted(d->url);

    bool metadataChanged = d->settings.updEXIFModDate || d->settings.updEXIFOriDate ||
                           d->settings.updEXIFDigDate || d->settings.updEXIFThmDate ||
                           d->settings.updIPTCDate    || d->settings.updXMPDate;

    int status = MyImageList::NOPROCESS_ERROR;

    if (metadataChanged)
    {
        bool ret = true;

        KPMetadata meta;

        ret &= meta.load(d->url.path());
        if (ret)
        {
            if (meta.canWriteExif(d->url.path()))
            {
                if (d->settings.updEXIFModDate)
                {
                    ret &= meta.setExifTagString("Exif.Image.DateTime",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (d->settings.updEXIFOriDate)
                {
                    ret &= meta.setExifTagString("Exif.Photo.DateTimeOriginal",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (d->settings.updEXIFDigDate)
                {
                    ret &= meta.setExifTagString("Exif.Photo.DateTimeDigitized",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }
                
                if (d->settings.updEXIFThmDate)
                {
                    ret &= meta.setExifTagString("Exif.Image.PreviewDateTime",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }
            }
            else if (d->settings.updEXIFModDate || d->settings.updEXIFOriDate || 
                     d->settings.updEXIFDigDate || d->settings.updEXIFThmDate)
            {
                ret = false;
            }

            if (d->settings.updIPTCDate)
            {
                if (meta.canWriteIptc(d->url.path()))
                {
                    ret &= meta.setIptcTagString("Iptc.Application2.DateCreated",
                        dt.date().toString(Qt::ISODate));
                    ret &= meta.setIptcTagString("Iptc.Application2.TimeCreated",
                        dt.time().toString(Qt::ISODate));
                }
                else
                {
                    ret = false;
                }
            }

            if (d->settings.updXMPDate)
            {
                if (meta.supportXmp() && meta.canWriteXmp(d->url.path()))
                {
                    ret &= meta.setXmpTagString("Xmp.exif.DateTimeOriginal",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.photoshop.DateCreated",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.tiff.DateTime",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.CreateDate",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.MetadataDate",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.ModifyDate",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }
                else
                {
                    ret = false;
                }
            }

            ret &= meta.save(d->url.path());

            if (!ret)
            {
                kDebug() << "Failed to update metadata in file " << d->url.fileName();
            }
        }
        else
        {
            kDebug() << "Failed to load metadata from file " << d->url.fileName();
        }

        if (!ret)
        {
            status |= MyImageList::META_TIME_ERROR;
        }
    }

    if (d->settings.updFileModDate)
    {
        // Since QFileInfo does not support timestamp updates, see Qt suggestion #79427 at
        // http://www.qtsoftware.com/developer/task-tracker/index_html?id=79427&method=entry
        // we have to use the utime() system call.

        utimbuf times;
        times.actime  = QDateTime::currentDateTime().toTime_t();
        times.modtime = dt.toTime_t();

        if (utime(QFile::encodeName(d->url.toLocalFile()).constData(), &times) != 0)
        {
            status |= MyImageList::FILE_TIME_ERROR;
        }
    }

    if (d->settings.updFileName)
    {
        bool ret    = true;
        KUrl newUrl = ActionThread::newUrl(d->url, dt);

        if (KDE_rename(QFile::encodeName(d->url.toLocalFile()), QFile::encodeName(newUrl.toLocalFile())) != 0)
            ret = false;

        ret &= KPMetadata::moveSidecar(d->url, newUrl);

        if (!ret)
            status |= MyImageList::FILE_NAME_ERROR;
    }
    
    if (d->settings.updAppDate)
    {
        KPImageInfo info(d->url);
        QDateTime dt = d->itemsMap.value(d->url);

        if (dt.isValid()) info.setDate(dt);
    }

    emit signalProcessEnded(d->url, status);
}
    
} // namespace KIPITimeAdjustPlugin
