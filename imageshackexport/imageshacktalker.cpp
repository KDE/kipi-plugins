/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
*
* This program is free software; you can redistribute it
* and/or modify it under the terms of the GNU General
* Public License as published by the Free Software Foundation;
* either version 2, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* ============================================================ */

#include "imageshacktalker.moc"

// Qt includes

#include <QMap>
#include <QString>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QXmlStreamReader>

// KDE includes

#include <kapplication.h>
#include <kdebug.h>
#include <kio/job.h>
#include <ktoolinvocation.h>
#include <kio/jobuidelegate.h>
#include <kmimetype.h>

// Local includes

#include "kpversion.h"
#include "imageshack.h"
#include "mpform.h"

namespace KIPIImageshackExportPlugin
{

ImageshackTalker::ImageshackTalker(Imageshack* imghack)
    : m_imageshack(imghack), m_loginInProgress(false), m_job(0),
    m_state(IMGHCK_DONOTHING)
{
    m_userAgent   = QString("KIPI-Plugin-Imageshack/%1").arg(kipiplugins_version);
    m_photoApiUrl = KUrl("http://www.imageshack.us/upload_api.php");
    m_videoApiUrl = KUrl("http://render.imageshack.us/upload_api.php");
    m_loginApiUrl = KUrl("http://my.imageshack.us/setlogin.php");
    m_galleryUrl  = KUrl("http://www.imageshack.us/gallery_api.php");
    m_appKey      = "YPZ2L9WV2de2a1e08e8fbddfbcc1c5c39f94f92a";
}

ImageshackTalker::~ImageshackTalker()
{
    if (m_job)
        m_job->kill();
}

bool ImageshackTalker::loggedIn()
{
    return !m_imageshack->registrationCode().isEmpty();
}

void ImageshackTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(false);
}

QString ImageshackTalker::getCallString(QMap< QString, QString >& args)
{
    QString result;
    for (QMap<QString, QString>::const_iterator it = args.constBegin();
         it != args.constEnd();
         ++it)
    {
        if (!result.isEmpty())
            result.append("&");
        result.append(it.key());
        result.append("=");
        result.append(it.value());
    }

    return result;
}

void ImageshackTalker::data(KIO::Job* /*job*/, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

void ImageshackTalker::slotResult(KJob* kjob)
{
    KIO::Job* job = static_cast<KIO::Job*>(kjob);

    if (job->error())
    {
        if (m_loginInProgress)
        {
            checkRegistrationCodeDone(job->error(), job->errorString());
            m_loginInProgress = false;
        }
        else if (m_state == IMGHCK_GETGALLERIES)
        {
            emit signalBusy(false);
            emit signalGetGalleriesDone(job->error(), job->errorString());
        }
        else if (m_state == IMGHCK_ADDPHOTO || m_state == IMGHCK_ADDPHOTOGALLERY)
        {
            emit signalBusy(false);
            emit signalAddPhotoDone(job->error(), job->errorString());
        }
        m_state = IMGHCK_DONOTHING;
        m_job = 0;
        return;
    }

    int step;
    switch (m_state)
    {
        case IMGHCK_CHECKREGCODE:
            m_job = 0;
            parseCheckRegistrationCode(m_buffer);
            break;
        case IMGHCK_ADDPHOTOGALLERY:
            step = job->property("k_step").toInt();
            if (step == STEP_UPLOADITEM)
                parseUploadPhotoDone(m_buffer);
            else
                parseAddPhotoToGalleryDone(m_buffer);
            break;
        case IMGHCK_ADDVIDEO:
        case IMGHCK_ADDPHOTO:
            m_job = 0;
            parseUploadPhotoDone(m_buffer);
            break;
        case IMGHCK_GETGALLERIES:
            m_job = 0;
            parseGetGalleries(m_buffer);
            break;
        default:
            break;
    }
}


void ImageshackTalker::authenticate()
{
    m_loginInProgress = true;

    if (!m_imageshack->m_registrationCode.isEmpty())
    {
        emit signalJobInProgress(1, 4, i18n("Checking the registration code"));
        checkRegistrationCode();
    }
}

void ImageshackTalker::getGalleries()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    emit signalJobInProgress(3, 4, i18n("Getting galleries from server"));

    KUrl gUrl(m_galleryUrl);
    gUrl.addQueryItem("action", "gallery_list");
    gUrl.addQueryItem("user", m_imageshack->username());

    KIO::TransferJob* job = KIO::get(gUrl, KIO::NoReload, KIO::HideProgressInfo);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = IMGHCK_GETGALLERIES;
    m_job = job;
    m_buffer.resize(0);
}

