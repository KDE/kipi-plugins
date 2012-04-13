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

#include <kdebug.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <kde_file.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <klocale.h>

// Local includes

#include "kpmetadata.h"

namespace KIPITimeAdjustPlugin
{

class ActionThread::ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        cancel              = false;
        updAppDate          = false;
        updEXIFModDate      = false;
        updEXIFOriDate      = false;
        updEXIFDigDate      = false;
        updIPTCDate         = false;
        updXMPDate          = false;
        updFileName         = false;
        updFileModDate      = false;
        useCustomDateBtn    = false;
        progress            = 0;
    }

    bool             cancel;
    bool             updAppDate;
    bool             updEXIFModDate;
    bool             updEXIFOriDate;
    bool             updEXIFDigDate;
    bool             updIPTCDate;
    bool             updXMPDate;
    bool             updFileName;
    bool             updFileModDate;
    bool             useCustomDateBtn;

    int              progress;

    QDateTime        customTime;
    QList<QDateTime> imageOriginalDates;
    QString          fileTimeErrorFile;
    QString          metaTimeErrorFile;
};

// ----------------------------------------------------------------------------------------------------

Task::Task(QObject* const parent, const KUrl& url, const QDateTime& dateTime,
           ActionThread::ActionThreadPriv* const d)
    : Job(parent)
{
    this->url      = url;
    this->ld       = d;
    this->dateTime = dateTime;
}

void Task::run()
{
    if (ld->cancel)
    {
        return;
    }

    if (!dateTime.isValid()) return;

    bool metadataChanged = ld->updEXIFModDate || ld->updEXIFOriDate ||
                           ld->updEXIFDigDate || ld->updIPTCDate    ||
                           ld->updXMPDate;


    ld->metaTimeErrorFile.clear();
    ld->fileTimeErrorFile.clear();

    if (metadataChanged)
    {
        bool ret = true;

        KPMetadata meta;

        ret &= meta.load(url.path());
        if (ret)
        {
            if (meta.canWriteExif(url.path()))
            {
                if (ld->updEXIFModDate)
                {
                    ret &= meta.setExifTagString("Exif.Image.DateTime",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (ld->updEXIFOriDate)
                {
                    ret &= meta.setExifTagString("Exif.Photo.DateTimeOriginal",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (ld->updEXIFDigDate)
                {
                    ret &= meta.setExifTagString("Exif.Photo.DateTimeDigitized",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }
            }
            else if (ld->updEXIFModDate || ld->updEXIFOriDate ||
                     ld->updEXIFDigDate)
            {
                ret = false;
            }

            if (ld->updIPTCDate)
            {
                if (meta.canWriteIptc(url.path()))
                {
                    ret &= meta.setIptcTagString("Iptc.Application2.DateCreated",
                        dateTime.date().toString(Qt::ISODate));
                    ret &= meta.setIptcTagString("Iptc.Application2.TimeCreated",
                        dateTime.time().toString(Qt::ISODate));
                }
                else ret = false;
            }

            if (ld->updXMPDate)
            {
                if (meta.supportXmp() && meta.canWriteXmp(url.path()))
                {
                    ret &= meta.setXmpTagString("Xmp.exif.DateTimeOriginal",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.photoshop.DateCreated",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.tiff.DateTime",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.CreateDate",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.MetadataDate",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= meta.setXmpTagString("Xmp.xmp.ModifyDate",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }
                else ret = false;
            }

            ret &= meta.save(url.path());

            if (!ret)
            {
                kDebug() << "Failed to update metadata in file " << url.fileName();
            }
        }
        else
        {
            kDebug() << "Failed to load metadata from file " << url.fileName();
        }

        if (!ret)
        {
            ld->metaTimeErrorFile = url.fileName();
            emit signalErrorFilesUpdate(QString(), ld->metaTimeErrorFile);
        }
    }

    if (ld->updFileModDate)
    {
        // since QFileInfo does not support timestamp updates, see Qt suggestion #79427 at
        // http://www.qtsoftware.com/developer/task-tracker/index_html?id=79427&method=entry
        // we have to use the utime() system call

        utimbuf times;
        times.actime  = QDateTime::currentDateTime().toTime_t();
        times.modtime = dateTime.toTime_t();

        if (0 != utime(url.path().toLatin1().constData(), &times))
        {
            ld->fileTimeErrorFile = url.fileName();
            emit signalErrorFilesUpdate(ld->fileTimeErrorFile, QString());
        }
    }

    if (ld->updFileName)
    {
        QFileInfo image(url.path());
        QString newdate = dateTime.toString(QString("yyyyMMddThhmmss"));

        newdate += '.';
        newdate += image.suffix();

        KUrl newUrl = url;
        newUrl.setFileName(newdate);

        KDE_rename(QFile::encodeName(url.toLocalFile()), QFile::encodeName(newUrl.toLocalFile()));

        KPMetadata::moveSidecar(url, newUrl);
    }

    ld->progress++;

    emit signalProgressChanged(ld->progress);
}

// ----------------------------------------------------------------------------------------------------

ActionThread::ActionThread(QObject* const parent)
    : KPActionThreadBase(parent), pd(new ActionThreadPriv)
{
}

ActionThread::~ActionThread()
{
}

void ActionThread::setImages(const KUrl::List& urlList)
{
    JobCollection* collection = new JobCollection();
    QMap<KUrl, QDateTime> mapping;

    for(int i=0; i< urlList.size(); ++i)
    {
        if (pd->useCustomDateBtn)
            mapping[urlList[i]] = pd->customTime;
        else
            mapping[urlList[i]] = pd->imageOriginalDates[i];
    }

    foreach(const KUrl& url, urlList)
    {

        Task* t = 0;

        t = new Task(this, url, mapping[url], pd);

        connect(t, SIGNAL(signalProgressChanged(int)),
                this, SIGNAL(signalProgressChanged(int)));

        connect(t, SIGNAL(signalErrorFilesUpdate(QString, QString)),
                this, SIGNAL(signalErrorFilesUpdate(QString, QString)));

        collection->addJob(t);
     }

    appendJob(collection);
}

void ActionThread::setDateSelection(bool useCustomDateBtn, const QDateTime& customTime, const QList<QDateTime>& imageOriginalDates)
{
    pd->useCustomDateBtn   = useCustomDateBtn;
    pd->customTime         = customTime;
    pd->imageOriginalDates = imageOriginalDates;
}

void ActionThread::setFileNameCheck(bool updFileName)
{
    pd->updFileName = updFileName;
}

void ActionThread::setEXIFDataCheck(bool updEXIFModDate, bool updEXIFOriDate, bool updEXIFDigDate)
{
    pd->updEXIFModDate = updEXIFModDate;
    pd->updEXIFOriDate = updEXIFOriDate;
    pd->updEXIFDigDate = updEXIFDigDate;
}

void ActionThread::setIPTCDateCheck(bool updIPTCDate)
{
    pd->updIPTCDate = updIPTCDate;
}

void ActionThread::setXMPDateCheck(bool updXMPDate)
{
    pd->updXMPDate = updXMPDate;
}

void ActionThread::setFileModDateCheck(bool updFileModDate)
{
    pd->updFileModDate = updFileModDate;
}

void ActionThread::setAppDateCheck(bool updAppDate)
{
    pd->updAppDate = updAppDate;
}

void ActionThread::cancel()
{
    pd->cancel = true;
    KPActionThreadBase::cancel();
}

}  // namespace KIPITimeAdjustPlugin
