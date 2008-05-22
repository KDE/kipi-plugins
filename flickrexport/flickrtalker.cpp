/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#include <qcstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qimage.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qdom.h>
#include <qmap.h>
#include <qfileinfo.h>
#include <qprogressdialog.h>

// KDE includes.

#include <klocale.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kmdcodec.h>
#include <kapp.h>
#include <kmessagebox.h>

// LibKExiv2 includes.

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.

#include <libkdcraw/dcrawbinary.h>
#include <libkdcraw/kdcraw.h>

// Local includes.

#include "pluginsversion.h"
#include "mpform.h"
#include "flickritem.h"
#include "flickrwindow.h"
#include "flickrtalker.h"
#include "flickrtalker.moc"

namespace KIPIFlickrExportPlugin
{

FlickrTalker::FlickrTalker(QWidget* parent)
{
    m_parent = parent;
    m_job    = 0;
    m_apikey = "49d585bafa0758cb5c58ab67198bf632";
    m_secret = "34b39925e6273ffd";

    connect(this, SIGNAL(signalAuthenticate()),
            this, SLOT(slotAuthenticate()));
}

FlickrTalker::~FlickrTalker()
{
    if (m_job)
        m_job->kill();
}

/** Compute MD5 signature using url queries keys and values following Flickr notice:
    http://www.flickr.com/services/api/auth.spec.html
*/
QString FlickrTalker::getApiSig(const QString& secret, const KURL& url)
{
    QMap<QString, QString> queries = url.queryItems();
    QString compressed(secret);

    // NOTE: iterator QMap iterator will sort alphabetically items based on key values.
    for (QMap<QString, QString>::iterator it = queries.begin() ; it != queries.end(); ++it)
    {
        compressed.append(it.key());
        compressed.append(it.data());
    }

    KMD5 context(compressed.utf8());
    return context.hexDigest().data();
}

/**get the Api sig and send it to the server server should return a frob.
*/
void FlickrTalker::getFrob()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KURL url("http://www.flickr.com/services/rest/");
    url.addQueryItem("method", "flickr.auth.getFrob");
    url.addQueryItem("api_key", m_apikey);
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kdDebug() << "Get frob url: " << url << endl;
    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, false);

    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KIO::Job *)),
            this, SLOT(slotResult(KIO::Job *)));

    m_state = FE_GETFROB;
    m_authProgressDlg->setLabelText(i18n("Getting the Frob"));
    m_authProgressDlg->setProgress(1, 4);
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

void FlickrTalker::checkToken(const QString& token) 
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KURL url("http://www.flickr.com/services/rest/");
    url.addQueryItem("method", "flickr.auth.checkToken");
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("auth_token", token);
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kdDebug() << "Check token url: " << url << endl;
    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, false);

    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KIO::Job *)),
            this, SLOT(slotResult(KIO::Job *)));

    m_state = FE_CHECKTOKEN;
    m_authProgressDlg->setLabelText(i18n("Checking if previous token is still valid"));
    m_authProgressDlg->setProgress(1, 4);
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void FlickrTalker::slotAuthenticate()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KURL url("http://www.flickr.com/services/auth/");
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("frob", m_frob);
    url.addQueryItem("perms", "write");
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kdDebug() << "Authenticate url: " << url << endl;

    KApplication::kApplication()->invokeBrowser(url.url());
    int valueOk = KMessageBox::questionYesNo(kapp->activeWindow(),
                  i18n("Please Follow through the instructions in the browser window and "
                       "return back to press ok if you are authenticated or press No"),
                  i18n("Flickr Service Web Authorization"));

    if( valueOk == KMessageBox::Yes)
    {
        getToken();
        m_authProgressDlg->setLabelText(i18n("Authenticating the User on web"));
        m_authProgressDlg->setProgress(2, 4);
        emit signalBusy(false);
    }
    else 
    {
        kdDebug() << "User didn't proceed with getToken Authorization, cannot proceed further, aborting" << endl;
        cancel();
    }
}

void FlickrTalker::getToken()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KURL url("http://www.flickr.com/services/rest/");
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("method", "flickr.auth.getToken");
    url.addQueryItem("frob", m_frob);
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kdDebug() << "Get token url: " << url << endl;
    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, false);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KIO::Job *)),
            this, SLOT(slotResult(KIO::Job *)));

    m_state = FE_GETTOKEN;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    m_authProgressDlg->setLabelText(i18n("Getting the Token from the server"));
    m_authProgressDlg->setProgress(3, 4);
}

