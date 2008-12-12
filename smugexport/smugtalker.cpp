/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to export images to SmugMug web service
 *
 * Copyright (C) 2008 by Luka Renko <lure at kmail dot org>
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

// Qt includes.
#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

// KDE includes.
#include <kcodecs.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>

// Local includes.
#include "pluginsversion.h"
#include "smugitem.h"

namespace KIPISmugExportPlugin
{

SmugTalker::SmugTalker(QWidget* parent)
{
    m_parent = parent;
    m_job    = 0;

    m_userAgent  = QString("KIPI-Plugin-SmugExport/%1 (lure@kubuntu.org)").arg(kipiplugins_version);
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

QString SmugTalker::getEmail()
{
    return m_email;
}

QString SmugTalker::getDisplayName()
{
    return m_displayName;
}

QString SmugTalker::getNickName()
{
    return m_nickName;
}

QString SmugTalker::getAccountType()
{
    return m_accountType;
}

int SmugTalker::getFileSizeLimit()
{
    return m_fileSizeLimit;
}

void SmugTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    if (m_authProgressDlg && !m_authProgressDlg->isHidden())
        m_authProgressDlg->hide();
}

void SmugTalker::login(const QString& email, const QString& password)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    m_authProgressDlg->setLabelText(i18n("Logging to SmugMug service..."));
    m_authProgressDlg->setMaximum(4);
    m_authProgressDlg->setValue(1);

    KUrl url(m_apiURL);
    url.addQueryItem("method", "smugmug.login.withPassword");
    url.addQueryItem("APIKey", m_apiKey);
    url.addQueryItem("EmailAddress", email);
    url.addQueryItem("Password", password);

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
    m_authProgressDlg->setValue(2);

    m_email = email;
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

    //connect(job, SIGNAL(result(KJob*)),
    //        this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LOGOUT;
    m_job   = job;
    m_buffer.resize(0);

    // logout is synchronous call
    job->exec();
    slotResult(job);
}

void SmugTalker::listAlbums()
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
    if (!album.description.isEmpty())
        url.addQueryItem("Description", album.description);
    if (!album.password.isEmpty())
        url.addQueryItem("Password", album.password);
    if (!album.passwordHint.isEmpty())
        url.addQueryItem("PasswordHint", album.passwordHint);
    if (album.isPublic)
        url.addQueryItem("Public", "1");
    else
        url.addQueryItem("Public", "0");

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
    kDebug(51000) << "errorToText: " << errCode << ": " << errMsg;

    switch (errCode)
    {
        case 0:
            transError = "";
            break;
        case 1:
            transError = i18n("Login failed");
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
        if (m_state == SMUG_ADDPHOTO)
        {
            // TODO: should we implement similar for all?
            //emit signalAddPhotoFailed(job->errorString());
        }
        else
        {
            job->ui()->setWindow(m_parent);
            job->ui()->showErrorMessage();
        }
        emit signalBusy(false);
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
    }
}

void SmugTalker::parseResponseLogin(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;

    m_authProgressDlg->setValue(3);
    QDomDocument doc("login");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    kDebug(51000) << "Parse Login response:" << endl << data;

    QDomElement e;
    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "Login")
        {
            e = node.toElement();
            m_accountType = e.attribute("AccountType");
            m_fileSizeLimit = e.attribute("FileSizeLimit").toInt();

            node = e.firstChild();
            while (!node.isNull())
            {
                if (node.isElement())
                {
                    e = node.toElement();

                    if (node.nodeName() == "Session")
                    {
                        m_sessionID = e.attribute("id");
                    }
                    else if (node.nodeName() == "User")
                    {
                        m_nickName = e.attribute("NickName");
                        m_displayName = e.attribute("DisplayName");
                    }
                }

                node = node.nextSibling();
            }

            errCode = 0;
        }
        else if (node.isElement() && node.nodeName() == "err")
        {
            e  = node.toElement();
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug(51000) << "Error:" << errCode << errMsg;
        }

        node = node.nextSibling();
    }

    m_authProgressDlg->setValue(4);
    if (errCode != 0) // if login failed, reset user properties
    {
        m_email.clear();
        m_nickName.clear();
        m_displayName.clear();
        m_sessionID.clear();
        m_accountType.clear();
        m_fileSizeLimit = 0;
    }

    kDebug(51000) << "Login finished";

    emit signalLoginDone(errCode, errorToText(errCode, errMsg));
    emit signalBusy(false);
    m_authProgressDlg->hide();
}

void SmugTalker::parseResponseLogout(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;

    QDomDocument doc("logout");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    kDebug(51000) << "Parse Logout response:" << endl << data;

    QDomElement e;
    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "Logout")
        {
            e = node.toElement();
            errCode = 0;
        }
        else if (node.isElement() && node.nodeName() == "err")
        {
            e  = node.toElement();
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug(51000) << "Error:" << errCode << errMsg;
        }

        node = node.nextSibling();
    }

    // consider we are logged out in any case
    m_email.clear();
    m_nickName.clear();
    m_displayName.clear();
    m_sessionID.clear();
    m_accountType.clear();
    m_fileSizeLimit = 0;

    kDebug(51000) << "Logout finished";

    emit signalBusy(false);
}

