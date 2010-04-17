/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009 by Luka Renko <lure at kubuntu dot org>
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

#include "picasawebtalker.moc"

// C++ includes

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

// Qt includes

#include <QTextDocument>
#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QLinkedList>
#include <QStringList>
#include <QUrl>
#include <QPointer>

// KDE includes

#include <kapplication.h>
#include <kcodecs.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/jobuidelegate.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kurl.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "mpform.h"
#include "picasawebitem.h"
#include "picasaweblogin.h"
#include "picasawebwindow.h"
#include "pluginsversion.h"

class PicasawebLogin;

namespace KIPIPicasawebExportPlugin
{

PicasawebTalker::PicasawebTalker( QWidget* parent )
               : m_parent( parent ),  m_job( 0 )
    {
        connect(this, SIGNAL(signalError(const QString&)),
                this, SLOT(slotError(const QString&)));
    }

PicasawebTalker::~PicasawebTalker()
{
    if (m_job)
        m_job->kill();
}

void PicasawebTalker::getToken(const QString& username, const QString& password )
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QString url = "https://www.google.com/accounts/ClientLogin";

    QPointer<PicasawebLogin> loginDialog = new PicasawebLogin(kapp->activeWindow(), i18n("Login"), username, password);

    QString username_edit, password_edit;

    if (!loginDialog)
    {
        kDebug() << " Out of memory error " ;
    }

    if (loginDialog->exec() == QDialog::Accepted)
    {
        username_edit = loginDialog->username();
        password_edit = loginDialog->password();
        delete loginDialog;
    }
    else
    {
        delete loginDialog;
        //Return something which say authentication needed.
        return ;
    }

    m_username    = username_edit;
    QString accountType = "GOOGLE";

