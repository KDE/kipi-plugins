/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "picasawebtalker.h"
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

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

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
    qsl.append("Email="+username_edit);
    qsl.append("Passwd="+password_edit);
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
    //authProgressDlg->setLabelText(i18n("Getting the token"));

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

    QString url = "http://picasaweb.google.com/data/feed/api";
    url.append("/user/" + QUrl::toPercentEncoding(m_username));
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
    QString url = "http://picasaweb.google.com/data/feed/api";
    url.append("/user/" + QUrl::toPercentEncoding(username));
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

void PicasawebTalker::addPhotoTag(const QString& photoURI, const QString& photoId,
                                  const QByteArray& metadataXML)
{
    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::put(photoURI, -1, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/atom+xml");
    job->addMetaData("content-length", QString("Content-Length: %1").arg(metadataXML.length()));
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );

    JobData dataTmp;
    dataTmp.data = metadataXML;
    dataTmp.id = photoId;
    m_jobData.insert(job, dataTmp);

    connect(job, SIGNAL(dataReq(KIO::Job*, QByteArray&)),
            this, SLOT(dataReq(KIO::Job*, QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_ADDTAG;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

void PicasawebTalker::listPhotos(const QString& username,
                                 const QString& albumId)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    QString url = "http://picasaweb.google.com/data/feed/api";
    url.append("/user/" + QUrl::toPercentEncoding(username));
    url.append("/albumid/" + albumId);
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

    QString newAlbumXML = QString("<entry xmlns='http://www.w3.org/2005/Atom' "
                                "xmlns:media='http://search.yahoo.com/mrss/' "
                                "xmlns:gphoto='http://schemas.google.com/photos/2007'> "
                                "<title type='text'>%1</title> "
                                "<summary type='text'>%2</summary> "
                                "<gphoto:location>%3</gphoto:location> "
                                "<gphoto:access>%4</gphoto:access> "
                "<gphoto:commentingEnabled>%5</gphoto:commentingEnabled> "
                "<gphoto:timestamp>%6</gphoto:timestamp> "
                "<media:group> "
                "<media:keywords>%7</media:keywords> "
                "</media:group> "
                "<category scheme='http://schemas.google.com/g/2005#kind' "
                        "term='http://schemas.google.com/photos/2007#album'></category> "
                        "</entry> ").arg(album.title)
                                    .arg(album.description)
                                    .arg(album.location)
                                    .arg(album.access)
                                    .arg(album.canComment ? "true" : "false")
                                    .arg(album.timestamp)
                                    .arg(album.tags.join(","));

    QByteArray buffer;
    buffer.append(newAlbumXML.toUtf8());

    MPForm form;
    QString url = "http://picasaweb.google.com/data/feed/api";
    url.append("/user/" + QUrl::toPercentEncoding(m_username));
    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::http_post(url, buffer, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/atom+xml");
    job->addMetaData("content-length", QString("Content-Length: %1").arg(newAlbumXML.length()));
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

    QString album_id = albumId;

    if (album_id.length() == 0)
        album_id = "test";

    QString url = "http://picasaweb.google.com/data/feed/api";
    url.append("/user/" + QUrl::toPercentEncoding(m_username));
    url.append("/albumid/" + albumId);
    QString     auth_string = "GoogleLogin auth=" + m_token;
    QStringList headers;
    MPForm      form;

    //Create the Body in atom-xml
    QStringList body_xml;
    body_xml.append("<entry xmlns=\'http://www.w3.org/2005/Atom\'>");
    body_xml.append("<title>"+ Qt::escape(info.title) +"</title>");
    body_xml.append("<summary>"+ Qt::escape(info.description) +"</summary>");
    body_xml.append("<category scheme=\"http://schemas.google.com/g/2005#kind\" "
                    "term=\"http://schemas.google.com/photos/2007#photo\" />");
    body_xml.append("</entry>");

    QString body = body_xml.join("");

    form.addPair("descr", body, "application/atom+xml");

    // save the tags for this photo in to the tags hashmap
    tags_map.insert(info.title, info.tags);

    if (!form.addFile("photo", photoPath))
        return false;

    form.finish();

    KIO::TransferJob* job = KIO::http_post(url, form.formData(), KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );

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
    QString output_data = QString(data);
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

void PicasawebTalker::dataReq(KIO::Job* job, QByteArray& data)
{
    if (m_jobData.contains(job))
    {
        data = m_jobData.value(job).data;
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
        case(FE_GETPHOTO):
            // all we get is data of the image
            emit signalGetPhotoDone(0, QString(), m_buffer);
            break;
        case(FE_ADDTAG):
            {
                QString photoId;
                KIO::Job* jobKIO = qobject_cast<KIO::Job *>(job);
                if ((jobKIO != 0) && (m_jobData.contains(jobKIO)))
                {
                    photoId = m_jobData.value(jobKIO).id;
                    m_jobData.remove(jobKIO);
                }
                emit signalAddPhotoDone(0, "", photoId);
            }
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

                    if(detailsNode.nodeName()=="content")
                    {
                        fps.originalURL = detailsElem.attribute("src", "");
                        fps.mimeType = detailsElem.attribute("type", "");
                    }

                    if (detailsNode.nodeName() == "link" && detailsElem.attribute("rel") == "edit")
                    {
                        fps.editUrl = detailsElem.attribute("href");
                    }

                   if(detailsNode.nodeName()=="media:group")
                    {
                        QDomNode mediaNode = detailsElem.namedItem("media:thumbnail");
                        if (!mediaNode.isNull() && mediaNode.isElement())
                        {
                            QDomElement mediaElem = mediaNode.toElement();
                            fps.thumbURL = mediaElem.attribute("url", "");
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
    bool success = false;
    QString     line;
    QString str(data);
    success = 1;
    QDomDocument doc( "AddPhoto Response" );

    if ( !doc.setContent( data ) )
    {
        emit signalAddPhotoDone(1, i18n("Failed to upload photo"), "");
        return;
    }

    // parse the new album name
    QDomElement docElem = doc.documentElement();
    QString photoUri("");
    QString photoId("");
    QString title("");
    QDomElement keywordElem;
    if (docElem.nodeName() == "entry")
    {
        success = true;
        QDomNode node = docElem.firstChild(); //this should mean <entry>
        while(!node.isNull())
        {
            if (node.isElement())
            {
                QDomElement e = node.toElement();

                if (node.nodeName() == "link" && e.attribute("rel") == "edit")
                {
                    photoUri = e.attribute("href");
                }

                if (node.nodeName() == "gphoto:id")
                {
                    photoId = e.text();
                }

                if(node.nodeName() == "title")
                {
                    success = true;
                    title = e.text();
                }

                if (node.nodeName() == "media:group")
                {
                    keywordElem = node.firstChildElement("media:keywords");
                }

             }
            node = node.nextSibling();
        }
    }

    if (!success)
    {
        emit signalAddPhotoDone(1, i18n("Failed to upload photo"), "");
    }
    else
    {
        // Update the tags information from the tags_map
        QStringList tags = tags_map[title];

        if (tags.count() == 0)
        {
            emit signalAddPhotoDone(0, "", photoId);
        }
        else
        {
            // Add our tags to the keyword element. Then send the whole XML
            // back.
            keywordElem.appendChild(doc.createTextNode(tags.join(", ")));
            addPhotoTag(photoUri, photoId, doc.toString(-1).toUtf8());
        }
    }
}

} // KIPIPicasawebExportPlugin
