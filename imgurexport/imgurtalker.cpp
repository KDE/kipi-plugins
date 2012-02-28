/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010 by Marius Orcisk <marius at habarnam dot ro>
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

using namespace KIPI;

namespace KIPIImgurExportPlugin
{

ImgurTalker::ImgurTalker (Interface *interface, QWidget *parent)
{
    m_parent        = parent;
    m_interface     = interface;

    m_job           = 0;
    m_userAgent     = QString("KIPI-Plugins-ImgurTalker/" + kipipluginsVersion());

    m_apiKey        = _IMGUR_API_KEY;

    m_queue         = new KUrl::List();
    ImageCollection images = interface->currentSelection();

    if (images.isValid())
    {
        slotAddItems(images.images());
    }
}

ImgurTalker::~ImgurTalker()
{
    if (m_job)
    {
        kDebug() << "Killing job";
        m_job->kill();
    }
}

void ImgurTalker::data(KIO::Job* j, const QByteArray& data)
{
    if (data.isEmpty())
    {
        return;
    }

    int oldSize = m_buffer.size();
    int newSize = data.size();

    m_buffer.resize(m_buffer.size() + newSize);
    memcpy(m_buffer.data()+oldSize, data.data(), newSize);

    emit signalUploadProgress(j->percent());
}

void ImgurTalker::slotResult (KJob *kjob)
{
    KIO::Job* job = static_cast<KIO::Job*>(kjob);

    if ( job->error() )
    {
        emit signalError(tr("Upload failed")); //job->errorString()
        kDebug() << "Error :" << job->errorString();
    }

    bool parseOk;
    switch(m_state)
    {
        case IE_REMOVEPHOTO:
            parseOk = parseResponseImageRemove(m_buffer);
            break;

        case IE_ADDPHOTO:
            parseOk = parseResponseImageUpload(m_buffer);
            break;
        default:
            break;
    }

    emit signalUploadDone();
    kDebug()  << m_currentUrl << parseOk << "Emitted the upload done signal";

    m_buffer.resize(0);
    emit signalBusy(false);
    return;
}

bool ImgurTalker::parseResponseImageRemove(QByteArray data)
{
    if (data.isEmpty()) {
        //
    }
    return false;
}

bool ImgurTalker::parseResponseImageUpload (QByteArray data)
{
    bool ok;

    if (data.isEmpty())
        return false;

    QJson::Parser   *p = new QJson::Parser();
    QVariant         r = p->parse(data, &ok);

    if (ok)
    {
        QMap<QString, QVariant> m = r.toMap();
        QString responseType = m.begin().key();

        if (responseType == "error")
        {
            QMap<QString,QVariant> errData = m.begin().value().toMap();

            for (QMap<QString,QVariant>::iterator it = errData.begin(); it != errData.end(); ++it)
            {
                QString v = it.value().toString();

                if (it.key() == "message")
                {
                    m_error.message = v;
                }
                if (it.key() == "request")
                {
                    m_error.request = v;
                }

                if (it.key() == "method")
                {
                    if ( v == "get")
                    {
                        m_error.method = ImgurError::GET;
                    }
                    if ( v == "post")
                    {
                        m_error.method = ImgurError::POST;
                    }
                }

                if (it.key() == "format")
                {
                    if ( v == "json")
                    {
                        m_error.format = ImgurError::JSON;
                    }
                    if ( v == "xml")
                    {
                        m_error.format = ImgurError::XML;
                    }
                }

                if (it.key() == "parameters")
                {
                    m_error.parameters =  v;
                }

                emit signalError (QString(m_error.message)); // p->errorString()
                return false;
            }
        }

        if (responseType == "upload" )
        {
            QMap<QString, QVariant> m_successData = m.begin().value().toMap();

            for (QMap<QString,QVariant>::iterator it = m_successData.begin(); it != m_successData.end(); ++it)
            {
                if (it.key() == "image")
                {
                    QMap<QString, QVariant> v = it.value().toMap();

                    for (QMap<QString,QVariant>::iterator it = v.begin(); it != v.end(); ++it)
                    {
                        QString value = it.value().toString();
                        if (it.key() == "name")
                        {
                            m_success.image.name = value;
                        }
                        if (it.key() == "title")
                        {
                            m_success.image.title = value;
                        }
                        if (it.key() == "caption")
                        {
                            m_success.image.caption = value;
                        }
                        if (it.key() == "hash")
                        {
                            m_success.image.hash = value;
                        }
                        if (it.key() == "deleteHash")
                        {
                            m_success.image.deletehash = value;
                        }
                        if (it.key() == "dateTime")
                        {
                            //m_success.image.datetime = QDateTime(value);
                        }
                        if (it.key() == "type")
                        {
                            m_success.image.type = value;
                        }
                        if (it.key() == "animated")
                        {
                            m_success.image.animated = (value == "true");
                        }
                        if (it.key() == "width")
                        {
                            m_success.image.width = value.toInt();
                        }
                        if (it.key() == "height")
                        {
                            m_success.image.height = value.toInt();
                        }
                        if (it.key() == "size")
                        {
                            m_success.image.size = value.toInt();
                        }
                        if (it.key() == "views")
                        {
                            m_success.image.views = value.toInt();
                        }
                        if (it.key() == "bandwidth")
                        {
                            m_success.image.bandwidth = value.toLongLong();
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
                            m_success.links.original = value;
                        }
                        if (it.key() == "imgur_page")
                        {
                            m_success.links.imgur_page = value;
                        }
                        if (it.key() == "delete_page")
                        {
                            m_success.links.delete_page = value;
                        }
                        if (it.key() == "small_square")
                        {
                            m_success.links.small_square = value;
                        }
                        if (it.key() == "largeThumbnail")
                        {
                            m_success.links.large_thumbnail = value;
                        }
                    }
                }

                kDebug() << "Link:" << m_success.links.imgur_page;
                kDebug() << "Delete:" << m_success.links.delete_page;
            }
        }
    }
    else
    {
        emit signalError (tr ("Upload error")); // p->errorString()
             kDebug() << "Parser error :" << p->errorString();
        return false;
    }


    return true;
}

bool ImgurTalker::imageUpload (KUrl filePath)
{
//        m_buffer.resize(0);

    kDebug() << "Upload image" << filePath;
    m_currentUrl = filePath;

    MPForm form;

    KUrl exportUrl     = KUrl("http://api.imgur.com/2/upload.json");
    exportUrl.addQueryItem("key", m_apiKey);

    exportUrl.addQueryItem("name", filePath.fileName());
    exportUrl.addQueryItem("title", filePath.fileName()); // this should be replaced with something the user submits
//    exportUrl.addQueryItem("caption", ""); // this should be replaced with something the user submits

    exportUrl.addQueryItem("type", "file");

    form.addFile("image", filePath.path());
    form.finish();

    KIO::TransferJob* job = KIO::http_post(exportUrl, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("content-length", QString("Content-Length: %1").arg(form.formData().length()));
    job->addMetaData("UserAgent", m_userAgent);

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

//         m_job   = job;

    m_state = IE_ADDPHOTO;

    emit signalUploadStart(filePath);
    emit signalBusy(true);

    return true;
}

bool ImgurTalker::imageRemove (QString delete_hash)
{
    // @TODO : make sure it works
    MPForm form;

    KUrl removeUrl     = KUrl("http://api.imgur.com/2/delete/");
    removeUrl.addPath(delete_hash + ".json");

    form.finish();

    KIO::TransferJob* job = KIO::http_post(removeUrl, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("UserAgent", m_userAgent);

    m_state = IE_REMOVEPHOTO;

    emit signalBusy(true);

    return true;
}

void ImgurTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(false);
}

//void ImgurTalker::startUpload()
//{
//    ImageCollection images = m_interface->currentSelection();

//    if (images.isValid())
//    {
//        KUrl::List list = images.images();
//        for (KUrl::List::ConstIterator it = list.begin(); it != list.end(); ++it)
//        {
//            KUrl imageUrl = *it;

//            imageUpload(imageUrl);
//            //kDebug() << images.images().at(i).pathOrUrl();
//        }
//    }
//}

void ImgurTalker::slotAddItems (const KUrl::List &list)
{
    if (list.count() == 0)
    {
        return;
    }

    kDebug() << "Appended" << list;
    m_queue->append(list);
}

ImgurSuccess ImgurTalker::success()
{
    return m_success;
}

ImgurError ImgurTalker::error()
{
    return m_error;
}

KUrl::List* ImgurTalker::imageQueue()
{
    return m_queue;
}


} // namespace KIPIImgurExportPlugin