    QStringList qsl;
    // do not encode username to support email adress
    qsl.append("Email="+username_edit);
    qsl.append("Passwd="+QUrl::toPercentEncoding(password_edit));
    qsl.append("accountType="+accountType);
    qsl.append("service=lh2");
    qsl.append("source=kipi-picasaweb-client");
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());
    KIO::TransferJob* job = KIO::http_post(url, buffer, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_state = FE_GETTOKEN;
    emit signalLoginProgress(1, 2, "Getting the token");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_job = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void PicasawebTalker::authenticate(const QString& token, const QString& username, const QString& password)
{
    if (!token.isNull() || token.length() > 0)
    {
        kDebug() << " Checktoken being called" << token ;
        m_username = username;
        m_password = password; //this would be needed if the checktoken failed
                                //we would need to reauthenticate using auth
        m_token = token;
        checkToken(token);
    }
    else
    {
        getToken(username, password);
    }
}

void PicasawebTalker::checkToken(const QString& token)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KUrl url("http://picasaweb.google.com/data/feed/api");
    url.addPath("/user/" + m_username);
    kDebug() << " token value is " << token ;
    QString auth_string = "GoogleLogin auth=" + token;
    KIO::TransferJob* job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_CHECKTOKEN;
    emit signalLoginProgress(1, 2, "Checking if previous token is still valid");
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

/** PicasaWeb's Album listing request/response
  * First a request is sent to the url below and then we might(?) get a redirect URL
  * WE then need to send the GET request to the Redirect url (this however gets taken care off by the
  * KIO libraries.
  * This uses the authenticated album list fetching to get all the albums included the unlisted-albums
  * which is not returned for an unauthorised request as done without the Authorization header.
*/
void PicasawebTalker::listAlbums(const QString& username)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    KUrl url("http://picasaweb.google.com/data/feed/api");
    // do not encode username to support email adress
    url.addPath("/user/" + username);
    KIO::TransferJob* job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    if (!m_token.isEmpty())
    {
        QString auth_string = "GoogleLogin auth=" + m_token;
        job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );
    }

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_LISTALBUMS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void PicasawebTalker::listPhotos(const QString& username,
                                 const QString& albumId)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    KUrl url("http://picasaweb.google.com/data/feed/api");
    url.addPath("/user/" + username);
    url.addPath("/albumid/" + albumId);
    url.addQueryItem("thumbsize", "200");
    KIO::TransferJob* job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    if (!m_token.isEmpty())
    {
        QString auth_string = "GoogleLogin auth=" + m_token;
        job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );
    }

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_LISTPHOTOS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void PicasawebTalker::createAlbum(const PicasaWebAlbum& album)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    //Create the Body in atom-xml
    QDomDocument docMeta;
    QDomProcessingInstruction instr = docMeta.createProcessingInstruction(
                      "xml", "version='1.0' encoding='UTF-8'");
    docMeta.appendChild(instr);
    QDomElement entryElem = docMeta.createElement("entry");
    docMeta.appendChild(entryElem);
    entryElem.setAttribute("xmlns", "http://www.w3.org/2005/Atom");
    QDomElement titleElem = docMeta.createElement("title");
    entryElem.appendChild(titleElem);
    QDomText titleText = docMeta.createTextNode(Qt::escape(album.title));
    titleElem.appendChild(titleText);
    QDomElement summaryElem = docMeta.createElement("summary");
    entryElem.appendChild(summaryElem);
    QDomText summaryText = docMeta.createTextNode(Qt::escape(album.description));
    summaryElem.appendChild(summaryText);
    QDomElement locationElem = docMeta.createElementNS("http://schemas.google.com/photos/2007", "gphoto:location");
    entryElem.appendChild(locationElem);
    QDomText locationText = docMeta.createTextNode(Qt::escape(album.location));
    locationElem.appendChild(locationText);
    QDomElement accessElem = docMeta.createElementNS("http://schemas.google.com/photos/2007", "gphoto:access");
    entryElem.appendChild(accessElem);
    QDomText accessText = docMeta.createTextNode(Qt::escape(album.access));
    accessElem.appendChild(accessText);
    QDomElement commentElem = docMeta.createElementNS("http://schemas.google.com/photos/2007", "gphoto:commentingEnabled");
    entryElem.appendChild(commentElem);
    QDomText commentText = docMeta.createTextNode(Qt::escape(album.canComment ? "true" : "false"));
    commentElem.appendChild(commentText);
    QDomElement timestampElem = docMeta.createElementNS("http://schemas.google.com/photos/2007", "gphoto:timestamp");
    entryElem.appendChild(timestampElem);
    QDomText timestampText = docMeta.createTextNode(Qt::escape(album.timestamp));
    timestampElem.appendChild(timestampText);
    QDomElement categoryElem = docMeta.createElement("category");
    entryElem.appendChild(categoryElem);
    categoryElem.setAttribute("scheme", "http://schemas.google.com/g/2005#kind");
    categoryElem.setAttribute("term", "http://schemas.google.com/photos/2007#album");
    QDomElement mediaGroupElem = docMeta.createElementNS("http://search.yahoo.com/mrss/", "media:group");
    entryElem.appendChild(mediaGroupElem);
    QDomElement mediaKeywordsElem = docMeta.createElementNS("http://search.yahoo.com/mrss/", "media:keywords");
    mediaGroupElem.appendChild(mediaKeywordsElem);
    QDomText mediaKeywordsText = docMeta.createTextNode(Qt::escape(album.tags.join(",")));
    mediaKeywordsElem.appendChild(mediaKeywordsText);

    QByteArray buffer;
    buffer.append(docMeta.toString().toUtf8());

    KUrl url("http://picasaweb.google.com/data/feed/api");
    url.addPath("/user/" + m_username);
    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::http_post(url, buffer, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/atom+xml");
    job->addMetaData("content-length", QString("Content-Length: %1").arg(buffer.length()));
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string ); 

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_CREATEALBUM;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

