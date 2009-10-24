/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to import/export images to/from 
                 SmugMug web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#include "smugtalker.h"
#include "smugtalker.moc"

// Qt includes

#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QTextDocument>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

// KDE includes

#include <KCodecs>
#include <KDebug>
#include <KIO/Job>
#include <KIO/JobUiDelegate>

// Local includes

#include "pluginsversion.h"
#include "smugitem.h"

namespace KIPISmugPlugin
{

SmugTalker::SmugTalker(QWidget* parent)
{
    m_parent = parent;
    m_job    = 0;

    m_userAgent  = QString("KIPI-Plugin-Smug/%1 (lure@kubuntu.org)").arg(kipiplugins_version);
    m_apiVersion = "1.2.0";
    m_apiURL     = QString("https://api.smugmug.com/hack/rest/%1/").arg(m_apiVersion);
    m_apiKey     = "R83lTcD4TvMsIiXqpdrA9OdIJ22uA4Wi";
}

SmugTalker::~SmugTalker()
{
    if (loggedIn())
        logout();

    if (m_job)
        m_job->kill();
}

bool SmugTalker::loggedIn()
{
    return !m_sessionID.isEmpty();
}

SmugUser SmugTalker::getUser() const
{
    return m_user;
}

void SmugTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(false);
}

void SmugTalker::login(const QString& email, const QString& password)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);
    emit signalLoginProgress(1, 4, i18n("Logging in to SmugMug service..."));

    KUrl url(m_apiURL);
    if (email.isEmpty()) 
    {
        url.addQueryItem("method", "smugmug.login.anonymously");
        url.addQueryItem("APIKey", m_apiKey);
    }
    else
    {
        url.addQueryItem("method", "smugmug.login.withPassword");
        url.addQueryItem("APIKey", m_apiKey);
        url.addQueryItem("EmailAddress", email);
        url.addQueryItem("Password", password);
    }

    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LOGIN;
    m_job   = job;
    m_buffer.resize(0);

    m_user.email = email;
}

void SmugTalker::logout()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KUrl url(m_apiURL);
    url.addQueryItem("method", "smugmug.logout");
    url.addQueryItem("SessionID", m_sessionID);

    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    m_state = SMUG_LOGOUT;
    m_job   = job;
    m_buffer.resize(0);

    // logout is synchronous call
    job->exec();
    slotResult(job);
}

void SmugTalker::listAlbums(const QString& nickName)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KUrl url(m_apiURL);
    url.addQueryItem("method", "smugmug.albums.get");
    url.addQueryItem("SessionID", m_sessionID);
    url.addQueryItem("Heavy", "1");
    if (!nickName.isEmpty())
        url.addQueryItem("NickName", nickName);

    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LISTALBUMS;
    m_job   = job;
    m_buffer.resize(0);
}

void SmugTalker::listPhotos(int albumID, 
                            const QString& albumPassword, 
                            const QString& sitePassword)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KUrl url(m_apiURL);
    url.addQueryItem("method", "smugmug.images.get");
    url.addQueryItem("SessionID", m_sessionID);
    url.addQueryItem("AlbumID", QString::number(albumID));
    url.addQueryItem("Heavy", "1");
    if (!albumPassword.isEmpty())
        url.addQueryItem("Password", albumPassword);
    if (!sitePassword.isEmpty())
        url.addQueryItem("SitePassword", sitePassword);

    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LISTPHOTOS;
    m_job   = job;
    m_buffer.resize(0);
}

void SmugTalker::listAlbumTmpl()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KUrl url(m_apiURL);
    url.addQueryItem("method", "smugmug.albumtemplates.get");
    url.addQueryItem("SessionID", m_sessionID);

    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LISTALBUMTEMPLATES;
    m_job   = job;
    m_buffer.resize(0);
}

void SmugTalker::listCategories()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KUrl url(m_apiURL);
    url.addQueryItem("method", "smugmug.categories.get");
    url.addQueryItem("SessionID", m_sessionID);

    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LISTCATEGORIES;
    m_job   = job;
    m_buffer.resize(0);
}

