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

#include "imgurtalker.h"

// Qt includes

#include <QVariant>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_debug.h"
#include "mpform.h"
#include "kpversion.h"
#include "plugin_imgur.h"

namespace KIPIImgurPlugin
{
typedef QMultiMap<QString, QString> KQOAuthParameters;

class ImgurTalker::Private
{
public:

    Private()
    {
        parent                      = 0;
        interface                   = 0;
        netMngr                     = 0;
        reply                       = 0;
        continueUpload              = true;
        userAgent                   = QLatin1String("KIPI-Plugins-ImgurExport") + QLatin1String("/") + kipipluginsVersion();
        const char _imgurClientId[] = _IMGUR_API_ANONYMOUS_CLIENT_ID;
        anonymousClientId           = QByteArray(_imgurClientId);
    }

    bool                   continueUpload;
    QString                userAgent;

    QByteArray             anonymousClientId;

    QWidget*               parent;
    Interface*             interface;
    QNetworkAccessManager* netMngr;
    QNetworkReply*         reply;
};

ImgurTalker::ImgurTalker(Interface* const interface, QWidget* const parent)
    : d(new Private)
{
    d->parent    = parent;
    d->interface = interface;
    d->reply     = 0;
    m_queue      = new QList<QUrl>();
    m_state      = IR_LOGOUT;

    d->netMngr   = new QNetworkAccessManager(this);

    connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotFinished(QNetworkReply*)));

    connect(this, SIGNAL(signalUploadDone(QUrl)),
            this, SLOT(slotUploadDone(QUrl)));

    // -------------------------------------------------------------------------

    ImageCollection images = interface->currentSelection();

    if (images.isValid())
    {
        slotAddItems(images.images());
    }
}

ImgurTalker::~ImgurTalker()
{
    if (d->reply)
    {
        d->reply->abort();
    }

    delete d;
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
        qCDebug(KIPIPLUGINS_LOG) << error.message;
    }

    emit signalBusy(false);
}

void ImgurTalker::slotFinished(QNetworkReply* reply)
{
    if (reply != d->reply)
    {
        return;
    }

    d->reply = 0;

    if (reply->error() != QNetworkReply::NoError)
    {
        ImgurError err;
        err.message = i18n("Upload failed");
        emit signalError(m_currentUrl, err); //reply->errorString()
        qCDebug(KIPIPLUGINS_LOG) << "Error :" << reply->errorString();
    }

    parseResponse(reply->readAll());

    reply->deleteLater();
    return;
}

void ImgurTalker::slotUploadDone(const QUrl& currentFile)
{
    // removing the top of the queue
    if (!m_queue->isEmpty())
    {
        m_queue->removeFirst();
        emit signalQueueChanged();
    }

    qCDebug(KIPIPLUGINS_LOG) << "Upload done for" << currentFile << "Queue has" << m_queue->length() << "items";
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
    qCDebug(KIPIPLUGINS_LOG) << "Upload Image data is " << data;

    if (data.isEmpty())
        return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    bool ok           = (err.error == QJsonParseError::NoError);

    if (ok)
    {
        QJsonObject jsonObject = doc.object();

        if (jsonObject.contains(QString::fromLatin1("data")))
        {
            ImgurSuccess success;
            QJsonObject obj1          = jsonObject[QString::fromLatin1("data")].toObject();

            success.image.id          = obj1[QString::fromLatin1("id")].toString();

            success.image.type        = obj1[QString::fromLatin1("type")].toString();

            success.image.name        = obj1[QString::fromLatin1("name")].toString();
            qCDebug(KIPIPLUGINS_LOG) << "Name is " << success.image.name;

            success.image.title       = obj1[QString::fromLatin1("title")].toString();

            success.image.description = obj1[QString::fromLatin1("description")].toString();

            success.image.deletehash  = obj1[QString::fromLatin1("deletehash")].toString();

            success.image.animated    = (obj1[QString::fromLatin1("animated")].toString() == QString::fromLatin1("true"));

            success.image.width       = obj1[QString::fromLatin1("width")].toString().toInt();

            success.image.height      = obj1[QString::fromLatin1("height")].toString().toInt();

            success.image.size        = obj1[QString::fromLatin1("size")].toString().toInt();

            success.image.views       = obj1[QString::fromLatin1("views")].toString().toInt();

            success.image.bandwidth   = obj1[QString::fromLatin1("bandwidth")].toString().toLongLong();

            success.image.link        = QUrl(obj1[QString::fromLatin1("link")].toString());
            qCDebug(KIPIPLUGINS_LOG) << "Link original is " << success.image.link;

            emit signalSuccess(m_currentUrl, success);
        }

        if (jsonObject.contains(QString::fromLatin1("error")))
        {
            ImgurError error;
            QJsonObject obj  = jsonObject[QString::fromLatin1("error")].toObject();

            error.message    = obj[QString::fromLatin1("message")].toString();

            error.request    = obj[QString::fromLatin1("request")].toString();

            error.parameters = obj[QString::fromLatin1("parameters")].toString();

            if (QString::compare(obj[QString::fromLatin1("method")].toString(),
                                 QString::fromLatin1("get"), Qt::CaseInsensitive) == 0)
            {
                error.method = ImgurError::GET;
            }
            else if (QString::compare(obj[QString::fromLatin1("method")].toString(),
                                QString::fromLatin1("post"), Qt::CaseInsensitive) == 0)
            {
                error.method = ImgurError::POST;
            }
            else if (QString::compare(obj[QString::fromLatin1("format")].toString(),
                                 QString::fromLatin1("json"), Qt::CaseInsensitive) == 0)
            {
                error.format = ImgurError::JSON;
            }
            else if (QString::compare(obj[QString::fromLatin1("format")].toString(),
                                 QString::fromLatin1("xml"), Qt::CaseInsensitive) == 0)
            {
                error.format = ImgurError::XML;
            }

            emit signalError(m_currentUrl, error);
            qCDebug(KIPIPLUGINS_LOG) << "Imgur Error:" << error.message;
        }
    }
    else
    {
        ImgurError error;
        error.message = i18n("Parse error");
        emit signalError (m_currentUrl, error);
        qCDebug(KIPIPLUGINS_LOG) << "Parse Error";
    }

    return ok;
}

