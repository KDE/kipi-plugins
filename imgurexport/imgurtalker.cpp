/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#include "imgurtalker.moc"

// Qt includes

#include <QVariant>

// KDE includes

#include <kdebug.h>
#include <kio/job.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>

// qJson include

#include <qjson/parser.h>

// Local includes

#include "mpform.h"
#include "kpversion.h"

namespace KIPIImgurExportPlugin
{

class ImgurTalker::ImgurTalkerPriv
{
public:

    ImgurTalkerPriv()
    {
        parent    = 0;
        interface = 0;
        job       = 0;
        queue     = 0;
    }

    QString     apiKey;
    QString     userAgent;

    QWidget*    parent;
    Interface*  interface;
    QByteArray  buffer;

    State       state;
    KUrl        currentUrl;
    KIO::Job*   job;

    KUrl::List* queue;
};

ImgurTalker::ImgurTalker(Interface* const interface, QWidget* const parent)
    : d(new ImgurTalkerPriv)
{
    d->parent              = parent;
    d->interface           = interface;

    d->job                 = 0;
    d->userAgent           = QString("KIPI-Plugins-ImgurTalker/" + kipipluginsVersion());

    d->apiKey              = _IMGUR_API_KEY;

    d->queue               = new KUrl::List();

    connect(this, SIGNAL(signalUploadDone()),
            this, SLOT(slotUploadDone()));

    ImageCollection images = interface->currentSelection();
    if (images.isValid())
    {
        slotAddItems(images.images());
    }
}

ImgurTalker::~ImgurTalker()
{
    if (d->job)
    {
        d->job->kill();
    }
    delete d;
}

void ImgurTalker::slotData(KIO::Job* j, const QByteArray& data)
{
    if (data.isEmpty())
    {
        return;
    }

    int oldSize = d->buffer.size();
    int newSize = data.size();

    d->buffer.resize(d->buffer.size() + newSize);
    memcpy(d->buffer.data()+oldSize, data.data(), newSize);

    emit signalUploadProgress(j->percent());
}

void ImgurTalker::slotResult(KJob* kjob)
{
    KIO::Job* job = static_cast<KIO::Job*>(kjob);

    if ( job->error() )
    {
        ImgurError err;
        err.message = tr("Upload failed");
        emit signalError(err); //job->errorString()
        kDebug() << "Error :" << job->errorString();
    }

    bool parseOk;

    switch(d->state)
    {
        case IE_REMOVEPHOTO:
            parseOk = parseResponseImageRemove(d->buffer);
            break;

        case IE_ADDPHOTO:
            parseOk = parseResponseImageUpload(d->buffer);
            break;
        default:
            break;
    }

    d->buffer.resize(0);

    emit signalBusy(false);

    return;
}

void ImgurTalker::slotUploadDone()
{
    if (d->queue->isEmpty())
    {
        return;
    }

    kDebug () << "Upload done for" << d->currentUrl << "Queue has" << d->queue->length() << "items";
    d->queue->removeFirst();

    emit signalQueueChanged();
}

bool ImgurTalker::parseResponseImageRemove(const QByteArray& data)
{
    emit signalUploadDone();
    if (data.isEmpty())
    {
        // TODO
    }
    return false;
}

bool ImgurTalker::parseResponseImageUpload(const QByteArray& data)
{
    bool ok = false;

    if (data.isEmpty())
        return false;

    QJson::Parser* p = new QJson::Parser();
    QVariant       r = p->parse(data, &ok);

    emit signalUploadDone();
    if (ok)
    {
        QMap<QString, QVariant> m = r.toMap();
        QString responseType      = m.begin().key();

        if (responseType == "error")
        {
            ImgurError error;
            QMap<QString,QVariant> errData = m.begin().value().toMap();

            for (QMap<QString,QVariant>::iterator it = errData.begin(); it != errData.end(); ++it)
            {
                QString v = it.value().toString();

                if (it.key() == "message")
                {
                    error.message = v;
                }
                if (it.key() == "request")
                {
                    error.request = v;
                }

                if (it.key() == "method")
                {
                    if ( v == "get")
                    {
                        error.method = ImgurError::GET;
                    }
                    if ( v == "post")
                    {
                        error.method = ImgurError::POST;
                    }
                }

                if (it.key() == "format")
                {
                    if ( v == "json")
                    {
                        error.format = ImgurError::JSON;
                    }
                    if ( v == "xml")
                    {
                        error.format = ImgurError::XML;
                    }
                }

                if (it.key() == "parameters")
                {
                    error.parameters =  v;
                }
            }

            emit signalError(error); // p->errorString()
            return false;
        }

        if (responseType == "upload" )
        {
            ImgurSuccess success;
            QMap<QString, QVariant> successData = m.begin().value().toMap();

            for (QMap<QString,QVariant>::iterator it = successData.begin(); it != successData.end(); ++it)
            {
                if (it.key() == "image")
                {
                    QMap<QString, QVariant> v = it.value().toMap();

                    for (QMap<QString,QVariant>::iterator it = v.begin(); it != v.end(); ++it)
                    {
                        QString value = it.value().toString();
                        if (it.key() == "name")
                        {
                            success.image.name = value;
                        }
                        if (it.key() == "title")
                        {
                            success.image.title = value;
                        }
                        if (it.key() == "caption")
                        {
                            success.image.caption = value;
                        }
                        if (it.key() == "hash")
                        {
                            success.image.hash = value;
                        }
                        if (it.key() == "deleteHash")
                        {
                            success.image.deletehash = value;
                        }
                        if (it.key() == "dateTime")
                        {
                            //success.image.datetime = QDateTime(value);
                        }
                        if (it.key() == "type")
                        {
                            success.image.type = value;
                        }
                        if (it.key() == "animated")
                        {
                            success.image.animated = (value == "true");
                        }
                        if (it.key() == "width")
                        {
                            success.image.width = value.toInt();
                        }
                        if (it.key() == "height")
                        {
                            success.image.height = value.toInt();
                        }
                        if (it.key() == "size")
                        {
                            success.image.size = value.toInt();
                        }
                        if (it.key() == "views")
                        {
                            success.image.views = value.toInt();
                        }
                        if (it.key() == "bandwidth")
                        {
                            success.image.bandwidth = value.toLongLong();
                        }
                    }
                }
                if (it.key() == "links")
                {
                    QMap<QString, QVariant> v = it.value().toMap();

                    for (QMap<QString,QVariant>::iterator it = v.begin(); it != v.end(); ++it)
                    {
                        QString value = it.value().toString();

                        if (it.key() == "original")
                        {
                            success.links.original = value;
                        }
                        if (it.key() == "imgur_page")
                        {
                            success.links.imgur_page = value;
                        }
                        if (it.key() == "delete_page")
                        {
                            success.links.delete_page = value;
                        }
                        if (it.key() == "small_square")
                        {
                            success.links.small_square = value;
                        }
                        if (it.key() == "largeThumbnail")
                        {
                            success.links.large_thumbnail = value;
                        }
                    }
                }
            }

            emit signalSuccess(success);
        }
    }
    else
    {
        ImgurError error;
        error.message = "Parse error";

        emit signalError (error); // p->errorString()
        kDebug() << "Parser error :" << p->errorString();
        return false;
    }

    return true;
}

bool ImgurTalker::imageUpload(const KUrl& filePath)
{
    d->currentUrl  = filePath;

    MPForm form;

    KUrl exportUrl = KUrl("http://api.imgur.com/2/upload.json");
    exportUrl.addQueryItem("key", d->apiKey);

    exportUrl.addQueryItem("name", filePath.fileName());
    exportUrl.addQueryItem("title", filePath.fileName()); // this should be replaced with something the user submits
//    exportUrl.addQueryItem("caption", ""); // this should be replaced with something the user submits

    exportUrl.addQueryItem("type", "file");

    form.addFile("image", filePath.path());
    form.finish();

    KIO::TransferJob* job = KIO::http_post(exportUrl, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("content-length", QString("Content-Length: %1").arg(form.formData().length()));
    job->addMetaData("UserAgent", d->userAgent);

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(slotData(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    d->state = IE_ADDPHOTO;

    emit signalUploadStart(filePath);
    emit signalBusy(true);

    return true;
}

bool ImgurTalker::imageRemove(const QString& delete_hash)
{
    // @TODO : make sure it works
    MPForm form;

    KUrl removeUrl = KUrl("http://api.imgur.com/2/delete/");
    removeUrl.addPath(delete_hash + ".json");

    form.finish();

    KIO::TransferJob* job = KIO::http_post(removeUrl, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("UserAgent", d->userAgent);

    d->state = IE_REMOVEPHOTO;

    emit signalBusy(true);
    emit signalQueueChanged();

    return true;
}

void ImgurTalker::cancel()
{
    if (d->job)
    {
        d->job->kill();
        d->job = 0;
    }

    emit signalBusy(false);
}

void ImgurTalker::slotAddItems(const KUrl::List& list)
{
    if (list.isEmpty())
    {
        return;
    }

    for( KUrl::List::ConstIterator it = list.begin(); it != list.end(); ++it )
    {
        if (!d->queue->contains(*it))
        {
            d->queue->append(*it);
        }
    }

    emit signalQueueChanged();
}

void ImgurTalker::slotRemoveItems(const KUrl::List list)
{
    if (list.isEmpty())
    {
        return;
    }

    for( KUrl::List::ConstIterator it = list.begin(); it != list.end(); ++it )
    {
        d->queue->removeAll(*it);
    }

    emit signalQueueChanged();
}

KUrl::List* ImgurTalker::imageQueue() const
{
    return d->queue;
}

KUrl ImgurTalker::geCurrentUrl() const
{
    return d->currentUrl;
}

} // namespace KIPIImgurExportPlugin