void SmugTalker::listSubCategories(int categoryID)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KUrl url(m_apiURL);
    url.addQueryItem("method", "smugmug.subcategories.get");
    url.addQueryItem("SessionID", m_sessionID);
    url.addQueryItem("CategoryID", QString::number(categoryID));

    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LISTSUBCATEGORIES;
    m_job   = job;
    m_buffer.resize(0);
}

void SmugTalker::createAlbum(const SmugAlbum& album)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KUrl url(m_apiURL);
    url.addQueryItem("method", "smugmug.albums.create");
    url.addQueryItem("SessionID", m_sessionID);
    url.addQueryItem("Title", album.title);
    url.addQueryItem("CategoryID", QString::number(album.categoryID));
    if (album.subCategoryID > 0)
        url.addQueryItem("SubCategoryID", QString::number(album.subCategoryID));
    if (!album.description.isEmpty())
        url.addQueryItem("Description", album.description);
    if (album.tmplID > 0)
    {
        // template will also define privacy settings
        url.addQueryItem("AlbumTemplateID", QString::number(album.tmplID));
    }
    else
    {
        if (!album.password.isEmpty())
            url.addQueryItem("Password", album.password);
        if (!album.passwordHint.isEmpty())
            url.addQueryItem("PasswordHint", album.passwordHint);
        if (album.isPublic)
            url.addQueryItem("Public", "1");
        else
            url.addQueryItem("Public", "0");
    }

    QByteArray tmp;
    KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_CREATEALBUM;
    m_job   = job;
    m_buffer.resize(0);
}

bool SmugTalker::addPhoto(const QString& imgPath, int albumID)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    // load temporary image to buffer
    QFile imgFile(imgPath);
    if (!imgFile.open(QIODevice::ReadOnly))
    {
        emit signalBusy(false);
        return false;
    }
    long long imgSize = imgFile.size();
    QByteArray imgData = imgFile.readAll();
    imgFile.close();
    KMD5 imgMD5(imgData);

    KUrl url("http://upload.smugmug.com/photos/xmlrawadd.mg");
    KIO::TransferJob* job = KIO::http_post(url, imgData, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-length",
                     QString("Content-Length: %1").arg(QString::number(imgSize)));
    QString customHdr;
    customHdr += "Content-MD5: " + QString(imgMD5.hexDigest()) + "\r\n";
    customHdr += "X-Smug-SessionID: " + m_sessionID + "\r\n";
    customHdr += "X-Smug-Version: " + m_apiVersion + "\r\n";
    customHdr += "X-Smug-ResponseType: REST\r\n";
    customHdr += "X-Smug-AlbumID: " + QString::number(albumID) + "\r\n";
    customHdr += "X-Smug-FileName: " + QFileInfo(imgPath).fileName() + "\r\n";
    job->addMetaData("customHTTPHeader", customHdr);

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_state = SMUG_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    return true;
}

void SmugTalker::getPhoto(const QString& imgPath)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    KIO::TransferJob* job = KIO::get(imgPath, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_GETPHOTO;
    m_job   = job;
    m_buffer.resize(0);
}

void SmugTalker::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

QString SmugTalker::errorToText(int errCode, const QString &errMsg)
{
    QString transError;
    kDebug() << "errorToText: " << errCode << ": " << errMsg;

    switch (errCode)
    {
        case 0:
            transError = "";
            break;
        case 1:
            transError = i18n("Login failed");
            break;
        case 4:
            transError = i18n("Invalid user/nick/password");
            break;
        case 18:
            transError = i18n("Invalid API key");
            break;
        default:
            transError = errMsg;
            break;
    }
    return transError;
}