void ImgurTalker::imageUpload (const QUrl& filePath)
{
    m_state = IE_ADDPHOTO;

    setCurrentUrl(filePath);

    qCDebug(KIPIPLUGINS_LOG) << "Anonymous upload of " << filePath;

    emit signalUploadStart(filePath);
    emit signalBusy(true);

    MPForm form;

    QUrl exportUrl = QUrl(ImgurConnection::APIuploadURL());
    QUrlQuery q(exportUrl);
    q.addQueryItem(QString::fromLatin1("name"), filePath.fileName());

    // This should be replaced with something the user submits
    q.addQueryItem(QString::fromLatin1("title"), filePath.fileName());

    // This should be replaced with something the user submits
    //q.addQueryItem("caption", "");

    q.addQueryItem(QString::fromLatin1("type"), QString::fromLatin1("file"));
    exportUrl.setQuery(q);

    form.addFile(QString::fromLatin1("image"), filePath.toLocalFile());
    form.finish();

    QNetworkRequest netRequest(exportUrl);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, form.contentType());
    netRequest.setRawHeader("Authorization", "Client-ID " + d->anonymousClientId);
    netRequest.setHeader(QNetworkRequest::UserAgentHeader, d->userAgent);

    d->reply = d->netMngr->post(netRequest, form.formData());
}

bool ImgurTalker::imageRemove(const QString& delete_hash)
{
    // @TODO : make sure it works
    MPForm form;

    QUrl removeUrl = QUrl(ImgurConnection::APIdeleteURL());
    removeUrl.setPath(removeUrl.path() + QLatin1Char('/') + delete_hash + QString::fromLatin1(".json"));

    form.finish();

    QNetworkRequest netRequest(removeUrl);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, form.contentType());
    netRequest.setHeader(QNetworkRequest::UserAgentHeader, d->userAgent);

    d->reply = d->netMngr->post(netRequest, form.formData());

    m_state = IE_REMOVEPHOTO;

    emit signalBusy(true);
    emit signalQueueChanged();

    return true;
}

void ImgurTalker::cancel()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = 0;
    }

    emit signalBusy(false);
}

void ImgurTalker::slotAddItems(const QList<QUrl>& list)
{
    if (list.isEmpty())
    {
        return;
    }

    for( QList<QUrl>::ConstIterator it = list.begin(); it != list.end(); ++it )
    {
        if (!m_queue->contains(*it))
        {
            m_queue->append(*it);
        }
    }

    emit signalQueueChanged();
}

void ImgurTalker::slotRemoveItems(const QList<QUrl> &list)
{
    if (list.isEmpty())
    {
        return;
    }

    for( QList<QUrl>::ConstIterator it = list.begin(); it != list.end(); ++it )
    {
        m_queue->removeAll(*it);
    }

    emit signalQueueChanged();
}

void ImgurTalker::setCurrentUrl(const QUrl& url)
{
    m_currentUrl = url;
}

QList<QUrl>* ImgurTalker::imageQueue() const
{
    return m_queue;
}

QUrl ImgurTalker::currentUrl() const
{
    return m_currentUrl;
}

} // namespace KIPIImgurPlugin