bool PicasawebTalker::addPhoto(const QString& photoPath, PicasaWebPhoto& info,
                               const QString& albumId)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KUrl url("http://picasaweb.google.com/data/feed/api");
    url.addPath("/user/" + m_username);
    url.addPath("/albumid/" + albumId);
    QString     auth_string = "GoogleLogin auth=" + m_token;
    MPForm      form;

    //Create the Body in atom-xml
    QDomDocument docMeta;
    QDomProcessingInstruction instr = docMeta.createProcessingInstruction(
                      "xml", "version='1.0' encoding='UTF-8'");
    docMeta.appendChild(instr);
    QDomElement entryElem = docMeta.createElement("entry");
    docMeta.appendChild(entryElem);
    entryElem.setAttribute("xmlns", "http://www.w3.org/2005/Atom");
    QDomElement titleElem = docMeta.createElement("title");
    entryElem.appendChild(titleElem);
    QDomText titleText = docMeta.createTextNode(Qt::escape(info.title));
    titleElem.appendChild(titleText);
    QDomElement summaryElem = docMeta.createElement("summary");
    entryElem.appendChild(summaryElem);
    QDomText summaryText = docMeta.createTextNode(Qt::escape(info.description));
    summaryElem.appendChild(summaryText);
    QDomElement categoryElem = docMeta.createElement("category");
    entryElem.appendChild(categoryElem);
    categoryElem.setAttribute("scheme", "http://schemas.google.com/g/2005#kind");
    categoryElem.setAttribute("term", "http://schemas.google.com/photos/2007#photo");
    QDomElement mediaGroupElem = docMeta.createElementNS("http://search.yahoo.com/mrss/", "media:group");
    entryElem.appendChild(mediaGroupElem);
    QDomElement mediaKeywordsElem = docMeta.createElementNS("http://search.yahoo.com/mrss/", "media:keywords");
    mediaGroupElem.appendChild(mediaKeywordsElem);
    QDomText mediaKeywordsText = docMeta.createTextNode(Qt::escape(info.tags.join(",")));
    mediaKeywordsElem.appendChild(mediaKeywordsText);
    if (!info.gpsLat.isEmpty() && !info.gpsLon.isEmpty())
    {
        QDomElement whereElem = docMeta.createElementNS("http://www.georss.org/georss", "georss:where");
        entryElem.appendChild(whereElem);
        QDomElement pointElem = docMeta.createElementNS("http://www.opengis.net/gml", "gml:Point");
        whereElem.appendChild(pointElem);
        QDomElement gpsElem = docMeta.createElementNS("http://www.opengis.net/gml", "gml:pos");
        pointElem.appendChild(gpsElem);
        QDomText gpsVal = docMeta.createTextNode(info.gpsLat + " " + info.gpsLon);
        gpsElem.appendChild(gpsVal);
    }

    form.addPair("descr", docMeta.toString(), "application/atom+xml");

    if (!form.addFile("photo", photoPath))
        return false;

    form.finish();

    KIO::TransferJob* job = KIO::http_post(url, form.formData(), KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("content-length", QString("Content-Length: %1").arg(form.formData().length()));
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string + "\nMIME-version: 1.0" );

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    return true;
}

bool PicasawebTalker::updatePhoto(const QString& photoPath, PicasaWebPhoto& info)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    MPForm      form;

    //Create the Body in atom-xml
    QDomDocument docMeta;
    QDomProcessingInstruction instr = docMeta.createProcessingInstruction(
                      "xml", "version='1.0' encoding='UTF-8'");
    docMeta.appendChild(instr);
    QDomElement entryElem = docMeta.createElement("entry");
    docMeta.appendChild(entryElem);
    entryElem.setAttribute("xmlns", "http://www.w3.org/2005/Atom");
    QDomElement titleElem = docMeta.createElement("title");
    entryElem.appendChild(titleElem);
    QDomText titleText = docMeta.createTextNode(Qt::escape(info.title));
    titleElem.appendChild(titleText);
    QDomElement summaryElem = docMeta.createElement("summary");
    entryElem.appendChild(summaryElem);
    QDomText summaryText = docMeta.createTextNode(Qt::escape(info.description));
    summaryElem.appendChild(summaryText);
    QDomElement categoryElem = docMeta.createElement("category");
    entryElem.appendChild(categoryElem);
    categoryElem.setAttribute("scheme", "http://schemas.google.com/g/2005#kind");
    categoryElem.setAttribute("term", "http://schemas.google.com/photos/2007#photo");
    QDomElement mediaGroupElem = docMeta.createElementNS("http://search.yahoo.com/mrss/", "media:group");
    entryElem.appendChild(mediaGroupElem);
    QDomElement mediaKeywordsElem = docMeta.createElementNS("http://search.yahoo.com/mrss/", "media:keywords");
    mediaGroupElem.appendChild(mediaKeywordsElem);
    QDomText mediaKeywordsText = docMeta.createTextNode(Qt::escape(info.tags.join(",")));
    mediaKeywordsElem.appendChild(mediaKeywordsText);
    if (!info.gpsLat.isEmpty() && !info.gpsLon.isEmpty())
    {
        QDomElement whereElem = docMeta.createElementNS("http://www.georss.org/georss", "georss:where");
        entryElem.appendChild(whereElem);
        QDomElement pointElem = docMeta.createElementNS("http://www.opengis.net/gml", "gml:Point");
        whereElem.appendChild(pointElem);
        QDomElement gpsElem = docMeta.createElementNS("http://www.opengis.net/gml", "gml:pos");
        pointElem.appendChild(gpsElem);
        QDomText gpsVal = docMeta.createTextNode(info.gpsLat + " " + info.gpsLon);
        gpsElem.appendChild(gpsVal);
    }

    form.addPair("descr", docMeta.toString(), "application/atom+xml");

    if (!form.addFile("photo", photoPath))
        return false;

    form.finish();

    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::put(info.editUrl, -1, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("content-length", QString("Content-Length: %1").arg(form.formData().length()));
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string + "\nIf-Match: *");

    m_jobData.insert(job, form.formData());

    connect(job, SIGNAL(dataReq(KIO::Job*, QByteArray&)),
            this, SLOT(dataReq(KIO::Job*, QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_UPDATEPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    return true;
}

void PicasawebTalker::getPhoto(const QString& imgPath)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KIO::TransferJob* job = KIO::get(imgPath, KIO::Reload, KIO::HideProgressInfo);
    //job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_GETPHOTO;
    m_job   = job;
    m_buffer.resize(0);
}

QString PicasawebTalker::getUserName()
{
    return m_username;
}

void PicasawebTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(false);
}