void SmugTalker::slotResult(KJob *kjob)
{
    m_job = 0;
    KIO::Job *job = static_cast<KIO::Job*>(kjob);

    if (job->error())
    {
        if (m_state == SMUG_LOGIN)
        {
            m_sessionID.clear();
            m_user.clear();

            emit signalBusy(false);
            emit signalLoginDone(job->error(), job->errorText());
        }
        else if (m_state == SMUG_ADDPHOTO)
        {
            emit signalBusy(false);
            emit signalAddPhotoDone(job->error(), job->errorText());
        }
        else if (m_state == SMUG_GETPHOTO)
        {
            emit signalBusy(false);
            emit signalGetPhotoDone(job->error(), job->errorText(), QByteArray());
        }
        else
        {
            emit signalBusy(false);
            job->ui()->setWindow(m_parent);
            job->ui()->showErrorMessage();
        }
        return;
    }

    switch(m_state)
    {
        case(SMUG_LOGIN):
            parseResponseLogin(m_buffer);
            break;
        case(SMUG_LOGOUT):
            parseResponseLogout(m_buffer);
            break;
        case(SMUG_LISTALBUMS):
            parseResponseListAlbums(m_buffer);
            break;
        case(SMUG_LISTPHOTOS):
            parseResponseListPhotos(m_buffer);
            break;
        case(SMUG_LISTALBUMTEMPLATES):
            parseResponseListAlbumTmpl(m_buffer);
            break;
        case(SMUG_LISTCATEGORIES):
            parseResponseListCategories(m_buffer);
            break;
        case(SMUG_LISTSUBCATEGORIES):
            parseResponseListSubCategories(m_buffer);
            break;
        case(SMUG_CREATEALBUM):
            parseResponseCreateAlbum(m_buffer);
            break;
        case(SMUG_ADDPHOTO):
            parseResponseAddPhoto(m_buffer);
            break;
        case(SMUG_GETPHOTO):
            // all we get is data of the image
            emit signalBusy(false);
            emit signalGetPhotoDone(0, QString(), m_buffer);
            break;
    }
}

void SmugTalker::parseResponseLogin(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;

    emit signalLoginProgress(3);
    QDomDocument doc("login");
    if (!doc.setContent(data))
        return;

    kDebug() << "Parse Login response:" << endl << data;

    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "Login")
        {
            m_user.accountType = e.attribute("AccountType");
            m_user.fileSizeLimit = e.attribute("FileSizeLimit").toInt();

            for (QDomNode nodeL = e.firstChild();
                 !nodeL.isNull();
                 nodeL = nodeL.nextSibling())
            {
                if (!nodeL.isElement())
                    continue;
                e = nodeL.toElement();
                if (e.tagName() == "Session")
                {
                    m_sessionID = e.attribute("id");
                }
                else if (e.tagName() == "User")
                {
                    m_user.nickName = e.attribute("NickName");
                    m_user.displayName = e.attribute("DisplayName");
                }
            }
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    emit signalLoginProgress(4);
    if (errCode != 0) // if login failed, reset user properties
    {
        m_sessionID.clear();
        m_user.clear();
    }

    emit signalBusy(false);
    emit signalLoginDone(errCode, errorToText(errCode, errMsg));
}

void SmugTalker::parseResponseLogout(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;

    QDomDocument doc("logout");
    if (!doc.setContent(data))
        return;

    kDebug() << "Parse Logout response:" << endl << data;

    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
        node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "Logout")
        {
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    // consider we are logged out in any case
    m_sessionID.clear();
    m_user.clear();

    emit signalBusy(false);
}

void SmugTalker::parseResponseAddPhoto(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("addphoto");
    if (!doc.setContent(data))
        return;

    kDebug() << "Parse Add Photo response:" << endl << data;

    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "Image")
        {
            kDebug() << "ImageID: " << e.attribute("id");
            kDebug() << "Key: " << e.attribute("Key");
            kDebug() << "URL: " << e.attribute("URL");
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    emit signalBusy(false);
    emit signalAddPhotoDone(errCode, errorToText(errCode, errMsg));
}

void SmugTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("createalbum");
    if (!doc.setContent(data))
        return;


    kDebug() << "Parse Create Album response:" << endl << data;

    int newAlbumID = -1;
    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "Album")
        {
            newAlbumID = e.attribute("id").toInt();
            kDebug() << "AlbumID: " << newAlbumID;
            kDebug() << "Key: " << e.attribute("Key");
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    emit signalBusy(false);
    emit signalCreateAlbumDone(errCode, errorToText(errCode, errMsg),
                               newAlbumID);
}

void SmugTalker::parseResponseListAlbums(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("albums.get");
    if (!doc.setContent(data))
        return;

    kDebug() << "Parse Albums response:" << endl << data;

    QList <SmugAlbum> albumsList;
    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "Albums")
        {
            for (QDomNode nodeA = e.firstChild();
                 !nodeA.isNull();
                 nodeA = nodeA.nextSibling())
            {
                if (!nodeA.isElement())
                    continue;
                e = nodeA.toElement();
                if (e.tagName() == "Album")
                {
                    SmugAlbum album;
                    album.id = e.attribute("id").toInt();
                    album.key = e.attribute("Key");
                    album.title = htmlToText(e.attribute("Title"));
                    album.description = htmlToText(e.attribute("Description"));
                    album.keywords = htmlToText(e.attribute("Keywords"));
                    album.isPublic = e.attribute("Public") == "1";
                    album.password = htmlToText(e.attribute("Password"));
                    album.passwordHint = htmlToText(e.attribute("PasswordHint"));
                    album.imageCount = e.attribute("ImageCount").toInt();

                    for (QDomNode nodeC = e.firstChild();
                         !nodeC.isNull();
                         nodeC = node.nextSibling())
                    {
                        if (!nodeC.isElement())
                            continue;
                        e = nodeC.toElement();
                        if (e.tagName() == "Category")
                        {
                            album.categoryID = e.attribute("id").toInt();
                            album.category = htmlToText(e.attribute("Name"));
                        }
                        else if (e.tagName() == "SubCategory")
                        {
                            album.subCategoryID = e.attribute("id").toInt();
                            album.subCategory = htmlToText(e.attribute("Name"));
                        }
                    }
                    albumsList.append(album);
                }
            }
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;
    emit signalBusy(false);
    emit signalListAlbumsDone(errCode, errorToText(errCode, errMsg),
                              albumsList);
}

void SmugTalker::parseResponseListPhotos(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("images.get");
    if (!doc.setContent(data))
        return;

    kDebug() << "Parse Photos response:" << endl << data;

    QList <SmugPhoto> photosList;
    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "Images")
        {
            for (QDomNode nodeP = e.firstChild();
                 !nodeP.isNull();
                 nodeP = nodeP.nextSibling())
            {
                if (!nodeP.isElement())
                    continue;
                e = nodeP.toElement();
                if (e.tagName() == "Image")
                {
                    SmugPhoto photo;
                    photo.id = e.attribute("id").toInt();
                    photo.key = e.attribute("Key");
                    photo.caption = htmlToText(e.attribute("Caption"));
                    photo.keywords = htmlToText(e.attribute("Keywords"));
                    photo.thumbURL = e.attribute("ThumbURL");
                    // try to get largest size available
                    if (e.hasAttribute("Video1280URL"))
                        photo.originalURL = e.attribute("Video1280URL");
                    else if (e.hasAttribute("Video960URL"))
                        photo.originalURL = e.attribute("Video960URL");
                    else if (e.hasAttribute("Video640URL"))
                        photo.originalURL = e.attribute("Video640URL");
                    else if (e.hasAttribute("Video320URL"))
                        photo.originalURL = e.attribute("Video320URL");
                    else if (e.hasAttribute("OriginalURL"))
                        photo.originalURL = e.attribute("OriginalURL");
                    else if (e.hasAttribute("X3LargeURL"))
                        photo.originalURL = e.attribute("X3LargeURL");
                    else if (e.hasAttribute("X2LargeURL"))
                        photo.originalURL = e.attribute("X2LargeURL");
                    else if (e.hasAttribute("XLargeURL"))
                        photo.originalURL = e.attribute("XLargeURL");
                    else if (e.hasAttribute("LargeURL"))
                        photo.originalURL = e.attribute("LargeURL");
                    else if (e.hasAttribute("MediumURL"))
                        photo.originalURL = e.attribute("MediumURL");
                    else if (e.hasAttribute("SmallURL"))
                        photo.originalURL = e.attribute("SmallURL");
                    photosList.append(photo);
                }
            }
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;
    emit signalBusy(false);
    emit signalListPhotosDone(errCode, errorToText(errCode, errMsg),
                              photosList);
}