void FlickrTalker::listPhotoSets()
{
    KURL url("http://www.flickr.com/services/rest/");
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("method", "flickr.photosets.getList");
    url.addQueryItem("user_id", m_userId);
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kdDebug() << "List photo sets url: " << url << endl;
    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, false);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KIO::Job *)),
            this, SLOT(slotResult(KIO::Job *)));

    m_state = FE_LISTPHOTOSETS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

void FlickrTalker::getPhotoProperty(const QString& method, const QStringList& argList)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KURL url("http://www.flickr.com/services/rest/");
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("method", method);
    url.addQueryItem("frob", m_frob);

    for (QStringList::const_iterator it = argList.begin(); it != argList.end(); ++it)
    {
        QStringList str = QStringList::split("=", (*it));
        url.addQueryItem(str[0], str[1]);
    }

    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kdDebug() << "Get photo property url: " << url << endl;
    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, false);
    job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KIO::Job *)),
            this, SLOT(slotResult(KIO::Job *)));

    m_state = FE_GETPHOTOPROPERTY;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );

//  m_authProgressDlg->setLabelText("Getting the Token from the server");
//  m_authProgressDlg->setProgress(3,4);
}
void FlickrTalker::listPhotos(const QString& /*albumName*/)
{
    // TODO
}

void FlickrTalker::createAlbum(const QString& /*parentAlbumName*/, const QString& /*albumName*/,
                               const QString& /*albumTitle*/, const QString& /*albumCaption*/)
{
    //TODO: The equivalent for Album is sets.
}

bool FlickrTalker::addPhoto(const QString& photoPath, const FPhotoInfo& info,
                            bool rescale, int maxDim, int imageQuality)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KURL    url("http://www.flickr.com/services/upload/");
    QString path = photoPath;
    MPForm  form;

    form.addPair("auth_token", m_token);
    url.addQueryItem("auth_token", m_token);

    form.addPair("api_key", m_apikey);
    url.addQueryItem("api_key", m_apikey);

    QString ispublic = (info.is_public==1) ? "1" : "0";
    form.addPair("is_public", ispublic);
    url.addQueryItem("is_public", ispublic);

    QString isfamily = (info.is_family==1) ? "1" : "0";
    form.addPair("is_family", isfamily);
    url.addQueryItem("is_family", isfamily);

    QString isfriend = (info.is_friend==1) ? "1" : "0";
    form.addPair("is_friend", isfriend);
    url.addQueryItem("is_friend", isfriend);

    QString tags = info.tags.join(" ");
    if(tags.length() > 0)
    {
        form.addPair("tags", tags);
        url.addQueryItem("tags", tags);
    }

    if (!info.title.isEmpty())
    {
        form.addPair("title", info.title);
        url.addQueryItem("title", info.title);
    }

    if (!info.description.isEmpty())
    {
        form.addPair("description", info.description);
        url.addQueryItem("description", info.description);
    }

    QString md5 = getApiSig(m_secret, url);
    form.addPair("api_sig", md5);
    url.addQueryItem("api_sig", md5);
    QImage image;

    // Check if RAW file.
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
    QFileInfo fileInfo(photoPath);
    if (rawFilesExt.upper().contains(fileInfo.extension(false).upper()))
        KDcrawIface::KDcraw::loadDcrawPreview(image, photoPath);
    else
        image.load(photoPath);

    kdDebug() << "Add photo query: " << url << endl;

    if (!image.isNull())
    {
        path = locateLocal("tmp", QFileInfo(photoPath).baseName().stripWhiteSpace() + ".jpg");

        if (rescale && (image.width() > maxDim || image.height() > maxDim))
            image = image.smoothScale(maxDim, maxDim, QImage::ScaleMin);

        image.save(path, "JPEG", imageQuality);

        // Restore all metadata.

        KExiv2Iface::KExiv2 exiv2Iface;

        if (exiv2Iface.load(photoPath))
        {
            exiv2Iface.setImageDimensions(image.size());

            // NOTE: see B.K.O #153207: Flickr use IPTC keywords to create Tags in web interface
            //       As IPTC do not support UTF-8, we need to remove it.
            exiv2Iface.removeIptcTag("Iptc.Application2.Keywords", false);

            exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            exiv2Iface.save(path);
        }
        else
        {
            kdWarning(51000) << "(flickrExport::Image doesn't have metdata)" << endl;
        }

        kdDebug() << "Resizing and saving to temp file: " << path << endl;
    }

    if (!form.addFile("photo", path))
        return false;

    form.finish();

    KIO::TransferJob* job = KIO::http_post(url, form.formData(), false);
    job->addMetaData("content-type", form.contentType());

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KIO::Job *)),
            this, SLOT(slotResult(KIO::Job *)));

    m_state = FE_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    return true;
}