void SmugTalker::parseResponseAddPhoto(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("addphoto");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    kDebug(51000) << "Parse Add Photo response:" << endl << data;

    QDomElement e;
    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "Image")
        {
            e = node.toElement();
            kDebug(51000) << "ImageID: " << e.attribute("id");
            kDebug(51000) << "Key: " << e.attribute("Key");
            kDebug(51000) << "URL: " << e.attribute("URL");

            errCode = 0;
        }
        else if (node.isElement() && node.nodeName() == "err")
        {
            e  = node.toElement();
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug(51000) << "Error:" << errCode << errMsg;
        }

        node = node.nextSibling();
    }

    kDebug(51000) << "Add Photo finished";

    emit signalAddPhotoDone(errCode, errorToText(errCode, errMsg));
    emit signalBusy(false);
}

void SmugTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("createalbum");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    kDebug(51000) << "Parse Create Album response:" << endl << data;

    int newAlbumID = -1;
    QDomElement e;
    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "Album")
        {
            e = node.toElement();
            newAlbumID = e.attribute("id").toInt();
            kDebug(51000) << "AlbumID: " << newAlbumID;
            kDebug(51000) << "Key: " << e.attribute("Key");

            errCode = 0;
        }
        else if (node.isElement() && node.nodeName() == "err")
        {
            e  = node.toElement();
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug(51000) << "Error:" << errCode << errMsg;
        }

        node = node.nextSibling();
    }

    kDebug(51000) << "Create Album finished";

    emit signalCreateAlbumDone(errCode, errorToText(errCode, errMsg), 
                               newAlbumID);
    emit signalBusy(false);
}

void SmugTalker::parseResponseListAlbums(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("albums.get");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    kDebug(51000) << "Parse Albums response:" << endl << data;

    QDomElement e;
    QList <SmugAlbum> albumsList;
    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "Albums")
        {
            e = node.toElement();

            QDomNode nodeA = e.firstChild();
            while (!nodeA.isNull())
            {
                if (nodeA.isElement() && nodeA.nodeName() == "Album")
                {
                    SmugAlbum album;
                    e = nodeA.toElement();
                    album.id = e.attribute("id").toInt();
                    album.key = e.attribute("Key");
                    album.title = e.attribute("Title");

                    QDomNode nodeC = e.firstChild();
                    while (!nodeC.isNull())
                    {
                        if (nodeC.isElement() && nodeC.nodeName() == "Category")
                        {
                            e = nodeC.toElement();
                            album.categoryID = e.attribute("id").toInt();
                            album.category = e.attribute("Name");
                        }
                        else if (nodeC.isElement() && nodeC.nodeName() == "SubCategory")
                        {
                            e = nodeC.toElement();
                            album.subCategoryID = e.attribute("id").toInt();
                            album.subCategory = e.attribute("Name");
                        }
                        nodeC = node.nextSibling();
                    }
                    albumsList.append(album);
                }
                nodeA = nodeA.nextSibling();
            }

            errCode = 0;
        }
        else if (node.isElement() && node.nodeName() == "err")
        {
            e  = node.toElement();
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug(51000) << "Error:" << errCode << errMsg;
        }

        node = node.nextSibling();
    }

    kDebug(51000) << "List Albums finished";

    if (errCode == 15)  // 15: empty list
        errCode = 0;
    emit signalListAlbumsDone(errCode, errorToText(errCode, errMsg),
                              albumsList);
    emit signalBusy(false);
}

void SmugTalker::parseResponseListCategories(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("categories.get");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    kDebug(51000) << "Parse Categories response:" << endl << data;

    QDomElement e;
    QList <SmugCategory> categoriesList;
    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "Categories")
        {
            e = node.toElement();

            QDomNode nodeC = e.firstChild();
            while (!nodeC.isNull())
            {
                if (nodeC.isElement() && nodeC.nodeName() == "Category")
                {
                    SmugCategory category;
                    e = nodeC.toElement();
                    category.id = e.attribute("id").toInt();
                    category.name = e.attribute("Title"); // Name in 1.2.1

                    categoriesList.append(category);
                }
                nodeC = nodeC.nextSibling();
            }

            errCode = 0;
        }
        else if (node.isElement() && node.nodeName() == "err")
        {
            e = node.toElement();
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug(51000) << "Error:" << errCode << errMsg;
        }

        node = node.nextSibling();
    }

    kDebug(51000) << "List Categories finished";

    if (errCode == 15)  // 15: empty list
        errCode = 0;
    emit signalListCategoriesDone(errCode, errorToText(errCode, errMsg),
                                  categoriesList);
    emit signalBusy(false);
}

void SmugTalker::parseResponseListSubCategories(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("subcategories.get");
    if (!doc.setContent(data))
        return;

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    kDebug(51000) << "Parse SubCategories response:" << endl << data;

    QDomElement e;
    QList <SmugCategory> categoriesList;
    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "SubCategories")
        {
            e = node.toElement();

            QDomNode nodeC = e.firstChild();
            while (!nodeC.isNull())
            {
                if (nodeC.isElement() && nodeC.nodeName() == "SubCategory")
                {
                    SmugCategory category;
                    e = nodeC.toElement();
                    category.id = e.attribute("id").toInt();
                    category.name = e.attribute("Title"); // Name in 1.2.1

                    categoriesList.append(category);
                }
                nodeC = nodeC.nextSibling();
            }

            errCode = 0;
        }
        else if (node.isElement() && node.nodeName() == "err")
        {
            e = node.toElement();
            errCode = e.attribute("code").toInt();
            errMsg = e.attribute("msg");
            kDebug(51000) << "Error:" << errCode << errMsg;
        }

        node = node.nextSibling();
    }

    kDebug(51000) << "List SubCategories finished";

    if (errCode == 15)  // 15: empty list
        errCode = 0;
    emit signalListSubCategoriesDone(errCode, errorToText(errCode, errMsg),
                                     categoriesList);
    emit signalBusy(false);
}

} // namespace KIPISmugExportPlugin