void SmugTalker::parseResponseListAlbumTmpl(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("albumtemplates.get");
    if (!doc.setContent(data))
        return;

    kDebug() << "Parse AlbumTemplates response:" << endl << data;

    QList<SmugAlbumTmpl> albumTList;
    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "AlbumTemplates")
        {
            for (QDomNode nodeT = e.firstChild();
                 !nodeT.isNull();
                 nodeT = nodeT.nextSibling())
            {
                if (!nodeT.isElement())
                    continue;
                QDomElement e = nodeT.toElement();
                if (e.tagName() == "AlbumTemplate")
                {
                    SmugAlbumTmpl tmpl;
                    tmpl.id = e.attribute("id").toInt();
                    tmpl.name = htmlToText(e.attribute("AlbumTemplateName"));
                    tmpl.isPublic = e.attribute("Public") == "1";
                    tmpl.password = htmlToText(e.attribute("Password"));
                    tmpl.passwordHint = htmlToText(e.attribute("PasswordHint"));
                    albumTList.append(tmpl);
                }
            }
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;
    emit signalBusy(false);
    emit signalListAlbumTmplDone(errCode, errorToText(errCode, errMsg),
                                      albumTList);
}

void SmugTalker::parseResponseListCategories(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("categories.get");
    if (!doc.setContent(data))
        return;

    kDebug() << "Parse Categories response:" << endl << data;

    QList <SmugCategory> categoriesList;
    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "Categories")
        {
            for (QDomNode nodeC = e.firstChild();
                 !nodeC.isNull();
                 nodeC = nodeC.nextSibling())
            {
                if (!nodeC.isElement())
                    continue;
                QDomElement e = nodeC.toElement();
                if (e.tagName() == "Category")
                {
                    SmugCategory category;
                    category.id = e.attribute("id").toInt();
                    category.name = htmlToText(e.attribute("Title")); // Name in 1.2.1
                    categoriesList.append(category);
                }
            }
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;
    emit signalBusy(false);
    emit signalListCategoriesDone(errCode, errorToText(errCode, errMsg),
                                  categoriesList);
}

void SmugTalker::parseResponseListSubCategories(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("subcategories.get");
    if (!doc.setContent(data))
        return;

    kDebug() << "Parse SubCategories response:" << endl << data;

    QList <SmugCategory> categoriesList;
    QDomElement e = doc.documentElement();
    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;
        e = node.toElement();
        if (e.tagName() == "SubCategories")
        {
            for (QDomNode nodeC = e.firstChild();
                 !nodeC.isNull();
                 nodeC = nodeC.nextSibling())
            {
                if (!nodeC.isElement())
                    continue;
                e = nodeC.toElement();
                if (e.tagName() == "SubCategory")
                {
                    SmugCategory category;
                    category.id = e.attribute("id").toInt();
                    category.name = htmlToText(e.attribute("Title")); // Name in 1.2.1
                    categoriesList.append(category);
                }
            }
            errCode = 0;
        }
        else if (e.tagName() == "err")
        {
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug() << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;
    emit signalBusy(false);
    emit signalListSubCategoriesDone(errCode, errorToText(errCode, errMsg),
                                     categoriesList);
}


QString SmugTalker::htmlToText(const QString& htmlText)
{
    QTextDocument txtDoc;
    txtDoc.setHtml(htmlText);
    return txtDoc.toPlainText();
}

} // namespace KIPISmugPlugin
