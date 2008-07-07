/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C++ includes.

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

// Qt includes.

#include <q3cstring.h>
#include <q3textstream.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qstringlist.h>
#include <q3url.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3ValueList>

// KDE includes.

#include <klocale.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kcodecs.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kio/jobclasses.h>

// LibKExiv2 includes.

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.

#include <libkdcraw/dcrawbinary.h>
#include <libkdcraw/kdcraw.h>

// Local includes.

#include "pluginsversion.h"
#include "mpform.h"
#include "picasawebitem.h"
#include "picasawebtalker.h"
#include "picasawebwindow.h"
#include "picasaweblogin.h"
#include "picasawebtalker.moc"

class PicasawebLogin;

namespace KIPIPicasawebExportPlugin
{

PicasawebTalker::PicasawebTalker( QWidget* parent )
               : m_parent( parent ),  m_job( 0 )
    {
        m_apikey="49d585bafa0758cb5c58ab67198bf632";
        m_secret="34b39925e6273ffd";

        connect(this, SIGNAL(signalError(const QString&)),
                this, SLOT(slotError(const QString&)));

        authProgressDlg=new Q3ProgressDialog();
    }

PicasawebTalker::~PicasawebTalker()
{
    if (m_job)
        m_job->kill();
}

QString PicasawebTalker::getApiSig(QString secret, QStringList headers)
{
    QStringList compressed ;//= new List<string>(headers.Length);

    for ( QStringList::Iterator it = headers.begin(); it != headers.end(); ++it ) {
        QStringList str=QStringList::split("=",(*it));
        compressed.append(str[0].trimmed()+str[1].trimmed());
    }

    compressed.sort();
    QString merged=compressed.join("");
    QString final = secret + merged;
    const char *test=final.ascii();
    KMD5 context (test);
    //kDebug()<< "Test Hex Digest output: " << context.hexDigest().data() << endl;
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

    PicasawebLogin *loginDialog = new PicasawebLogin(kapp->activeWindow(), QString("LoginWindow"), username, password);
    /*if (username!=NULL && username.length() > 0){
        //  kDebug()<<"Showing stored username"<< username << endl;
        loginDialog->setUsername(username);
        if (password != NULL && password.length() > 0){
        //    kDebug()<<"Showing stored password"<< password << endl;
            loginDialog->setPassword(password);
            //  kDebug()<<"Showing stored password"<< password << endl;
        }
    }
    */

    QString username_edit, password_edit;

    if (!loginDialog)
    {
        kDebug()<<" Out of memory error "<< endl;
    }

    if (loginDialog->exec() == QDialog::Accepted)
    {
        username_edit = loginDialog->username();
        password_edit = loginDialog->password();
    }
    else
    {
        //Return something which say authentication needed.
        return ;
    }

    m_username    = username_edit;
    username_edit = username;
    QString accountType = "GOOGLE";

    if (!(username_edit.endsWith("@gmail.com")))
        username_edit += "@gmail.com";

    QByteArray buffer;
    QStringList qsl;
    qsl.append("Email="+username_edit);
    qsl.append("Passwd="+password_edit);
    qsl.append("accountType="+accountType);
    qsl.append("service=lh2");
    qsl.append("source=kipi-picasaweb-client");
    QString dataParameters = qsl.join("&");

    Q3TextStream ts(buffer, QIODevice::Append|QIODevice::WriteOnly);
    ts.setEncoding(Q3TextStream::UnicodeUTF8);
    ts << dataParameters;

    KIO::TransferJob* job = KIO::http_post(url, buffer, KIO::HideProgressInfo);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_state = FE_GETTOKEN;
    authProgressDlg->setLabelText(i18n("Getting the token"));

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void PicasawebTalker::authenticate(const QString& token, const QString& username, const QString& password)
{
    if (!token.isNull() || token.length() < 1)
    {
        checkToken(token);
        m_username = username;
        m_password = password; //this would be needed if the checktoken failed
                                //we would need to reauthenticate using auth
    }
    else
    {
        getToken(username, password);
    }
}

void PicasawebTalker::checkToken(const QString& /*token*/)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QString    url         = "https://www.google.com/accounts/ClientLogin";
    QString    auth_string = "GoogleLogin auth=" + m_token;
    QByteArray tmp;
    KIO::TransferJob* job  = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("customHTTPHeader", "Authorization: " + auth_string);

    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = FE_CHECKTOKEN;
    authProgressDlg->setLabelText(i18n("Checking if previous token is still valid"));
    authProgressDlg->setProgress(1,4);
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
void PicasawebTalker::listAllAlbums() {
    //PORT TO KDE4
#if 0
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QString    url = "http://picasaweb.google.com/data/feed/api/user/" + m_username + "?kind=album";
    QByteArray tmp;
    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::get(url, tmp, false);
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
#endif
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
    QString postUrl=url+queryStr;
    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(postUrl, tmp, KIO::HideProgressInfo);
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
    //if (m_job && m_state != FE_ADDTAG){ //we shouldn't kill the old tag request
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
    Q3TextStream ts(buffer, QIODevice::Append|QIODevice::WriteOnly);
    ts.setEncoding(Q3TextStream::UnicodeUTF8);
    ts << addTagXML;

    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::http_post(postUrl, buffer, KIO::HideProgressInfo);
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
                                  const QString& location, uint  timestamp, const QString& access,
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
    Q3TextStream ts(buffer, QIODevice::Append|QIODevice::WriteOnly);
    ts.setEncoding(Q3TextStream::UnicodeUTF8);
    ts << newAlbumXML;

    MPForm form;
    QString postUrl = "http://www.picasaweb.google.com/data/feed/api/user/" + m_username ;
    QString auth_string = "GoogleLogin auth=" + m_token;
    KIO::TransferJob* job = KIO::http_post(postUrl, buffer, KIO::HideProgressInfo);
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
                               const QString& albumName, bool rescale,
                               int maxDim, int imageQuality)
{
    // Disabling this totally may be checking the m_state and doing selecting
    // disabling is a better idea
    /*if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }*/

    QString album_name = albumName;

    if (album_name.length() == 0)
        album_name = "test";

    QString     postUrl = "http://www.picasaweb.google.com/data/feed/api/user/" + m_username + "/album/" + album_name;
    QString     path    = photoPath;
    QStringList headers;
    MPForm      form;
    QString     auth_string = "GoogleLogin auth=" + m_token;

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
    QImage image;

    // Check if RAW file.
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
    QFileInfo fileInfo(photoPath);
    if (rawFilesExt.toUpper().contains(fileInfo.extension(false).toUpper()))
        KDcrawIface::KDcraw::loadDcrawPreview(image, photoPath);
    else
        image.load(photoPath);

    if (!image.isNull())
    {
        path = KStandardDirs::locateLocal("tmp", QFileInfo(photoPath).baseName().trimmed() + ".jpg");

        if (rescale && (image.width() > maxDim || image.height() > maxDim))
            image = image.smoothScale(maxDim, maxDim, Qt::KeepAspectRatio);

        image.save(path, "JPEG", imageQuality);

        // Restore all metadata.

        KExiv2Iface::KExiv2 exiv2Iface;

        if (exiv2Iface.load(photoPath))
        {
            exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            exiv2Iface.setImageDimensions(image.size());
            exiv2Iface.save(path);
        }
        else
        {
            kWarning(51000) << "(picasawebExport::Image doesn't have exif data)" << endl;
        }

        kDebug() << "Resizing and saving to temp file: " << path << endl;
    }

    if (!form.addFile("photo", path))
        return false;

    form.finish();

    KIO::TransferJob* job = KIO::http_post(postUrl, form.formData(), KIO::HideProgressInfo);
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
            transError=i18n("Filesize was zero");break;
        case 5:
            transError=i18n("Filetype was not recognised");break;
        case 6:
            transError=i18n("User exceeded upload limit");break;
        case 96:
            transError=i18n("Invalid signature"); break;
        case 97:
            transError=i18n("Missing signature"); break;
        case 98:
            transError=i18n("Login Failed / Invalid auth token"); break;
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
            transError=i18n("The POST method is now required for all setters"); break;
        default:
            transError=i18n("Unknown error");
    };

    KMessageBox::error(kapp->activeWindow(), i18n("Error Occured: %1\n We can not proceed further",transError));
    //kDebug()<<"Not handling the error now will see it later"<<endl;
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
            static_cast<KIO::Job*>(job)->showErrorDialog(m_parent);
        }