void PicasawebTalker::info(KIO::Job* /*job*/, const QString& /*str*/)
{
}

void PicasawebTalker::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

void PicasawebTalker::dataReq(KIO::Job* job, QByteArray& data)
{
    if (m_jobData.contains(job))
    {
        data = m_jobData.value(job);
        m_jobData.remove(job);
    }
}

void PicasawebTalker::slotError(const QString & error)
{
    QString transError;
    int     errorNo = 0;

    if (!error.isEmpty())
        errorNo = error.toInt();

    switch (errorNo)
    {
        case 2:
            transError=i18n("No photo specified");break;
        case 3:
            transError=i18n("General upload failure");break;
        case 4:
            transError=i18n("File-size was zero");break;
        case 5:
            transError=i18n("File-type was not recognized");break;
        case 6:
            transError=i18n("User exceeded upload limit");break;
        case 96:
            transError=i18n("Invalid signature"); break;
        case 97:
            transError=i18n("Missing signature"); break;
        case 98:
            transError=i18n("Login failed / Invalid auth token"); break;
        case 100:
            transError=i18n("Invalid API Key"); break;
        case 105:
            transError=i18n("Service currently unavailable");break;
        case 108:
            transError=i18n("Invalid Frob");break;
        case 111:
            transError=i18n("Format \"xxx\" not found"); break;
        case 112:
            transError=i18n("Method \"xxx\" not found"); break;
        case 114:
            transError=i18n("Invalid SOAP envelope");break;
        case 115:
            transError=i18n("Invalid XML-RPC Method Call");break;
        case 116:
            transError=i18n("The POST method is now required for all setters."); break;
        default:
            transError=i18n("Unknown error");
    };

    KMessageBox::error(kapp->activeWindow(), i18n("Error occurred: %1\nUnable to proceed further.",transError + error));
}

