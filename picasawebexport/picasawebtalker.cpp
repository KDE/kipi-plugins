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
               : m_albumsList(0), m_parent( parent ),  m_job( 0 )
    {
        m_apikey="49d585bafa0758cb5c58ab67198bf632";
        m_secret="34b39925e6273ffd";

        connect(this, SIGNAL(signalError(const QString&)),
                this, SLOT(slotError(const QString&)));

        authProgressDlg=new QProgressDialog();
    }

PicasawebTalker::~PicasawebTalker()
{
    if (m_job)
        m_job->kill();
    delete m_albumsList;
}

QString PicasawebTalker::getApiSig(QString secret, QStringList headers)
{
    QStringList compressed ;//= new List<string>(headers.Length);

    for ( QStringList::ConstIterator it = headers.constBegin(); it != headers.constEnd(); ++it )
    {
	QString str = (*it);
        QStringList strList = str.split("=");
        compressed.append(strList[0].trimmed()+strList[1].trimmed());
    }

    compressed.sort();
    QString merged=compressed.join("");
    QString final = secret + merged;
    const char *text = final.toAscii();
    KMD5 context (text);
    return context.hexDigest().data();
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
    // username_edit = username;
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
    authProgressDlg->setLabelText(i18n("Getting the token"));

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

    QString url = "http://picasaweb.google.com/data/feed/api/user/" + m_username + "?kind=album";
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
    authProgressDlg->setLabelText(i18n("Checking if previous token is still valid"));
    authProgressDlg->setMaximum(4);
    authProgressDlg->setValue(1);
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
void PicasawebTalker::listAllAlbums() 
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    QString url           = "http://picasaweb.google.com/data/feed/api/user/" + m_username + "?kind=album";
    QString auth_string   = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_LISTALBUMS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void PicasawebTalker::getPhotoProperty(const QString& method,const QString& argList)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QString     url="http://www.picasaweb.com/services/rest/?";
    QStringList headers;
    headers.append("api_key="+ m_apikey);
    headers.append("method="+method);
    headers.append("frob="+ m_frob);
    headers.append(argList);
    QString md5=getApiSig(m_secret,headers);
    headers.append("api_sig="+ md5);
    QString queryStr=headers.join("&");
    QString postUrl = url+queryStr;
    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(postUrl, tmp, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_GETPHOTOPROPERTY;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
    //authProgressDlg->setLabelText("Getting the Token from the server");
    //authProgressDlg->setProgress(3,4);
}

void PicasawebTalker::addPhotoTag(const QString& photoURI, const QString& tag)
{
    //if (m_job && m_state != FE_ADDTAG)
    //{ // we shouldn't kill the old tag request
    //	m_job->kill();
    //	m_job = 0;
    //}

    QString addTagXML = QString("<entry xmlns='http://www.w3.org/2005/Atom'> "
                                "<title>%1</title> "
                                "<category scheme='http://schemas.google.com/g/2005#kind' "
                                "term='http://schemas.google.com/photos/2007#tag'/> "
                                "</entry>").arg(tag);
    QString postUrl = QString("%1").arg(photoURI);
    QByteArray buffer;
    buffer.append(addTagXML.toUtf8());

    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::http_post(postUrl, buffer, KIO::HideProgressInfo);
    job->ui()->setWindow(m_parent);
    job->addMetaData("content-type", "Content-Type: application/atom+xml");
    job->addMetaData("content-length", QString("Content-Length: %1").arg(addTagXML.length()));
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );

    //connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
    //        this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_ADDTAG;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

void PicasawebTalker::listPhotos(const QString& /*albumName*/)
{
    // TODO
}

void PicasawebTalker::createAlbum(const QString& albumTitle, const QString& albumDesc,
                                  const QString& location, long long timestamp, const QString& access,
                                  const QString& media_keywords, bool isCommentsEnabled)
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
                        "</entry> ").arg(albumTitle)
                                    .arg(albumDesc)
                                    .arg(location)
                                    .arg(access)
                                    .arg(isCommentsEnabled==true?"true":"false")
                                    .arg(timestamp)
                                    .arg(media_keywords);

    QByteArray buffer;
    buffer.append(newAlbumXML.toUtf8());

    MPForm form;
    QString postUrl = "http://www.picasaweb.google.com/data/feed/api/user/" + m_username ;
    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::http_post(postUrl, buffer, KIO::HideProgressInfo);
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