QString FlickrTalker::getUserName()
{
    return m_username;
}

QString FlickrTalker::getUserId()
{
    return m_userId;
}

void FlickrTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    if (m_authProgressDlg && !m_authProgressDlg->isHidden()) 
        m_authProgressDlg->hide();
}

void FlickrTalker::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

void FlickrTalker::slotError(const QString& error)
{
    QString transError;
    int errorNo = atoi(error.latin1());

    switch (errorNo)
    {
        case 2: 
            transError = i18n("No photo specified");
            break;
        case 3: 
            transError = i18n("General upload failure");
            break;
        case 4: 
            transError = i18n("Filesize was zero");
            break;
        case 5: 
            transError = i18n("Filetype was not recognised");
            break;
        case 6:
            transError = i18n("User exceeded upload limit");
            break;
        case 96:
            transError = i18n("Invalid signature");
            break;
        case 97:
            transError = i18n("Missing signature"); 
            break;
        case 98:
            transError = i18n("Login Failed / Invalid auth token"); 
            break;
        case 100: 
            transError = i18n("Invalid API Key"); 
            break;
        case 105:
            transError = i18n("Service currently unavailable");
            break;
        case 108:
            transError = i18n("Invalid Frob");
            break;
        case 111: 
            transError = i18n("Format \"xxx\" not found"); 
            break;
        case 112:
            transError = i18n("Method \"xxx\" not found"); 
            break;
        case 114:
            transError = i18n("Invalid SOAP envelope");
            break;
        case 115:
            transError = i18n("Invalid XML-RPC Method Call");
            break;
        case 116:
            transError = i18n("The POST method is now required for all setters"); 
            break;
        default:
            transError = i18n("Unknown error");
            break;
    };

    KMessageBox::error(kapp->activeWindow(),
                 i18n("Error Occured: %1\n We can not proceed further").arg(transError));

//  kdDebug() << "Not handling the error now will see it later" << endl;
}

void FlickrTalker::slotResult(KIO::Job *job)
{
    m_job = 0;
    emit signalBusy(false);

    if (job->error())
    {
        if (m_state == FE_ADDPHOTO)
            emit signalAddPhotoFailed( job->errorString());
        else
            job->showErrorDialog(m_parent);
        return;
    }

    switch(m_state)
    {
        case(FE_LOGIN):
            //parseResponseLogin(m_buffer);
            break;
        case(FE_LISTPHOTOSETS):
            parseResponseListPhotoSets(m_buffer);
            break;
        case(FE_GETFROB):
            parseResponseGetFrob(m_buffer);
            break;
        case(FE_GETTOKEN):
            parseResponseGetToken(m_buffer);
            break;
        case(FE_CHECKTOKEN):
            parseResponseCheckToken(m_buffer);
            break;
        case(FE_GETAUTHORIZED):
            //parseResponseGetToken(m_buffer);
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
    }
    /*
        if (m_state == FE_LOGIN && m_loggedIn)
        {
    //  listAlbums();
    }*/
}

void FlickrTalker::parseResponseGetFrob(const QByteArray& data)
{
    bool success = false;
    QString errorString;
    QDomDocument doc("mydocument");

    if (!doc.setContent(data)) 
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "frob")
        {
            QDomElement e = node.toElement();    // try to convert the node to an element.
            kdDebug() << "Frob is" << e.text() << endl; 
            m_frob        = e.text();            // this is what is obtained from data.
            success       = true;
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kdDebug() << "Checking Error in response" << endl;
            errorString = node.toElement().attribute("code");
            kdDebug() << "Error code=" << errorString << endl;
            kdDebug() << "Msg=" << node.toElement().attribute("msg") << endl;	
        }
        node = node.nextSibling();
    }

    kdDebug() << "GetFrob finished" << endl;
    m_authProgressDlg->setProgress(2, 4);
    m_state = FE_GETAUTHORIZED;
    if(success)
        emit signalAuthenticate();
    else
        emit signalError(errorString);
}