void PicasawebTalker::slotResult(KJob *job)
{
    m_job = 0;
    emit signalBusy(false);

    if (job->error())
    {
        if (m_state == FE_ADDPHOTO)
        {
            emit signalAddPhotoDone(job->error(), job->errorText(), "");
        }
        else
        {
            static_cast<KIO::Job*>(job)->ui()->showErrorMessage();
        }

        return;
    }
    if (static_cast<KIO::TransferJob*>(job)->isErrorPage())
    {
        if (m_state == FE_CHECKTOKEN) 
        {
            kDebug() << " Error encountered in checking token, require user credentials" ;
            return getToken(m_username, "");
        }
    }
    switch(m_state)
    {
        case(FE_LOGIN):
            //parseResponseLogin(m_buffer);
            break;
        case(FE_CREATEALBUM):
            parseResponseCreateAlbum(m_buffer);
            break;
        case(FE_LISTALBUMS):
            parseResponseListAlbums(m_buffer);
            break;
        case(FE_GETTOKEN):
            parseResponseGetToken(m_buffer);
            break;
        case(FE_CHECKTOKEN):
            parseResponseCheckToken(m_buffer);
            break;
        case(FE_LISTPHOTOS):
            parseResponseListPhotos(m_buffer);
            break;
        case(FE_ADDPHOTO):
            parseResponseAddPhoto(m_buffer);
            break;
        case(FE_UPDATEPHOTO):
            emit signalAddPhotoDone(0, "", "");
            break;
        case(FE_GETPHOTO):
            // all we get is data of the image
            emit signalGetPhotoDone(0, QString(), m_buffer);
            break;
    }
}

void PicasawebTalker::parseResponseCheckToken(const QByteArray& /*data*/)
{
    bool success = true;
    // TODO(vardhman): Fix this with proper error handling.
    if(!success)
        getToken(m_username, m_password);
    emit signalLoginDone(0, "");
}

void PicasawebTalker::parseResponseGetToken(const QByteArray& data)
{
    bool success = false;
    QString errorString;
    QString str(data);
    //Check the response code should it be 200, proceed
    //if it is 403 handle the error mesg
    //figure out the auth string from this response

    if (str.contains("Auth="))
    {
        QStringList strList = str.split("Auth=");
        if ( strList.count() > 0 )
        {
            m_token = strList[1].trimmed();
            kDebug() << " m_token as obtained in token Response " << m_token ;
            success = true;
        }
    }

    if(success)
    {
        emit signalLoginDone(0, "");
    }
    else
    {
        emit signalError("98");
    }
}

