/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "actionthread.moc"

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
#include "myimagelist.h"

namespace KIPITimeAdjustPlugin
{

class ActionThread::ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        cancel   = false;
        progress = 0;
    }

    // To manage items processing.
    bool                  cancel;
    int                   progress;

    // Settings from GUI.
    TimeAdjustSettings    settings;

    // Map of item urls and Updated Timestamps.
    QMap<KUrl, QDateTime> itemsMap;
};

// ----------------------------------------------------------------------------------------------------

Task::Task(QObject* const parent, const KUrl& url, ActionThread::ActionThreadPriv* const d)
    : Job(parent)
{
    m_url      = url;
    m_d        = d;
}

Task::~Task()
{
}

void Task::run()
{
    if (m_d->cancel) return;

    QDateTime dt = m_d->itemsMap.value(m_url);

    if (!dt.isValid()) return;

    emit signalProcessStarted(m_url);

    bool metadataChanged = m_d->settings.updEXIFModDate || m_d->settings.updEXIFOriDate ||
                           m_d->settings.updEXIFDigDate || m_d->settings.updIPTCDate    ||
                           m_d->settings.updXMPDate;

    int status = MyImageList::NO_ERROR;

    if (metadataChanged)
    {
        bool ret = true;

        KPMetadata meta;

        ret &= meta.load(m_url.path());
        if (ret)
        {
            if (meta.canWriteExif(m_url.path()))
            {
                if (m_d->settings.updEXIFModDate)
                {
                    ret &= meta.setExifTagString("Exif.Image.DateTime",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (m_d->settings.updEXIFOriDate)
                {
                    ret &= meta.setExifTagString("Exif.Photo.DateTimeOriginal",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (m_d->settings.updEXIFDigDate)
                {
                    ret &= meta.setExifTagString("Exif.Photo.DateTimeDigitized",
                        dt.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }
            }
            else if (m_d->settings.updEXIFModDate || m_d->settings.updEXIFOriDate || m_d->settings.updEXIFDigDate)
            {
                ret = false;
            }

            if (m_d->settings.updIPTCDate)
            {
                if (meta.canWriteIptc(m_url.path()))
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

            if (m_d->settings.updXMPDate)
            {
                if (meta.supportXmp() && meta.canWriteXmp(m_url.path()))
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

            ret &= meta.save(m_url.path());

            if (!ret)
            {
                kDebug() << "Failed to update metadata in file " << m_url.fileName();
            }
        }
        else
        {
            kDebug() << "Failed to load metadata from file " << m_url.fileName();
        }

        if (!ret)
        {
            status |= MyImageList::META_TIME_ERROR;
        }
    }

    if (m_d->settings.updFileModDate)
    {
        // Since QFileInfo does not support timestamp updates, see Qt suggestion #79427 at
        // http://www.qtsoftware.com/developer/task-tracker/index_html?id=79427&method=entry
        // we have to use the utime() system call.

        utimbuf times;
        times.actime  = QDateTime::currentDateTime().toTime_t();
        times.modtime = dt.toTime_t();

        if (utime(m_url.path().toLatin1().constData(), &times) != 0)
        {
            status |= MyImageList::FILE_TIME_ERROR;
        }
    }

    if (m_d->settings.updFileName)
    {
        bool ret    = true;
        KUrl newUrl = ActionThread::newUrl(m_url, dt);

        if (KDE_rename(QFile::encodeName(m_url.toLocalFile()), QFile::encodeName(newUrl.toLocalFile())) != 0)
            ret = false;

        ret &= KPMetadata::moveSidecar(m_url, newUrl);

        if (!ret)
            status |= MyImageList::FILE_NAME_ERROR;
    }

    m_mutex.lock();
    m_d->progress++;
    emit signalProgressChanged(m_d->progress);
    m_mutex.unlock();

    emit signalProcessEnded(m_url, status);
}

// ----------------------------------------------------------------------------------------------------

ActionThread::ActionThread(QObject* const parent)
    : KPActionThreadBase(parent), d(new ActionThreadPriv)
{
}

ActionThread::~ActionThread()
{
}

void ActionThread::setUpdatedDates(const QMap<KUrl, QDateTime>& map)
{
    d->itemsMap               = map;
    JobCollection* collection = new JobCollection();

    foreach (const KUrl& url, d->itemsMap.keys())
    {
        Task* t = new Task(this, url, d);

        connect(t, SIGNAL(signalProgressChanged(int)),
                this, SIGNAL(signalProgressChanged(int)));

        connect(t, SIGNAL(signalProcessStarted(KUrl)),
                this, SIGNAL(signalProcessStarted(KUrl)));

        connect(t, SIGNAL(signalProcessEnded(KUrl, int)),
                this, SIGNAL(signalProcessEnded(KUrl, int)));

        collection->addJob(t);
     }

    appendJob(collection);
}

void ActionThread::setSettings(const TimeAdjustSettings& settings)
{
    d->settings = settings;
}

void ActionThread::cancel()
{
    d->cancel = true;
    KPActionThreadBase::cancel();
}

/** Static public method also called from GUI to update listview information about new filename
 *  computed with timeStamp.
 */
KUrl ActionThread::newUrl(const KUrl& url, const QDateTime& dt)
{
    if (!dt.isValid()) return KUrl();

    QFileInfo fi(url.path());

    QString newFileName = fi.baseName();
    newFileName += '-';
    newFileName += dt.toString(QString("yyyyMMddThhmmss"));
    newFileName += '.';
    newFileName += fi.completeSuffix();

    KUrl newUrl = url;
    newUrl.setFileName(newFileName);
    return newUrl;
}

}  // namespace KIPITimeAdjustPlugin