void ImageshackTalker::checkRegistrationCode()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    emit signalJobInProgress(2, 4, i18n("Checking the web server"));

    QString args = "login=";
    args.append(m_imageshack->registrationCode());
    args.append("&xml=yes");

    QByteArray tmp = args.toUtf8();
    KIO::TransferJob* job = KIO::http_post(KUrl(m_loginApiUrl), tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = IMGHCK_CHECKREGCODE;
    m_job   = job;
    m_buffer.resize(0);

}

void ImageshackTalker::checkRegistrationCodeDone(int errCode, const QString& errMsg)
{
    emit signalBusy(false);
    emit signalLoginDone(errCode, errMsg);
    m_loginInProgress = false;
}

void ImageshackTalker::parseGetGalleries(const QByteArray &data)
{
    QDomDocument document;
    if (!document.setContent(data))
        return;

    QDomElement rootElem  = document.documentElement();
    QDomNodeList children = rootElem.childNodes();

    QStringList gTexts;
    QStringList gNames;

    for (int i = 0; i < children.size(); ++i)
    {
        QDomElement e = children.at(i).toElement();
        if (e.tagName() == "gallery")
        {
            QDomElement nameElem   = e.firstChildElement("name");
            QDomElement titleElem  = e.firstChildElement("title");
            QDomElement serverElem = e.firstChildElement("server");

            if (!nameElem.isNull())
            {
                QString fmt;
                fmt          = nameElem.firstChild().toText().data();
                // this is a very ugly hack
                QString name = "http://img" + serverElem.firstChild().toText().data() + ".imageshack.us/gallery_api.php?g=" + fmt;
                gNames << name;

                if (!titleElem.isNull())
                {
                    fmt.append(" (" + titleElem.firstChild().toText().data() + ")");
                }
                gTexts << fmt;
            }
        }
    }

    m_state = IMGHCK_DONOTHING;

    emit signalUpdateGalleries(gTexts, gNames);
    emit signalGetGalleriesDone(0, i18n("Successfully retrieved galleries"));
}

void ImageshackTalker::parseCheckRegistrationCode(const QByteArray& data)
{
    QXmlStreamReader reader(data);
    bool foundResponse;

    while (!reader.atEnd())
    {
        reader.readNext();

        if (reader.isStartElement())
        {
            foundResponse = true;
            if (reader.name() == "exists")
            {
                reader.readNext();
                if (reader.text().toString() == "yes")
                {
                    m_imageshack->m_loggedIn = true;
                }
                else
                {
                    m_imageshack->m_loggedIn = false;
                }
            }
            if (reader.name() == "username")
            {
                reader.readNext();
                m_imageshack->setUsername(reader.text().toString());
            }
            if (reader.name() == "email")
            {
                reader.readNext();
                m_imageshack->setEmail(reader.text().toString());
            }
        }
    }

    if (!foundResponse)
    {
        emit signalLoginDone(2, i18n("Server does not respond. Please check you internet connection!"));
        return;
    }

    if (m_imageshack->loggedIn())
    {
        authenticationDone(0, i18n("Registration code successfully verified"));
    }
    else
    {
        authenticationDone(1, i18n("Registration code not valid!"));
    }
}

void ImageshackTalker::authenticationDone(int errCode, const QString& errMsg)
{
    if (errCode)
    {
        m_imageshack->logOut();
    }

    emit signalBusy(false);
    emit signalLoginDone(errCode, errMsg);
    m_loginInProgress = false;
}

void ImageshackTalker::logOut()
{
    m_imageshack->logOut();
    m_loginInProgress = false;
}

void ImageshackTalker::cancelLogIn()
{
    logOut();
    emit signalLoginDone(-1, "Canceled by the user!");
}

QString ImageshackTalker::mimeType(const QString& path)
{
    KMimeType::Ptr mimePtr = KMimeType::findByUrl(path);
    return mimePtr->name();
}

void ImageshackTalker::uploadItem(QString path, QMap<QString, QString> opts)
{
    uploadItemToGallery(path, "", opts);
}

