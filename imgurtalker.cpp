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

namespace KIPIImgurExportPlugin {
    ImgurTalker::ImgurTalker (QObject *parent) {
        m_parent        = parent;

        m_job           = 0;
        m_userAgent     = QString("KIPI-Plugin-ImgurTalker/0.0.1");

        m_exportUrl     = QString("http://api.imgur.com/2/upload.json");
        m_removeUrl     = QString("http://api.imgur.com/2/delete.json");
        \
        m_apiKey        = _IMGUR_API_KEY;
    }

    const QString ImgurTalker::getStatusError (ImgurTalker::ServerStatusCode code) {
        switch (code) {
        case NO_IMAGE:
            return tr ("No image selected");
            break;
        case UPLOAD_FAILED:
            return tr ("Image failed to upload");
            break;
        case TOO_LARGE:
            return tr ("Image larger than 10MB");
            break;
        case INVALID_TYPE:
            return tr ("Invalid image type or URL");
            break;
        case INVALID_KEY:
            return tr ("Invalid API Key");
            break;
        case PROCESS_FAILED:
            return tr ("Upload failed during process");
            break;
        case COPY_FAILED:
            return tr ("Upload failed during the copy process");
            break;
        case THUMBNAIL_FAILED:
            return tr ("Upload failed during thumbnail process");
            break;
        case UPLOAD_LIMIT:
            return tr ("Upload limit reached");
            break;
        case GIF_TOO_LARGE:
            return tr ("Animated GIF is larger than 2MB");
            break;
        case PNG_TOO_LARGE:
            return tr ("Animated PNG is larger than 2MB");
            break;
        case INVALID_URL:
            return tr ("Invalid URL");
            break;
        case URL_UNAVAILABLE:
            return tr ("Could not download the image from that URL");
            break;
        case INVALID_API_REQUEST:
            return tr ("Invalid API request");
            break;
        case INVALID_RESPONSE_FORMAT:
            return tr ("Invalid response format");
            break;
        default:
            return tr ("Unimplemented");
        }
    }

    ImgurTalker::~ImgurTalker()
    {
        if (m_job) {
            kDebug() << "Killing job";
            m_job->kill();
        }
    }

    void ImgurTalker::data(KIO::Job*, const QByteArray& data)
    {
        if (data.isEmpty())
            return;

        int oldSize = m_buffer.size();
        m_buffer.resize(m_buffer.size() + data.size());
        memcpy(m_buffer.data()+oldSize, data.data(), data.size());
    }

//    void ImgurTalker::dataReq(KIO::Job* job, QByteArray& data)
//    {
//        if (m_jobData.contains(job))
//        {
//            data = m_jobData.value(job);
//            m_jobData.remove(job);
//        }
//    }

    void ImgurTalker::slotResult (KJob *kjob) {
        KIO::Job* job = static_cast<KIO::Job*>(kjob);

        if ( job->error() )
            kDebug() << "err: " << job->errorString();

        switch(m_state)
        {
            default:
                kDebug () << m_buffer;
                parseResponseImageUpload(m_buffer);
                emit signalUploadDone(1, "Done");
            break;
        }

//        kDebug () << "buff len" << m_buffer.length() << m_buffer;
        return;
    }

    void ImgurTalker::parseResponseImageUpload (QByteArray data) {
        QJson::Parser* p = new QJson::Parser();
        QVariant r = p->parse(data);

        kDebug() << "qJson :" << r;
        return;
    }

    bool ImgurTalker::imageUpload (QString filePath)
    {
        KUrl url(m_exportUrl);
        MPForm form;

        url.addQueryItem("key", m_apiKey);
        url.addQueryItem("title", "TEST Kipi Imgur uploader");

        form.addFile("image", filePath);
        form.finish();

        KIO::TransferJob* job = KIO::http_post(url, form.formData(), KIO::HideProgressInfo);
        job->addMetaData("content-type", form.contentType());
        job->addMetaData("content-length", QString("Content-Length: %1").arg(form.formData().length()));
        job->addMetaData("UserAgent", m_userAgent);


        connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
                this, SLOT(data(KIO::Job*, const QByteArray&)));

        connect(job, SIGNAL(result(KJob *)),
                this, SLOT(slotResult(KJob *)));

         m_job   = job;

         m_state = FE_ADDPHOTO;
         emit signalBusy(true);

         m_buffer.resize(0);
         return true;
    }

    bool ImgurTalker::imageDelete (QString hash)
    {
     /* TODO */
         m_state = FE_REMOVEPHOTO;
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
}