bool PicasawebTalker::addPhoto(const QString& photoPath, FPhotoInfo& info,
                               const QString& albumId, bool rescale,
                               int maxDim, int imageQuality)
{
    // Disabling this totally may be checking the m_state and doing selecting
    // disabling is a better idea
    /*if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }*/

    QString album_id = albumId;

    if (album_id.length() == 0)
        album_id = "test";

    QString     postUrl     = "http://www.picasaweb.google.com/data/feed/api/user/" + 
                              QUrl::toPercentEncoding(m_username) + "/albumid/" + album_id;
    QString     auth_string = "GoogleLogin auth=" + m_token;
    QStringList headers;
    MPForm      form;

    //form.addPair("Authorization", auth_string);

    //Create the Body in atom-xml
    QStringList body_xml;
    body_xml.append("<entry xmlns=\'http://www.w3.org/2005/Atom\'>");
    body_xml.append("<title>"+ info.title +"</title>");
    body_xml.append("<summary>"+ info.description +"</summary>");
    body_xml.append("<category scheme=\"http://schemas.google.com/g/2005#kind\" "
                    "term=\"http://schemas.google.com/photos/2007#photo\" />");
    body_xml.append("</entry>");

    QString body = body_xml.join("");

    form.addPair("test", body, "application/atom+xml");

    // save the tags for this photo in to the tags hashmap
    tags_map.insert(info.title, info.tags);

    // Check if RAW file.
#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    QFileInfo fileInfo(photoPath);
    bool isRAW = rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper());

    if (isRAW || rescale)
    {
        // use temporary file for upload
        QImage image;
        if (isRAW)
            KDcrawIface::KDcraw::loadDcrawPreview(image, photoPath);
        else
            image.load(photoPath);

        QString tmpPath = KStandardDirs::locateLocal("tmp", QFileInfo(photoPath).baseName().trimmed() + ".jpg");

        if (rescale && (image.width() > maxDim || image.height() > maxDim))
            image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        image.save(tmpPath, "JPEG", imageQuality);

        // Restore all metadata.
        KExiv2Iface::KExiv2 exiv2Iface;

        if (exiv2Iface.load(photoPath))
        {
            exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            exiv2Iface.setImageDimensions(image.size());
            exiv2Iface.save(tmpPath);
        }
        else
        {
            kWarning() << "Image " << photoPath << " has no exif data";
        }

        kDebug() << "Resizing and saving to temp file: " << tmpPath;

        if (!form.addFile("photo", tmpPath))
        {
            QFile::remove(tmpPath);
            return false;
        }

        QFile::remove(tmpPath);
    }
    else
    {
        // use original file
        if (!form.addFile("photo", photoPath))
            return false;
    }

    form.finish();

    KIO::TransferJob* job = KIO::http_post(postUrl, form.formData(), KIO::HideProgressInfo);
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

QString PicasawebTalker::getUserName()
{
    return m_username;
}

QString PicasawebTalker::getUserId()
{
    return m_userId;
}

void PicasawebTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    if (authProgressDlg && !authProgressDlg->isHidden())
        authProgressDlg->hide();
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

void PicasawebTalker::slotCheckTokenSuccessful() 
{
    if (authProgressDlg && !authProgressDlg->isHidden()) 
    {
        authProgressDlg->setValue(4);
        authProgressDlg->hide();
    }
    emit signalTokenObtained(m_token); 
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
            emit signalAddPhotoFailed(job->errorString());
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
        case(FE_GETFROB):
            break;
        case(FE_GETTOKEN):
            parseResponseGetToken(m_buffer);
            break;
        case(FE_CHECKTOKEN):
            parseResponseCheckToken(m_buffer);
            break;
        case(FE_GETAUTHORIZED):
            break;
        case(FE_LISTPHOTOS):
            parseResponseListPhotos(m_buffer);
            break;
        case(FE_GETPHOTOPROPERTY):
            parseResponsePhotoProperty(m_buffer);
            break;
        case(FE_ADDPHOTO):
            parseResponseAddPhoto(m_buffer);
            break;
        case(FE_ADDTAG):
            parseResponseAddTag(m_buffer);
            break;
    }
}

void PicasawebTalker::parseResponseCheckToken(const QByteArray &data)
{
    bool success = true;
    QString errorString;
    QString username;
    QString transReturn(data);
    // TODO(vardhman): Fix this with proper error handling.
    if(!success)
        getToken(m_username, m_password);
     // kDebug() << "Return code is " << transReturn ;
     emit slotCheckTokenSuccessful();
    // emit signalError(errorString);
}

void PicasawebTalker::parseResponseGetToken(const QByteArray &data)
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
        authProgressDlg->hide();
        emit signalTokenObtained(m_token);
    }
    else
    {
        emit signalError(errorString);
    }

    emit signalBusy(false);
}

void PicasawebTalker::getHTMLResponseCode(const QString& /*str*/)
{
}