void ImageshackTalker::uploadItemToGallery(QString path, const QString &/*gallery*/, QMap<QString, QString> opts)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    QMap<QString, QString> args;
    args["key"]        = m_appKey;
    args["fileupload"] = KUrl(path).fileName();

    MPForm form;

    for (QMap<QString, QString>::const_iterator it = opts.constBegin();
         it != opts.constEnd();
         ++it)
    {
        form.addPair(it.key(), it.value());
    }

    for (QMap<QString, QString>::const_iterator it = args.constBegin();
         it != args.constEnd();
         ++it)
    {
        form.addPair(it.key(), it.value());
    }

    if (!form.addFile(KUrl(path).fileName(), path))
    {
        emit signalBusy(false);
        return;
    }

    form.finish();

    // Check where to upload
    QString mime = mimeType(path);

    // TODO support for video uploads
    KUrl uploadUrl;

    if (mime.startsWith(QLatin1String("video/")))
    {
        uploadUrl = KUrl(m_videoApiUrl);
        m_state = IMGHCK_ADDVIDEO;
    }
    else
    {
        // image file
        uploadUrl = KUrl(m_photoApiUrl);
        m_state   = IMGHCK_ADDPHOTO;
    }

    KIO::Job* const job = KIO::http_post(uploadUrl, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type", form.contentType());

    m_job = job;

//    if (gallery.isEmpty())
//    {
//        m_state = IMGHCK_ADDPHOTO;
//    }
//    else
//    {
//        kDebug() << "upload to gallery " << gallery;
//        m_state = IMGHCK_ADDPHOTOGALLERY;
//        job->setProperty("k_galleryName", gallery);
//        m_job->setProperty("k_step", STEP_UPLOADITEM);
//    }
    // TODO implement upload to galleries
    m_buffer.resize(0);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

}

int ImageshackTalker::parseErrorResponse(QDomElement elem, QString& errMsg)
{
    int errCode = -1;
    QString err_code;

    for (QDomNode node = elem.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        QDomElement e = node.toElement();

        if (e.tagName() == "error")
        {
            err_code = e.attributeNode("id").value();
            errMsg = e.text();
        }
    }

    if (err_code == "file_too_big")
    {
        errCode = 501;
    }
    else
    {
        errCode = 502;
    }

    return errCode;
}

void ImageshackTalker::parseUploadPhotoDone(QByteArray data)
{
    int errCode    = -1;
    QString errMsg = "";
    QDomDocument doc("addPhoto");
    if (!doc.setContent(data))
        return;

    QDomElement elem = doc.documentElement();

    if (!elem.isNull() && elem.tagName() == "imginfo")
    {
        errCode = 0;
    }
    else
    {
        kDebug() << elem.tagName();
        QDomNode node = elem.firstChild();
        if (node.nodeName() == "error") {
            errCode =  parseErrorResponse(elem, errMsg);
        }
    }

    if (m_state == IMGHCK_ADDPHOTO || m_state == IMGHCK_ADDVIDEO
            || (m_state == IMGHCK_ADDPHOTOGALLERY && errCode))
    {
        emit signalBusy(false);
        emit signalAddPhotoDone(errCode, errMsg);
    }
    else if (m_state == IMGHCK_ADDPHOTOGALLERY)
    {
        QDomElement chld = elem.firstChildElement("files");
        if (chld.isNull())
        {
            emit signalBusy(false);
            emit signalAddPhotoDone(-2, "Error parsing server response. No information about uploaded item");
        }
        else
        {
            QString server = chld.attribute("server");
            QString bucket = chld.attribute("bucket");

            QDomElement image = chld.firstChildElement("image");
            if (image.isNull() || server.isEmpty() || bucket.isEmpty() || !image.hasChildNodes())
            {
                emit signalBusy(false);
                emit signalAddPhotoDone(-3, "Error parsing server response. No image, server or bucket returned");
            }
            else
            {
                QString imgPath = QString("img") + server + "/" + bucket + "/" + image.firstChild().toText().data();

                KUrl url(m_job->property("k_galleryName").toString());
                url.addQueryItem("action", "add");
                url.addQueryItem("image[]", imgPath);
                url.addQueryItem("cookie", m_imageshack->registrationCode());

                kDebug() << url.url();

                m_job->kill();

                KIO::Job *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);

                m_job = job;
                m_job->setProperty("k_step", STEP_ADDITEMTOGALLERY);

                m_buffer.resize(0);

                connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
                        this, SLOT(data(KIO::Job*,QByteArray)));

                connect(job, SIGNAL(result(KJob*)),
                        this, SLOT(slotResult(KJob*)));
            }
        }
    }
}

void ImageshackTalker::parseAddPhotoToGalleryDone(QByteArray data)
{
//    int errCode = -1;
    QString errMsg = "";
    QDomDocument domDoc("galleryXML");

    kDebug() << data;

    if (!domDoc.setContent(data))
        return;

    QDomElement rootElem = domDoc.documentElement();

    if (rootElem.isNull() || rootElem.tagName() != "gallery")
    {
        // TODO error cheking
    }
    else
    {
        emit signalBusy(false);
        emit signalAddPhotoDone(0, "");
    }
}

} // namespace KIPIImageshackExportPlugin