        return;
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
    bool success = false;
    QString errorString;
    QString username;
    QString transReturn(data);
    // If checktoken failed.
    // getToken ...

    if(transReturn.startsWith("Error="))
        success = false;
    else
        success = true;

    if(!success)
        getToken(m_username, m_password);
    //emit signalError(errorString);
}

void PicasawebTalker::parseResponseGetToken(const QByteArray &data)
{
    bool success = false;
    QString errorString;
    QString str(data);
    //Check the response code should it be 200, proceed
    //if it is 403 handle the error mesg
    //figure out the auth string from this response

    if (str.find("Auth="))
    {
        QStringList strList = QStringList::split("Auth=", str);
        m_token = strList[1];
        success = 1;
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
    m_albumsList = new Q3ValueList <PicasaWebAlbum>();

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
                        album_id = detailsNode.toElement().text();
                        //this is what is obtained from data.
                        fps.id = album_id;
                    }

                    if(detailsNode.nodeName() == "title")
                    {
                        album_title = "Not fetched";

                        if(detailsNode.toElement().attribute("type")=="text")
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
        emit signalGetAlbumsListFailed(i18n("Failed to fetch photoSets List"));
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
        QStringList tags = tags_map[title];
        remaining_tags_count = tags.count();

        if (tags.count() == 0)
            emit signalAddPhotoSucceeded();

        for ( QStringList::Iterator it = tags.begin(); it != tags.end(); ++it )
        {
            QString photoURI= QString("http://picasaweb.google.com/data/feed/api/user/"
                    "%1/albumid/%2/photoid/%3").arg(m_username).arg(album_id).arg(photo_id);
            addPhotoTag( photoURI, *it);
        }
    }
}

void PicasawebTalker::parseResponsePhotoProperty(const QByteArray &data)
{
    bool success = false;
    QString     line;
    QDomDocument doc( "Photos Properties" );

    if ( !doc.setContent( data ) )
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
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
            kDebug()<<"Checking Error in response"<<endl;
            QString code=node.toElement().attribute("code");
            kDebug()<<"Error code="<<code<<endl;
            kDebug()<<"Msg="<<node.toElement().attribute("msg")<<endl;
            emit signalError(code);
        }
        node = node.nextSibling();
    }

    kDebug()<<"GetToken finished"<<endl;
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
