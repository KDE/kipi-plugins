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
#include <klocale.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>

// qJson include

#include <qjson/parser.h>

// Local includes

#include "mpform.h"
#include "kpversion.h"
#include "plugin_imgurexport.h"

namespace KIPIImgurExportPlugin
{

typedef QMultiMap<QString, QString> KQOAuthParameters;

class ImgurTalker::Private
{
public:

    Private()
    {
        parent                    = 0;
        interface                 = 0;
        job                       = 0;
        continueUpload            = true;
        userAgent                 = QString("KIPI-Plugins-" + Plugin_ImgurExport::name() + "/" + kipipluginsVersion());
        const char _imgurApiKey[] = _IMGUR_API_ANONYMOUS_KEY;
        anonymousKey              = QByteArray( _imgurApiKey );
    }

    bool       continueUpload;
    QString    userAgent;

    QByteArray anonymousKey;

    QWidget*   parent;
    Interface* interface;
    QByteArray buffer;

    KIO::Job*  job;
};

ImgurTalker::ImgurTalker(Interface* const interface, QWidget* const parent)
    : d(new Private)
{
    d->parent    = parent;
    d->interface = interface;
    d->job       = 0;
    m_queue      = new KUrl::List();
    m_state      = IR_LOGOUT;

    connect(this, SIGNAL(signalUploadDone(KUrl)),
            this, SLOT(slotUploadDone(KUrl)));

    // -------------------------------------------------------------------------

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

void ImgurTalker::parseResponse(const QByteArray& buffer)
{
    bool parseOk = false;
    emit signalUploadDone(m_currentUrl);

    switch(m_state)
    {
        case IE_REMOVEPHOTO:
            parseOk = parseResponseImageRemove(buffer);
            break;
        case IE_ADDPHOTO:
            parseOk = parseResponseImageUpload(buffer);
            break;
        default:
            break;
    }

    if (!parseOk)
    {
        ImgurError error;
        error.message = i18n("Unexpected response from the web service");
        emit signalError(m_currentUrl, error);

        // received something unexpected
        kDebug() << error.message;
    }

    emit signalBusy(false);
}

void ImgurTalker::slotResult(KJob* kjob)
{
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

    if ( job->error() )
    {
        ImgurError err;
        err.message = i18n("Upload failed");
        emit signalError(m_currentUrl, err); //job->errorString()
        kDebug() << "Error :" << job->errorString();
    }

    parseResponse(d->buffer);

    d->buffer.resize(0);

    return;
}

void ImgurTalker::slotUploadDone(const KUrl& currentFile)
{
    // removing the top of the queue
    if (!m_queue->isEmpty())
    {
        m_queue->removeFirst();
        emit signalQueueChanged();
    }

    kDebug () << "Upload done for" << currentFile << "Queue has" << m_queue->length() << "items";
}

void ImgurTalker::slotContinueUpload(bool yes)
{
    d->continueUpload = yes;

    if (yes && !m_queue->isEmpty())
    {
        // the top of the queue was already removed - first() is a new image
        imageUpload(m_queue->first());
    }

    return;
}

bool ImgurTalker::parseResponseImageRemove(const QByteArray& data)
{
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

    QJson::Parser p;
    QVariant      r = p.parse(data, &ok);

    //kDebug() << data;

    if (ok)
    {
        QMap<QString, QVariant> m = r.toMap();
        QString responseType      = m.begin().key();

        if (responseType == "error")
        {
            ImgurError error;
            QMap<QString, QVariant> errData = m.begin().value().toMap();

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

            emit signalError(m_currentUrl, error); // p.errorString()

            kDebug() << "Imgur Error:" << p.errorString();
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

                        if (it.key() == "deletehash")
                        {
                            success.image.deletehash = value;
                        }

                        if (it.key() == "datetime")
                        {
//                            success.image.datetime = QDateTime(value);
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

            emit signalSuccess(m_currentUrl, success);
        }
    }
    else
    {
        ImgurError error;
        error.message = i18n("Parse error");

        emit signalError (m_currentUrl, error);
        kDebug() << "Parse Error:" << p.errorString();
    }

    return ok;
}

void ImgurTalker::imageUpload (const KUrl& filePath)
{
    m_state = IE_ADDPHOTO;

    setCurrentUrl(filePath);

    kDebug() << "Anonymous upload of " << filePath;

    emit signalUploadStart(filePath);
    emit signalBusy(true);

    MPForm form;

    KUrl exportUrl = KUrl(ImgurConnection::APIuploadURL());

    exportUrl.addQueryItem("key",   d->anonymousKey.data());
    exportUrl.addQueryItem("name",  filePath.fileName());
    exportUrl.addQueryItem("title", filePath.fileName()); // this should be replaced with something the user submits
    //exportUrl.addQueryItem("caption", "");              // this should be replaced with something the user submits

    exportUrl.addQueryItem("type", "file");

    form.addFile("image", filePath.path());
    form.finish();

    KIO::TransferJob* const job = KIO::http_post(exportUrl, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("content-length", QString("Content-Length: %1").arg(form.formData().length()));
    job->addMetaData("UserAgent", d->userAgent);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

bool ImgurTalker::imageRemove(const QString& delete_hash)
{
    // @TODO : make sure it works
    MPForm form;

    KUrl removeUrl = KUrl(ImgurConnection::APIdeleteURL());
    removeUrl.addPath(delete_hash + ".json");

    form.finish();

    KIO::TransferJob* const job = KIO::http_post(removeUrl, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("UserAgent", d->userAgent);

    m_state = IE_REMOVEPHOTO;

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
        if (!m_queue->contains(*it))
        {
            m_queue->append(*it);
        }
    }

    emit signalQueueChanged();
}

void ImgurTalker::slotRemoveItems(const KUrl::List &list)
{
    if (list.isEmpty())
    {
        return;
    }

    for( KUrl::List::ConstIterator it = list.begin(); it != list.end(); ++it )
    {
        m_queue->removeAll(*it);
    }

    emit signalQueueChanged();
}

void ImgurTalker::setCurrentUrl(const KUrl& url)
{
    m_currentUrl = url;
}

KUrl::List* ImgurTalker::imageQueue() const
{
    return m_queue;
}

KUrl ImgurTalker::currentUrl() const
{
    return m_currentUrl;
}

} // namespace KIPIImgurExportPlugin
