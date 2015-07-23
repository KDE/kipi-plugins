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

// KDE includes

#include <kio/job.h>
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
        parent                    = 0;
        interface                 = 0;
        job                       = 0;
        continueUpload            = true;
        userAgent                 = QLatin1String("KIPI-Plugins-ImgurExport") + QLatin1String("/") + kipipluginsVersion();
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
    m_queue      = new QList<QUrl>();
    m_state      = IR_LOGOUT;

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
        qCDebug(KIPIPLUGINS_LOG) << error.message;
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
        qCDebug(KIPIPLUGINS_LOG) << "Error :" << job->errorString();
    }

    parseResponse(d->buffer);

    d->buffer.resize(0);

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
    qCDebug(KIPIPLUGINS_LOG)<<"Upload Image data is "<<data;
    
    if (data.isEmpty())
        return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    bool ok = (err.error == QJsonParseError::NoError);
    
    if(ok)
    {
        QJsonObject jsonObject = doc.object();
        
        if(jsonObject.contains(QString("upload")))
        {
            ImgurSuccess success;
            QJsonObject obj1 = jsonObject["upload"].toObject();
            QJsonObject obj2 = obj1["image"].toObject();
            QJsonObject obj3 = obj1["links"].toObject();
            
            success.image.name = obj2["name"].toString();
            qCDebug(KIPIPLUGINS_LOG)<<"Name is "<<success.image.name;
                    
            success.image.title = obj2["title"].toString();
                    
            success.image.caption = obj2["caption"].toString();
                    
            success.image.hash = obj2["hash"].toString();
                   
            success.image.deletehash = obj2["deletehash"].toString();
                   
            success.image.type = obj2["type"].toString();
                    
            success.image.animated = (obj2["animated"].toString() == "true");
                   
            success.image.width = obj2["width"].toString().toInt();
                    
            success.image.height = obj2["height"].toString().toInt();
                    
            success.image.size = obj2["size"].toString().toInt();
                    
            success.image.views = obj2["views"].toString().toInt();
                   
            success.image.bandwidth = obj2["bandwidth"].toString().toLongLong();
            
            success.links.original = QUrl(obj3["original"].toString());
            qCDebug(KIPIPLUGINS_LOG)<<"Link original is "<<success.links.original;
                    
            success.links.imgur_page = QUrl(obj3["imgur_page"].toString());
                    
            success.links.delete_page = QUrl(obj3["delete_page"].toString());
                    
            success.links.small_square = QUrl(obj3["small_square"].toString());
                    
            success.links.large_thumbnail = QUrl(obj3["largeThumbnail"].toString());        
            
            emit signalSuccess(m_currentUrl, success);
            
        }
        
        if(jsonObject.contains(QString("error")))
        {
            ImgurError error;
            QJsonObject obj = jsonObject["error"].toObject();
            
            error.message   = obj["message"].toString();
                
            error.request = obj["request"].toString();
                
            error.parameters = obj["parameters"].toString();
                
            if((QString::compare(obj["method"].toString(), QString("get"), Qt::CaseInsensitive) == 0))
                error.method = ImgurError::GET;
                
            if((QString::compare(obj["method"].toString(), QString("post"), Qt::CaseInsensitive) == 0))
                error.method = ImgurError::POST;
                
            if((QString::compare(obj["format"].toString(), QString("json"), Qt::CaseInsensitive) == 0))
                error.format = ImgurError::JSON;
                
            if((QString::compare(obj["format"].toString(), QString("xml"), Qt::CaseInsensitive) == 0))
                error.format = ImgurError::XML;
 
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

    exportUrl.addQueryItem("key",   d->anonymousKey.data());
    exportUrl.addQueryItem("name",  filePath.fileName());
    exportUrl.addQueryItem("title", filePath.fileName()); // this should be replaced with something the user submits
    //exportUrl.addQueryItem("caption", ""); // this should be replaced with something the user submits

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

    QUrl removeUrl = QUrl(ImgurConnection::APIdeleteURL());
    removeUrl.setPath(removeUrl.path() + '/' + delete_hash + ".json");

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
