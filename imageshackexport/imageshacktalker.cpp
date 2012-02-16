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
{
    m_job         = 0;
    m_imageshack  = imghack;
    m_userAgent   = QString("KIPI-Plugin-Imageshack/%1").arg(kipiplugins_version);
    m_photoApiUrl = KUrl("http://www.imageshack.us/upload_api.php");
    m_videoApiUrl = KUrl("http://render.imageshack.us/upload_api.php");
    m_loginApiUrl = KUrl("http://my.imageshack.us/setlogin.php");
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

    kDebug() << "CALL: " << result;

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
    m_job = 0;
    KIO::Job* job = static_cast<KIO::Job*>(kjob);

    if (job->error())
    {
        if (m_loginInProgress)
        {
            checkRegistrationCodeDone(job->error(), job->errorString());
        }
        else if (m_state == IMGHCK_ADDPHOTO)
        {
            emit signalBusy(false);
            emit signalAddPhotoDone(job->error(), job->errorString());
        }
        return;
    }

    switch (m_state)
    {
        case IMGHCK_CHECKREGCODE:
            parseCheckRegistrationCode(m_buffer);
            break;
        case IMGHCK_ADDPHOTO:
            parseUploadPhotoDone(m_buffer);
        default:
            break;
    }
}


void ImageshackTalker::authenticate()
{
    m_loginInProgress = true;

//     if (m_imageshack->m_registrationCode.isEmpty())
//     {
//         emit signalNeedRegistrationCode();
//     }
//     else
    if (!m_imageshack->m_registrationCode.isEmpty())
    {
        emit signalLoginInProgress(1, 4, i18n("Checking the registration code"));
        checkRegistrationCode();
    }
}

void ImageshackTalker::checkRegistrationCode()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    emit signalLoginInProgress(2, 4, i18n("Checking the web server"));

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
    if (errCode)
    {
//         m_imageshack->m_registrationCode.clear();
        m_imageshack->logOut();
    }
    emit signalBusy(false);
    emit signalLoginDone(errCode, errMsg);
    m_loginInProgress = false;
}

void ImageshackTalker::parseCheckRegistrationCode(const QByteArray& data)
{
    QString errMsg;
    QXmlStreamReader reader(data);
    bool foundResponse;

    emit signalLoginInProgress(3, 4, i18n("Parsing server response"));

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
                kDebug() << m_imageshack->username();
            }
            if (reader.name() == "email")
            {
                reader.readNext();
                m_imageshack->setEmail(reader.text().toString());
                kDebug() << m_imageshack->email();
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
        authenticationDone(0, i18n("Registration code successfuly verified"));
    }
    else
    {
        authenticationDone(1, i18n("Registration code not valid!"));
    }
}

void ImageshackTalker::authenticationDone(int errCode, const QString& errMsg)
{
    kDebug() << errCode << " -- " << errMsg;
    if (errCode)
    {
//         m_imageshack->m_loggedIn = false;
//         m_imageshack->m_registrationCode.clear();
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
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    QMap<QString, QString> args;
    args["key"] = m_appKey;
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
    kDebug() << "FORM" << form.formData() << "--------------------";

    // Check where to upload

    QString mime = mimeType(path);

    if (mime.startsWith("video/"))
    {
        // video file, check supported types
        QString form = mime.split("/").at(1);
    }
    else
    {
        // image file
    }

    KIO::Job *job = KIO::http_post(KUrl(m_photoApiUrl), form.formData(),
                                   KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type", form.contentType());

    connect(job, SIGNAL(data(KIO::Job*, QByteArray)),
            this, SLOT(data(KIO::Job*, QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_job = job;
    m_state = IMGHCK_ADDPHOTO;
    m_buffer.resize(0);
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
        if (e.tagName() == "error") {
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

    kDebug() << "-------------" << elem.tagName() << "------------";
    kDebug() << data;

    if (elem.tagName() == "imginfo")
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

    emit signalBusy(false);
    emit signalAddPhotoDone(errCode, errMsg);
}

} // namespace KIPIImageshackExportPlugin
