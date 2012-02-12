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

// qt
#include <QVariant>

// kde
#include <KDebug>
#include <KIO/Job>
#include <qjson/parser.h>

// local
#include "mpform.h"

// kipi
#include <libkipi/imagecollection.h>
#include "kpversion.h"

using namespace KIPI;

namespace KIPIImgurExportPlugin {
    ImgurTalker::ImgurTalker (Interface *interface, QWidget *parent) {
        m_parent        = parent;
        m_interface     = interface;

        m_job           = 0;
        m_userAgent     = QString("KIPI-Plugins-ImgurTalker/" + kipipluginsVersion());

        m_exportUrl     = KUrl("http://api.imgur.com/2/upload.json");
        m_removeUrl     = KUrl("http://api.imgur.com/2/delete/");

        m_apiKey        = _IMGUR_API_KEY;
    }

    ImgurTalker::~ImgurTalker()
    {
        if (m_job) {
            kDebug() << "Killing job";
            m_job->kill();
        }
    }

    void ImgurTalker::data(KIO::Job* j, const QByteArray& data)
    {
        if (data.isEmpty()) {
            return;
        }

        int oldSize = m_buffer.size();
        int newSize = data.size();

        m_buffer.resize(m_buffer.size() + newSize);
        memcpy(m_buffer.data()+oldSize, data.data(), newSize);

        emit signalUploadProgress(j->percent());
    }

    void ImgurTalker::slotResult (KJob *kjob) {
        KIO::Job* job = static_cast<KIO::Job*>(kjob);

        if ( job->error() ) {
            emit signalError(tr("Upload failed")); //job->errorString()
            kDebug() << "err: " << job->errorString();
        }

        switch(m_state)
        {
            case IE_REMOVEPHOTO:
            break;
            case IE_ADDPHOTO:
            default:
                if (m_currentUrl != NULL) {
                    emit signalUploadDone(m_currentUrl, parseResponseImageUpload(m_buffer));
                }
            break;
        }

        return;
    }

    bool ImgurTalker::parseResponseImageUpload (QByteArray data) {
        bool ok;

        QJson::Parser* p = new QJson::Parser();
        QVariant r = p->parse(data, &ok);

        if (ok) {
            QMap<QString, QVariant> m = r.toMap();
            QString responseType = m.begin().key();
            if (responseType == "error") {
                ImgurError error;
                QMap<QString,QVariant> errData = m.begin().value().toMap();

                for (QMap<QString,QVariant>::iterator it = errData.begin(); it != errData.end(); ++it) {
                    QString v = it.value().toString();
                    if (it.key() == "message") {
                        error.message = v;
                    }
                    if (it.key() == "request") {
                        error.request = v;
                    }
                    if (it.key() == "method") {
                       if ( v == "get") {
                            error.method = ImgurError::GET;
                        }
                        if ( v == "post") {
                            error.method = ImgurError::POST;
                        }
                    }
                    if (it.key() == "format") {
                        if ( v == "json") {
                            error.format = ImgurError::JSON;
                        }
                        if ( v == "xml") {
                            error.format = ImgurError::XML;
                        }
                    }
                    if (it.key() == "parameters") {
                        error.parameters =  v;
                    }
                    kDebug() << "ERROR :" << error.message;
                    emit signalError (QString(error.message)); // p->errorString()
                    return false;
                }
            }

            if (responseType == "upload" ) {
                ImgurSuccess success;
                QMap<QString,QVariant> successData = m.begin().value().toMap();
                for (QMap<QString,QVariant>::iterator it = successData.begin(); it != successData.end(); ++it) {
                    if (it.key() == "image") {
                        QMap<QString, QVariant> v = it.value().toMap();
                        for (QMap<QString,QVariant>::iterator it = v.begin(); it != v.end(); ++it) {
                            QString value = it.value().toString();
                            if (it.key() == "name") {
                                success.image.name = value;
                            }
                            if (it.key() == "title") {
                                success.image.title = value;
                            }
                            if (it.key() == "caption") {
                                success.image.caption = value;
                            }
                            if (it.key() == "hash") {
                                success.image.hash = value;
                            }
                            if (it.key() == "deleteHash") {
                                success.image.deletehash = value;
                            }
                            if (it.key() == "dateTime") {
                                //success.image.datetime = QDateTime(value);
                            }
                            if (it.key() == "type") {
                                success.image.type = value;
                            }
                            if (it.key() == "animated") {
                                success.image.animated = (value == "true");
                            }
                            if (it.key() == "width") {
                                success.image.width = value.toInt();
                            }
                            if (it.key() == "height") {
                                success.image.height = value.toInt();
                            }
                            if (it.key() == "size") {
                                success.image.size = value.toInt();
                            }
                            if (it.key() == "views") {
                                success.image.views = value.toInt();
                            }
                            if (it.key() == "bandwidth") {
                                success.image.bandwidth = value.toLongLong();
                            }
                        }
                    }
                    if (it.key() == "links") {
                        QMap<QString, QVariant> v = it.value().toMap();
                        for (QMap<QString,QVariant>::iterator it = v.begin(); it != v.end(); ++it) {
                            QString value = it.value().toString();
                            if (it.key() == "original") {
                                success.links.original = value;
                            }
                            if (it.key() == "imgur_page") {
                                success.links.imgur_page = value;
                            }
                            if (it.key() == "delete_page") {
                                success.links.delete_page = value;
                            }
                            if (it.key() == "small_square") {
                                success.links.small_square = value;
                            }
                            if (it.key() == "largeThumbnail") {
                                success.links.large_thumbnail = value;
                            }
                        }
                    }

                    kDebug () << "Link:" << success.links.imgur_page;
                    kDebug () << "Delete:" << success.links.delete_page;
                }
            }
        } else {
            emit signalError (tr ("Upload error")); // p->errorString()
            return false;
        }

//        kDebug() << "qJson :" << tr(data);
        return true;
    }