void PicasawebTalker::parseResponseListAlbums(const QByteArray &data)
{
    bool success = false;
    QString str(data);
    QDomDocument doc( "feed" );
    if ( !doc.setContent( data ) )
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
    QDomElement e;
    QString feed_id, feed_updated, feed_title, feed_subtitle;
    QString feed_icon_url, feed_link_url;
    QString feed_username, feed_user_uri;

    QString album_id, album_title, album_description;
    m_albumsList = new QLinkedList <PicasaWebAlbum>();

    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "entry")
        {
            success = true;
            e = node.toElement();
            QDomNode details=e.firstChild();
            PicasaWebAlbum fps;
            QDomNode detailsNode = details;

            while(!detailsNode.isNull())
            {
                if(detailsNode.isElement())
                {
                    if(detailsNode.nodeName() == "id")
                    {
                        // The node data is a URL of which album id is the string following the last /
                        // like <id>http://www.picasaweb.google.com/.../AlbumID<id>
                        QString albumIdUrl = detailsNode.toElement().text();
                        int index = albumIdUrl.lastIndexOf("/");
                        int length = albumIdUrl.length();
                        QString album_id = albumIdUrl.right(length - index - 1);
                        fps.id = album_id;
                    }

                    if(detailsNode.nodeName() == "title")
                    {
                        album_title = detailsNode.toElement().text();

                        //this is what is obtained from data.
                        fps.title = album_title;
                    }

                    if(detailsNode.nodeName()=="gphoto:name")
                    {
                        QString name = detailsNode.toElement().text();
                    }
                }

                detailsNode = detailsNode.nextSibling();
            }

            m_albumsList->append(fps);
        }

    node = node.nextSibling();
    }

    if (!success)
    {
        emit signalGetAlbumsListFailed(i18n("Failed to fetch photo-set list"));
        m_albumsList = NULL;
    }
    else
    {
        emit signalGetAlbumsListSucceeded();
    }
}

void PicasawebTalker::parseResponseListPhotos(const QByteArray &data)
{
    QDomDocument doc( "getPhotosList" );
    if ( !doc.setContent( data ) )
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
    //QDomElement e;
    // TODO
}

void PicasawebTalker::parseResponseCreateAlbum(const QByteArray &data)
{
    bool success = false;
    QString errorString;
    QString response(data);
    QDomDocument doc( "AddPhoto Response" );
    // parse the new album name
    QDomElement docElem = doc.documentElement();
    QString title, photo_id, album_id, photoURI;
    QDomNode node = docElem.firstChild(); //this should mean <entry>
    QDomElement e;

    while( !node.isNull() )
    {
        if ( node.isElement())
        {
            QString node_name = node.nodeName();
            QString node_value = node.toElement().text();
            if(node_name == "title")
            {
                success = true;
                title = node_value;
            }
            else if (node_name == "id")
                photoURI = node_value;
            else if(node_name == "gphoto:id")
                photo_id = node_value;
            else if(node_name == "gphoto:albumid")
                album_id = node_value;
        }

        node = node.nextSibling();
    }

    // Raise a popup informing success
}

void PicasawebTalker::parseResponseAddTag(const QByteArray &data)
{
    QString str(data);
    remaining_tags_count -= 1;
    emit signalBusy( false );
    m_buffer.resize(0);

    if (remaining_tags_count == 0)
        emit signalAddPhotoSucceeded();
}

void PicasawebTalker::parseResponseAddPhoto(const QByteArray &data)
{
    bool success = false;
    QString     line;
    QString str(data);
    success = 1;
    QDomDocument doc( "AddPhoto Response" );

    if ( !doc.setContent( data ) )
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QString title, photo_id, album_id, photoURI;
    QDomNode node = docElem.firstChild(); //this should mean <entry>
    QDomElement e;

    while( !node.isNull() )
    {
        if ( node.isElement())
        {
            QString node_name = node.nodeName();
            QString node_value = node.toElement().text();
            if(node_name == "title")
            {
                success = true;
                title = node_value;
            }
            else if (node_name == "id")
                photoURI = node_value;
            else if(node_name == "gphoto:id")
                photo_id = node_value;
            else if(node_name == "gphoto:albumid")
                album_id = node_value;
        }

        node = node.nextSibling();
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        // Update the tags information from the tags_map
        QStringList tags     = tags_map[title];
        remaining_tags_count = tags.count();

        if (tags.count() == 0)
            emit signalAddPhotoSucceeded();

        for ( QStringList::ConstIterator it = tags.constBegin(); it != tags.constEnd(); ++it )
        {
            QString photoURI= QString("http://picasaweb.google.com/data/feed/api/user/"
                    "%1/albumid/%2/photoid/%3").arg(m_username).arg(album_id).arg(photo_id);
            addPhotoTag( photoURI, *it);
        }
    }
}

void PicasawebTalker::parseResponsePhotoProperty(const QByteArray &data)
{
    bool         success = false;
    QString      line;
    QDomDocument doc( "Photos Properties" );

    if ( !doc.setContent( data ) )
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;

    while( !node.isNull() )
    {
        if ( node.isElement() && node.nodeName() == "photoid" )
        {
            e = node.toElement(); // try to convert the node to an element.
            QDomNode details=e.firstChild();
            success=true;
        }

        if ( node.isElement() && node.nodeName() == "err" )
        {
            kDebug(AREA_CODE_LOADING) << "Checking Error in response" ;
            QString code = node.toElement().attribute("code");
            kDebug(AREA_CODE_LOADING) << "Error code=" << code ;
            kDebug(AREA_CODE_LOADING) << "Msg=" << node.toElement().attribute("msg") ;
            emit signalError(code);
        }
        node = node.nextSibling();
    }

    kDebug(AREA_CODE_LOADING) << "GetToken finished" ;
    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to query photo information"));
    }
    else
    {
        //emit signalAddPhotoSucceeded();
    }
}

} // KIPIPicasawebExportPlugin