void PicasawebTalker::parseResponseListAlbums(const QByteArray& data)
{
    QDomDocument doc( "feed" );
    if ( !doc.setContent( data ) )
    {
        emit signalListAlbumsDone(1, i18n("Failed to fetch photo-set list"), QList<PicasaWebAlbum>());
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
    QDomElement e;

    QList<PicasaWebAlbum> albumList;

    while(!node.isNull())
    {
        if(node.isElement() && node.nodeName() == "gphoto:user")
        {
            m_username = node.toElement().text();
        }

        if (node.isElement() && node.nodeName() == "entry")
        {
            e = node.toElement();
            QDomNode details=e.firstChild();
            PicasaWebAlbum fps;
            QDomNode detailsNode = details;

            while(!detailsNode.isNull())
            {
                if(detailsNode.isElement())
                {
                    if(detailsNode.nodeName() == "gphoto:id")
                    {
                        fps.id = detailsNode.toElement().text();
                    }

                    if(detailsNode.nodeName() == "title")
                    {
                        fps.title = detailsNode.toElement().text();
                    }

                    if(detailsNode.nodeName()=="gphoto:access")
                    {
                        fps.access = detailsNode.toElement().text();
                    }
                }
                detailsNode = detailsNode.nextSibling();
            }
            albumList.append(fps);
        }
        node = node.nextSibling();
    }

    emit signalListAlbumsDone(0, "", albumList);
}

void PicasawebTalker::parseResponseListPhotos(const QByteArray& data)
{
    QDomDocument doc( "feed" );
    if ( !doc.setContent( data ) )
    {
        emit signalListPhotosDone(1, i18n("Failed to fetch photo-set list"), QList<PicasaWebPhoto>());
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();

    QList<PicasaWebPhoto> photoList;

    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "entry")
        {
            QDomNode details = node.firstChild();
            PicasaWebPhoto fps;
            QDomNode detailsNode = details;

            while(!detailsNode.isNull())
            {
                if(detailsNode.isElement())
                {
                    QDomElement detailsElem = detailsNode.toElement();
                    if(detailsNode.nodeName() == "gphoto:id")
                    {
                        fps.id = detailsElem.text();
                    }

                    if(detailsNode.nodeName() == "title")
                    {
                        fps.title = detailsElem.text();
                    }

                    if(detailsNode.nodeName()=="gphoto:access")
                    {
                        fps.access = detailsElem.text();
                    }

                    if (detailsNode.nodeName() == "link" && detailsElem.attribute("rel") == "edit-media")
                    {
                        fps.editUrl = detailsElem.attribute("href");
                    }

                    if(detailsNode.nodeName()=="georss:where")
                    {
                        QDomNode geoPointNode = detailsElem.namedItem("gml:Point");
                        if (!geoPointNode.isNull() && geoPointNode.isElement())
                        {
                            QDomNode geoPosNode = geoPointNode.toElement().namedItem("gml:pos");
                            if (!geoPosNode.isNull() && geoPosNode.isElement())
                            {
                                QStringList value = geoPosNode.toElement().text().split(" ");
                                if (value.size() == 2)
                                {
                                    fps.gpsLat = value[0];
                                    fps.gpsLon = value[1];
                                }
                            }
                        }
                    }

                    if(detailsNode.nodeName()=="media:group")
                    {
                        QDomNode thumbNode = detailsElem.namedItem("media:thumbnail");
                        if (!thumbNode.isNull() && thumbNode.isElement())
                        {
                            fps.thumbURL = thumbNode.toElement().attribute("url", "");
                        }
                        QDomNode keywordNode = detailsElem.namedItem("media:keywords");
                        if (!keywordNode.isNull() && keywordNode.isElement())
                        {
                            fps.tags = keywordNode.toElement().text().split(",");
                        }

                        QDomNodeList contentsList = detailsElem.elementsByTagName("media:content");
                        for(int i = 0; i < contentsList.size(); i++)
                        {
                            QDomElement contentElem = contentsList.at(i).toElement();
                            if (!contentElem.isNull())
                            {
                                if ((contentElem.attribute("medium") == "image") &&
                                    fps.originalURL.isEmpty())
                                {
                                    fps.originalURL = contentElem.attribute("url");
                                    fps.mimeType = contentElem.attribute("type");
                                }

                                if ((contentElem.attribute("medium") == "video") &&
                                    (contentElem.attribute("type") == "video/mpeg4"))
                                {
                                    fps.originalURL = contentElem.attribute("url");
                                    fps.mimeType = contentElem.attribute("type");
                                }
                            }
                        }
                    }
                }
                detailsNode = detailsNode.nextSibling();
            }
            photoList.append(fps);
        }
        node = node.nextSibling();
    }

    emit signalListPhotosDone(0, "", photoList);
}

void PicasawebTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    bool success = false;

    QDomDocument doc( "AddPhoto Response" );
    if ( !doc.setContent( data ) )
    {
        signalCreateAlbumDone(1, i18n("Failed to create album"), "-1");
        return;
    }

    // parse the new album name
    QDomElement docElem = doc.documentElement();
    QString albumId("");
    if (docElem.nodeName() == "entry")
    {
        success = true;
        QDomNode node = docElem.firstChild(); //this should mean <entry>
        while( !node.isNull() )
        {
            if ( node.isElement())
            {
                if (node.nodeName() == "gphoto:id")
                {
                    albumId = node.toElement().text();
                }
             }
            node = node.nextSibling();
        }
    }

    if(success == true)
    {
        signalCreateAlbumDone(0, "", albumId);
    }
    else
    {
        signalCreateAlbumDone(1, i18n("Failed to create album"), "-1");
    }
}

void PicasawebTalker::parseResponseAddPhoto(const QByteArray& data)
{
    QDomDocument doc( "AddPhoto Response" );
    if ( !doc.setContent( data ) )
    {
        emit signalAddPhotoDone(1, i18n("Failed to upload photo"), "");
        return;
    }

    // parse the new album name
    QDomElement docElem = doc.documentElement();
    QString photoId("");
    if (docElem.nodeName() == "entry")
    {
        QDomNode node = docElem.firstChild(); //this should mean <entry>
        while(!node.isNull())
        {
            if (node.isElement())
            {
                if (node.nodeName() == "gphoto:id")
                {
                    photoId = node.toElement().text();
                }
            }
            node = node.nextSibling();
        }
    }

    emit signalAddPhotoDone(0, "", photoId);
}

} // KIPIPicasawebExportPlugin