    bool ImgurTalker::imageUpload (KUrl filePath)
    {
        m_currentUrl = filePath;
        emit signalUploadStart(filePath);
        MPForm form;

        m_exportUrl.addQueryItem("key", m_apiKey);

        m_exportUrl.addQueryItem("name", filePath.fileName());
        m_exportUrl.addQueryItem("title", "TEST Kipi Imgur uploader"); // this should be replaced with something the user submits
        m_exportUrl.addQueryItem("caption", ""); // this should be replaced with something the user submits

        m_exportUrl.addQueryItem("type", "file");

        form.addFile("image", filePath.path());
        form.finish();

        KIO::TransferJob* job = KIO::http_post(m_exportUrl, form.formData(), KIO::HideProgressInfo);
        job->addMetaData("content-type", form.contentType());
        job->addMetaData("content-length", QString("Content-Length: %1").arg(form.formData().length()));
        job->addMetaData("UserAgent", m_userAgent);


        connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
                this, SLOT(data(KIO::Job*, const QByteArray&)));

        connect(job, SIGNAL(result(KJob *)),
                this, SLOT(slotResult(KJob *)));

         m_job   = job;

         m_state = IE_ADDPHOTO;
         emit signalBusy(true);

         m_buffer.resize(0);
         return true;
    }

    bool ImgurTalker::imageDelete (QString delete_hash)
    {
        // @TODO : make sure it works
        MPForm form;

        m_removeUrl.addPath(delete_hash + ".json");

        form.finish();

        KIO::TransferJob* job = KIO::http_post(m_removeUrl, form.formData(), KIO::HideProgressInfo);
        job->addMetaData("content-type", form.contentType());
        job->addMetaData("UserAgent", m_userAgent);

        m_state = IE_REMOVEPHOTO;
        emit signalBusy(true);

        return true;
    }

    void ImgurTalker::cancel()
    {
        if (m_job) {
            m_job->kill();
            m_job = 0;
        }
        emit signalBusy(false);
    }

    void ImgurTalker::startUpload() {
        ImageCollection images = m_interface->currentSelection();

        if (images.isValid()) {
            KUrl::List list = images.images();
            for (KUrl::List::ConstIterator it = list.begin(); it != list.end(); ++it) {
                KUrl imageUrl = *it;

                imageUpload(imageUrl);
                //kDebug () << images.images().at(i).pathOrUrl();
            }
        }
    }

    void ImgurTalker::slotAddItems (const KUrl::List &list) {
       if (list.count() == 0)
       {
           return;
       }

      for (KUrl::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
      {
           KUrl imageUrl = *it;

           imageUpload(imageUrl);
      }
    }
}