void FlickrTalker::parseResponseCheckToken(const QByteArray& data)
{
    bool         success = false;
    QString      errorString;
    QString      username;
    QString      transReturn;
    QDomDocument doc("checktoken");

    if (!doc.setContent(data)) 
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;

    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "auth")
        {
            e                = node.toElement(); // try to convert the node to an element.
            QDomNode details = e.firstChild();

            while(!details.isNull())
            {
                if(details.isElement())
                {
                    e = details.toElement();

                    if(details.nodeName() == "token")
                    {
                        kdDebug() << "Token=" << e.text() << endl; 
                        m_token = e.text();//this is what is obtained from data.
                    }

                    if(details.nodeName() == "perms")
                    {
                        kdDebug() << "Perms=" << e.text() << endl; 
                        QString	perms = e.text();//this is what is obtained from data.

                        if(perms == "write")
                            transReturn = i18n("As in the persmission to", "write");
                        else if(perms == "read")
                            transReturn = i18n("As in the permission to", "read");
                        else if(perms == "delete")
                            transReturn = i18n("As in the permission to", "delete");
                    }

                    if(details.nodeName() == "user")
                    {
                        kdDebug() << "nsid=" << e.attribute("nsid") << endl; 
                        m_userId   = e.attribute("nsid");
                        username   = e.attribute("username");
                        m_username = username;
                        kdDebug() << "username=" << e.attribute("username") << endl; 
                        kdDebug() << "fullname=" << e.attribute("fullname") << endl; 
                    }
                }

                details = details.nextSibling();
            }

            m_authProgressDlg->hide();
            emit signalTokenObtained(m_token);
            success = true;
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kdDebug() << "Checking Error in response" << endl;
            errorString = node.toElement().attribute("code");
            kdDebug() << "Error code=" << errorString << endl;
            kdDebug() << "Msg=" << node.toElement().attribute("msg") << endl;

            int valueOk = KMessageBox::questionYesNo(kapp->activeWindow(),
                                       i18n("Your token is invalid. Would you like to "
                                            "get a new token to proceed ?\n"));
            if(valueOk == KMessageBox::Yes)
            {
                getFrob();
                return;
            }
            else
            {
                m_authProgressDlg->hide(); //will popup the result for the checktoken failure below 
            }

        }

        node = node.nextSibling();
    }

    if(!success)
        emit signalError(errorString);

    kdDebug() << "CheckToken finished" << endl;
}

