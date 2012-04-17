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

namespace KIPITimeAdjustPlugin
{

class ActionThread::ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        cancel         = false;
        progress       = 0;

        updAppDate     = false;
        updEXIFModDate = false;
        updEXIFOriDate = false;
        updEXIFDigDate = false;
        updIPTCDate    = false;
        updXMPDate     = false;
        updFileName    = false;
        updFileModDate = false;
        useCustomDate  = false;
    }

    // To manage items processing
    bool             cancel;
    int              progress;

    // Settings from GUI.
    bool             updAppDate;
    bool             updEXIFModDate;
    bool             updEXIFOriDate;
    bool             updEXIFDigDate;
    bool             updIPTCDate;
    bool             updXMPDate;
    bool             updFileName;
    bool             updFileModDate;
    bool             useCustomDate;

    QDateTime        customTime;
    QList<QDateTime> imageOriginalDates;
};

// ----------------------------------------------------------------------------------------------------

Task::Task(QObject* const parent, const KUrl& url, const QDateTime& dt, ActionThread::ActionThreadPriv* const d)
    : Job(parent)
{
    m_url      = url;
    m_dateTime = dt;
    m_d        = d;
}

Task::~Task()
{
}

void Task::run()
{
    if (m_d->cancel || !m_dateTime.isValid()) return;

    bool metadataChanged = m_d->updEXIFModDate || m_d->updEXIFOriDate ||
                           m_d->updEXIFDigDate || m_d->updIPTCDate    ||
                           m_d->updXMPDate;

    if (metadataChanged)
    {
        bool ret = true;

        KPMetadata meta;

        ret &= meta.load(m_url.path());
        if (ret)
        {
            if (meta.canWriteExif(m_url.path()))
            {
                if (m_d->updEXIFModDate)
                {
                    ret &= meta.setExifTagString("Exif.Image.DateTime",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (m_d->updEXIFOriDate)
                {
                    ret &= meta.setExifTagString("Exif.Photo.DateTimeOriginal",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (m_d->updEXIFDigDate)
                {
                    ret &= meta.setExifTagString("Exif.Photo.DateTimeDigitized",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }
            }
            else if (m_d->updEXIFModDate || m_d->updEXIFOriDate || m_d->updEXIFDigDate)
            {
                ret = false;
            }

            if (m_d->updIPTCDate)
            {
                if (meta.canWriteIptc(m_url.path()))
                {
                    ret &= meta.setIptcTagString("Iptc.Application2.DateCreated",
                        m_dateTime.date().toString(Qt::ISODate));
                    ret &= meta.setIptcTagString("Iptc.Application2.TimeCreated",
                        m_dateTime.time().toString(Qt::ISODate));
                }
                else
                {
                    ret = false;
                }
            }

            if (m_d->updXMPDate)
            {
                if (meta.supportXmp() && meta.canWriteXmp(m_url.path()))
                {
                    ret &= meta.setXmpTagString("Xmp.exif.DateTimeOriginal",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.photoshop.DateCreated",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.tiff.DateTime",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.CreateDate",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.MetadataDate",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.ModifyDate",
                        m_dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
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
            emit signalErrorFilesUpdate(QString(), m_url.fileName());
        }
    }

    if (m_d->updFileModDate)
    {
        // Since QFileInfo does not support timestamp updates, see Qt suggestion #79427 at
        // http://www.qtsoftware.com/developer/task-tracker/index_html?id=79427&method=entry
        // we have to use the utime() system call.

        utimbuf times;
        times.actime  = QDateTime::currentDateTime().toTime_t();
        times.modtime = m_dateTime.toTime_t();

        if (utime(m_url.path().toLatin1().constData(), &times) != 0)
        {
            emit signalErrorFilesUpdate(m_url.fileName(), QString());
        }
    }

    if (m_d->updFileName)
    {
        QFileInfo fi(m_url.path());

        QString newFileName = fi.baseName();
        newFileName += '-';
        newFileName += m_dateTime.toString(QString("yyyyMMddThhmmss"));
        newFileName += '.';
        newFileName += fi.completeSuffix();

        KUrl newUrl = m_url;
        newUrl.setFileName(newFileName);

        KDE_rename(QFile::encodeName(m_url.toLocalFile()), QFile::encodeName(newUrl.toLocalFile()));

        KPMetadata::moveSidecar(m_url, newUrl);
    }

    m_mutex.lock();
    m_d->progress++;
    m_mutex.unlock();

    emit signalProgressChanged(m_d->progress);
}

// ----------------------------------------------------------------------------------------------------

ActionThread::ActionThread(QObject* const parent)
    : KPActionThreadBase(parent), d(new ActionThreadPriv)
{
}

ActionThread::~ActionThread()
{
}

void ActionThread::setImages(const KUrl::List& urlList)
{
    JobCollection* collection = new JobCollection();

    for(int i = 0; i < urlList.size(); ++i)
    {
        Task* t = new Task(this, 
                           urlList[i], 
                           d->useCustomDate ? d->customTime 
                                            : d->imageOriginalDates[i],
                           d);

        connect(t, SIGNAL(signalProgressChanged(int)),
                this, SIGNAL(signalProgressChanged(int)));

        connect(t, SIGNAL(signalErrorFilesUpdate(QString, QString)),
                this, SIGNAL(signalErrorFilesUpdate(QString, QString)));

        collection->addJob(t);
     }

    appendJob(collection);
}

void ActionThread::setDateSelection(bool useCustomDate, const QDateTime& customTime, const QList<QDateTime>& imageOriginalDates)
{
    d->useCustomDate      = useCustomDate;
    d->customTime         = customTime;
    d->imageOriginalDates = imageOriginalDates;
}

void ActionThread::setFileNameCheck(bool updFileName)
{
    d->updFileName = updFileName;
}

void ActionThread::setEXIFDataCheck(bool updEXIFModDate, bool updEXIFOriDate, bool updEXIFDigDate)
{
    d->updEXIFModDate = updEXIFModDate;
    d->updEXIFOriDate = updEXIFOriDate;
    d->updEXIFDigDate = updEXIFDigDate;
}

void ActionThread::setIPTCDateCheck(bool updIPTCDate)
{
    d->updIPTCDate = updIPTCDate;
}

void ActionThread::setXMPDateCheck(bool updXMPDate)
{
    d->updXMPDate = updXMPDate;
}

void ActionThread::setFileModDateCheck(bool updFileModDate)
{
    d->updFileModDate = updFileModDate;
}

void ActionThread::setAppDateCheck(bool updAppDate)
{
    d->updAppDate = updAppDate;
}

void ActionThread::cancel()
{
    d->cancel = true;
    KPActionThreadBase::cancel();
}

}  // namespace KIPITimeAdjustPlugin