void FlickrTalker::parseResponseGetToken(const QByteArray& data)
{
    bool success = false;
    QString errorString;
    QDomDocument doc("gettoken");
    if (!doc.setContent( data ))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;

    while(!node.isNull()) 
    {
        if (node.isElement() && node.nodeName() == "auth")
        {
            e                = node.toElement(); // try to convert the node to an element.
            QDomNode details = e.firstChild();

            while(!details.isNull())
            {
                if(details.isElement())
                {
                    e = details.toElement();

                    if(details.nodeName() == "token")
                    {
                        kdDebug() << "Token=" << e.text() << endl;
                        m_token = e.text();      //this is what is obtained from data.
                    }

                    if(details.nodeName() == "perms")
                    {
                        kdDebug() << "Perms=" << e.text() << endl;
                    }

                    if(details.nodeName() == "user")
                    {
                        kdDebug() << "nsid=" << e.attribute("nsid") << endl;
                        kdDebug() << "username=" << e.attribute("username") << endl;
                        kdDebug() << "fullname=" << e.attribute("fullname") << endl;
                        m_username = e.attribute("username");
                        m_userId   = e.attribute("nsid");
                    }
                }

                details = details.nextSibling();
            }

            success = true;
        }
        else if (node.isElement() && node.nodeName() == "err")
        {
            kdDebug() << "Checking Error in response" << endl;
            errorString = node.toElement().attribute("code");
            kdDebug() << "Error code=" << errorString << endl;
            kdDebug() << "Msg=" << node.toElement().attribute("msg") << endl;
            //emit signalError(code);
        }

        node = node.nextSibling();
    }

    kdDebug() << "GetToken finished" << endl;
    //emit signalBusy( false );
    m_authProgressDlg->hide();

    if(success)
        emit signalTokenObtained(m_token);
    else
        emit signalError(errorString);
}
void FlickrTalker::parseResponseListPhotoSets(const QByteArray& data)
{
    bool success = false;
    QDomDocument doc("getListPhotoSets");
    if (!doc.setContent(data)) 
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;
    QString photoSet_id, photoSet_title, photoSet_description;
    QValueList <FPhotoSet> photoSetList;

    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "photosets")
        {
            e                = node.toElement();
            QDomNode details = e.firstChild();
            FPhotoSet fps;
            QDomNode detailsNode = details;

            while(!detailsNode.isNull())
            {
                if(detailsNode.isElement())
                {
                    e = detailsNode.toElement();
                    if(detailsNode.nodeName() == "photoset")
                    {
                        kdDebug() << "id=" << e.attribute("id") << endl;
                        photoSet_id              = e.attribute("id");           //this is what is obtained from data.
                        fps.id                   = photoSet_id;
                        QDomNode photoSetDetails = detailsNode.firstChild();
                        QDomElement e_detail;

                        while(!photoSetDetails.isNull())
                        {
                            e_detail = photoSetDetails.toElement();

                            if(photoSetDetails.nodeName() == "title")
                            {
                                kdDebug() << "Title=" << e_detail.text() << endl;
                                photoSet_title = e_detail.text();
                                fps.title      = photoSet_title;
                            }
                            else if(photoSetDetails.nodeName() == "description")
                            {
                                kdDebug() << "Description =" << e_detail.text() << endl;
                                photoSet_description = e_detail.text();
                                fps.description      = photoSet_description;
                            }

                            photoSetDetails = photoSetDetails.nextSibling();
                        }
                    }
                }

                detailsNode = detailsNode.nextSibling();
            }

            photoSetList.append(fps);
            details = details.nextSibling();
            success = true;
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kdDebug() << "Checking Error in response" << endl;
            QString code = node.toElement().attribute("code");
            kdDebug() << "Error code=" << code << endl;
            kdDebug() << "Msg=" << node.toElement().attribute("msg") << endl;
            emit signalError(code);
        }

        node = node.nextSibling();
    }

    kdDebug() << "GetPhotoList finished" << endl;

    if (!success)
    {
        emit signalListPhotoSetsFailed(i18n("Failed to fetch photoSets List"));
    }
    else
    {
        emit signalListPhotoSetsSucceeded(photoSetList);
    }
}

void FlickrTalker::parseResponseListPhotos(const QByteArray& data)
{
    QDomDocument doc("getPhotosList");
    if (!doc.setContent( data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    //QDomElement e;
    //TODO
}

void FlickrTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    QDomDocument doc("getCreateAlbum");
    if (!doc.setContent(data)) 
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    //QDomElement e;
    //TODO
}

void FlickrTalker::parseResponseAddPhoto(const QByteArray& data)
{
    bool    success = false;
    QString line;
    QDomDocument doc("AddPhoto Response");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;

    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "photoid")
        {
            e                = node.toElement();           // try to convert the node to an element.
            QDomNode details = e.firstChild();
            kdDebug() << "Photoid= " << e.text() << endl; 
            success = true;
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kdDebug() << "Checking Error in response" << endl;
            QString code = node.toElement().attribute("code");
            kdDebug() << "Error code=" << code << endl;
            kdDebug() << "Msg=" << node.toElement().attribute("msg") << endl;
            emit signalError(code);
        }

        node = node.nextSibling();
    }

    kdDebug() << "GetToken finished" << endl;

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }

}
void FlickrTalker::parseResponsePhotoProperty(const QByteArray& data)
{
    bool         success = false;
    QString      line;
    QDomDocument doc("Photos Properties");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;

    while(!node.isNull()) 
    {
        if (node.isElement() && node.nodeName() == "photoid")
        {
            e = node.toElement();                 // try to convert the node to an element.
            QDomNode details = e.firstChild();
            kdDebug() << "Photoid=" << e.text() << endl;
            success = true;
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kdDebug() << "Checking Error in response" << endl;
            QString code = node.toElement().attribute("code");
            kdDebug() << "Error code=" << code << endl;
            kdDebug() << "Msg=" << node.toElement().attribute("msg") << endl;
            emit signalError(code);
        }

        node = node.nextSibling();
    }

    kdDebug() << "GetToken finished" << endl;

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to query photo information"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }
}

} // namespace KIPIFlickrExportPlugin
