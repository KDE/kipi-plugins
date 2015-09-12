/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

// Qt includes

#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QTextDocument>
#include <QFile>
#include <QFileInfo>
#include <QtCore/QCryptographicHash>
#include <QUrlQuery>

// KDE includes

#include <kjobwidgets.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>

// Local includes

#include "kipiplugins_debug.h"
#include "kpversion.h"
#include "mpform.h"
#include "smugitem.h"

namespace KIPISmugPlugin
{

SmugTalker::SmugTalker(QWidget* const parent)
{
    m_parent     = parent;
    m_job        = 0;
    m_state      = SMUG_LOGOUT;
    m_userAgent  = QStringLiteral("KIPI-Plugin-Smug/%1 (lure@kubuntu.org)").arg(kipipluginsVersion());
    m_apiVersion = QStringLiteral("1.2.2");
    m_apiURL     = QStringLiteral("https://api.smugmug.com/services/api/rest/%1/").arg(m_apiVersion);
    m_apiKey     = QStringLiteral("R83lTcD4TvMsIiXqpdrA9OdIJ22uA4Wi");
}

SmugTalker::~SmugTalker()
{
    if (loggedIn())
        logout();

    if (m_job)
        m_job->kill();
}

bool SmugTalker::loggedIn() const
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

    QUrl url(m_apiURL);
    QUrlQuery q;

    if (email.isEmpty()) 
    {
        q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.login.anonymously"));
        q.addQueryItem(QStringLiteral("APIKey"), m_apiKey);
    }
    else
    {
        q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.login.withPassword"));
        q.addQueryItem(QStringLiteral("APIKey"), m_apiKey);
        q.addQueryItem(QStringLiteral("EmailAddress"), email);
        q.addQueryItem(QStringLiteral("Password"), password);
    }
    
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    QUrl url(m_apiURL);
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.logout"));
    q.addQueryItem(QStringLiteral("SessionID"), m_sessionID);
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    QUrl url(m_apiURL);
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.albums.get"));
    q.addQueryItem(QStringLiteral("SessionID"), m_sessionID);
    q.addQueryItem(QStringLiteral("Heavy"), QStringLiteral("1"));
    
    if (!nickName.isEmpty())
        q.addQueryItem(QStringLiteral("NickName"), nickName);

    url.setQuery(q);
    
    KIO::TransferJob* const job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LISTALBUMS;
    m_job   = job;
    m_buffer.resize(0);
}

void SmugTalker::listPhotos(const qint64 albumID,
                            const QString& albumKey,
                            const QString& albumPassword,
                            const QString& sitePassword)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    QUrl url(m_apiURL);
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.images.get"));
    q.addQueryItem(QStringLiteral("SessionID"), m_sessionID);
    q.addQueryItem(QStringLiteral("AlbumID"), QString::number(albumID));
    q.addQueryItem(QStringLiteral("AlbumKey"), albumKey);
    q.addQueryItem(QStringLiteral("Heavy"), QStringLiteral("1"));

    if (!albumPassword.isEmpty())
        q.addQueryItem(QStringLiteral("Password"), albumPassword);

    if (!sitePassword.isEmpty())
        q.addQueryItem(QStringLiteral("SitePassword"), sitePassword);
    
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    QUrl url(m_apiURL);
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.albumtemplates.get"));
    q.addQueryItem(QStringLiteral("SessionID"), m_sessionID);
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    QUrl url(m_apiURL);
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.categories.get"));
    q.addQueryItem(QStringLiteral("SessionID"), m_sessionID);
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_LISTCATEGORIES;
    m_job   = job;
    m_buffer.resize(0);
}

void SmugTalker::listSubCategories(qint64 categoryID)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    QUrl url(m_apiURL);
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.subcategories.get"));
    q.addQueryItem(QStringLiteral("SessionID"), m_sessionID);
    q.addQueryItem(QStringLiteral("CategoryID"), QString::number(categoryID));
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    QUrl url(m_apiURL);
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("method"), QStringLiteral("smugmug.albums.create"));
    q.addQueryItem(QStringLiteral("SessionID"), m_sessionID);
    q.addQueryItem(QStringLiteral("Title"), album.title);
    q.addQueryItem(QStringLiteral("CategoryID"), QString::number(album.categoryID));

    if (album.subCategoryID > 0)
        q.addQueryItem(QStringLiteral("SubCategoryID"), QString::number(album.subCategoryID));

    if (!album.description.isEmpty())
        q.addQueryItem(QStringLiteral("Description"), album.description);

    if (album.tmplID > 0)
    {
        // template will also define privacy settings
        q.addQueryItem(QStringLiteral("AlbumTemplateID"), QString::number(album.tmplID));
    }
    else
    {
        if (!album.password.isEmpty())
            q.addQueryItem(QStringLiteral("Password"), album.password);
        if (!album.passwordHint.isEmpty())
            q.addQueryItem(QStringLiteral("PasswordHint"), album.passwordHint);
        if (album.isPublic)
            q.addQueryItem(QStringLiteral("Public"), QStringLiteral("1"));
        else
            q.addQueryItem(QStringLiteral("Public"), QStringLiteral("0"));
    }
    
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = SMUG_CREATEALBUM;
    m_job   = job;
    m_buffer.resize(0);
}

bool SmugTalker::addPhoto(const QString& imgPath, qint64 albumID,
                          const QString& albumKey,
                          const QString& caption)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    QString imgName = QFileInfo(imgPath).fileName();
    // load temporary image to buffer
    QFile imgFile(imgPath);

    if (!imgFile.open(QIODevice::ReadOnly))
    {
        emit signalBusy(false);
        return false;
    }

    long long imgSize  = imgFile.size();
    QByteArray imgData = imgFile.readAll();
    imgFile.close();

    MPForm form;

    form.addPair(QStringLiteral("ByteCount"), QString::number(imgSize));
    form.addPair(QStringLiteral("MD5Sum"), QString::fromLatin1(
        QCryptographicHash::hash(imgData, QCryptographicHash::Md5).toHex()));
    form.addPair(QStringLiteral("AlbumID"), QString::number(albumID));
    form.addPair(QStringLiteral("AlbumKey"), albumKey);
    form.addPair(QStringLiteral("ResponseType"), QStringLiteral("REST"));

    if (!caption.isEmpty())
        form.addPair(QStringLiteral("Caption"), caption);

    if (!form.addFile(imgName, imgPath))
        return false;

    form.finish();

    QString customHdr;
    QUrl url(QStringLiteral("http://upload.smugmug.com/photos/xmladd.mg"));
    KIO::TransferJob* const job = KIO::http_post(url, form.formData(), KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("content-type"), form.contentType());
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    customHdr += QStringLiteral("X-Smug-SessionID: ") + m_sessionID + QStringLiteral("\r\n");
    customHdr += QStringLiteral("X-Smug-Version: ") + m_apiVersion + QStringLiteral("\r\n");
    job->addMetaData(QStringLiteral("customHTTPHeader"), customHdr);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

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

    KIO::TransferJob* const job = KIO::get(QUrl::fromLocalFile(imgPath), KIO::Reload, KIO::HideProgressInfo);
    QString customHdr;
    customHdr += QStringLiteral("X-Smug-SessionID: ") + m_sessionID + QStringLiteral("\r\n");
    customHdr += QStringLiteral("X-Smug-Version: ") + m_apiVersion + QStringLiteral("\r\n");
    job->addMetaData(QStringLiteral("UserAgent"), m_userAgent);
    job->addMetaData(QStringLiteral("customHTTPHeader"), customHdr);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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
    qCDebug(KIPIPLUGINS_LOG) << "errorToText: " << errCode << ": " << errMsg;

    switch (errCode)
    {
        case 0:
            transError = QString();
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

void SmugTalker::slotResult(KJob* kjob)
{
    m_job               = 0;
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

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
            KJobWidgets::setWindow(job, m_parent);
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

    QDomDocument doc(QStringLiteral("login"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse Login response:" << endl << data;

    QDomElement e = doc.documentElement();

    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        e = node.toElement();

        if (e.tagName() == QStringLiteral("Login"))
        {
            m_user.accountType   = e.attribute(QStringLiteral("AccountType"));
            m_user.fileSizeLimit = e.attribute(QStringLiteral("FileSizeLimit")).toInt();

            for (QDomNode nodeL = e.firstChild(); !nodeL.isNull(); nodeL = nodeL.nextSibling())
            {
                if (!nodeL.isElement())
                    continue;

                e = nodeL.toElement();

                if (e.tagName() == QStringLiteral("Session"))
                {
                    m_sessionID = e.attribute(QStringLiteral("id"));
                }
                else if (e.tagName() == QStringLiteral("User"))
                {
                    m_user.nickName    = e.attribute(QStringLiteral("NickName"));
                    m_user.displayName = e.attribute(QStringLiteral("DisplayName"));
                }
            }
            errCode = 0;
        }
        else if (e.tagName() == QStringLiteral("err"))
        {
            errCode = e.attribute(QStringLiteral("code")).toInt();
            errMsg  = e.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
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

    QDomDocument doc(QStringLiteral("logout"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse Logout response:" << endl << data;

    QDomElement e = doc.documentElement();

    for (QDomNode node = e.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        e = node.toElement();

        if (e.tagName() == QStringLiteral("Logout"))
        {
            errCode = 0;
        }
        else if (e.tagName() == QStringLiteral("err"))
        {
            errCode = e.attribute(QStringLiteral("code")).toInt();
            errMsg  = e.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
        }
    }

    // consider we are logged out in any case
    m_sessionID.clear();
    m_user.clear();

    emit signalBusy(false);
}

void SmugTalker::parseResponseAddPhoto(const QByteArray& data)
{
    // A multi-part put response (which we get now) looks like:
    // <?xml version="1.0" encoding="utf-8"?>
    // <rsp stat="ok">
    //   <method>smugmug.images.upload</method>
    //   <ImageID>884775096</ImageID>
    //   <ImageKey>L7aq5</ImageKey>
    //   <ImageURL>http://froody.smugmug.com/Other/Test/12372176_y7yNq#884775096_L7aq5</ImageURL>
    // </rsp>

    // A simple put response (which we used to get) looks like:
    // <?xml version="1.0" encoding="utf-8"?>
    // <rsp stat="ok">
    //   <method>smugmug.images.upload</method>
    //   <Image id="884790545" Key="seeQa" URL="http://froody.smugmug.com/Other/Test/12372176_y7yNq#884790545_seeQa"/>
    // </rsp>

    // Since all we care about is success or not, we can just check the rsp
    // stat.

    int errCode = -1;
    QString errMsg;
    QDomDocument doc(QStringLiteral("addphoto"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse Add Photo response:" << endl << data;

    QDomElement document = doc.documentElement();

    if (document.tagName() == QStringLiteral("rsp"))
    {
        qCDebug(KIPIPLUGINS_LOG) << "rsp stat: " << document.attribute(QStringLiteral("stat"));
        if (document.attribute(QStringLiteral("stat")) == QStringLiteral("ok"))
        {
            errCode = 0;
        }
        else if (document.attribute(QStringLiteral("stat")) == QStringLiteral("fail"))
        {
            QDomElement error = document.firstChildElement(QStringLiteral("err"));
            errCode = error.attribute(QStringLiteral("code")).toInt();
            errMsg = error.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "error" << errCode << ":" << errMsg << endl;
        }
    }
    else
    {
        errCode = -2;
        errMsg = QStringLiteral("Malformed response from smugmug: ") + document.tagName();
        qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
    }

    emit signalBusy(false);
    emit signalAddPhotoDone(errCode, errorToText(errCode, errMsg));
}

void SmugTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc(QStringLiteral("createalbum"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse Create Album response:" << endl << data;

    int newAlbumID = -1;
    QString newAlbumKey;
    QDomElement e  = doc.documentElement();

    for (QDomNode node = e.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        e = node.toElement();

        if (e.tagName() == QStringLiteral("Album"))
        {
            newAlbumID  = e.attribute(QStringLiteral("id")).toLongLong();
            newAlbumKey = e.attribute(QStringLiteral("Key"));
            qCDebug(KIPIPLUGINS_LOG) << "AlbumID: " << newAlbumID;
            qCDebug(KIPIPLUGINS_LOG) << "Key: " << newAlbumKey;
            errCode = 0;
        }
        else if (e.tagName() == QStringLiteral("err"))
        {
            errCode = e.attribute(QStringLiteral("code")).toInt();
            errMsg  = e.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
        }
    }

    emit signalBusy(false);
    emit signalCreateAlbumDone(errCode, errorToText(errCode, errMsg),
                               newAlbumID, newAlbumKey);
}

void SmugTalker::parseResponseListAlbums(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc(QStringLiteral("albums.get"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse Albums response:" << endl << data;

    QList <SmugAlbum> albumsList;
    QDomElement e = doc.documentElement();

    for (QDomNode node = e.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        e = node.toElement();

        if (e.tagName() == QStringLiteral("Albums"))
        {
            for (QDomNode nodeA = e.firstChild(); !nodeA.isNull(); nodeA = nodeA.nextSibling())
            {
                if (!nodeA.isElement())
                    continue;

                e = nodeA.toElement();

                if (e.tagName() == QStringLiteral("Album"))
                {
                    SmugAlbum album;
                    album.id           = e.attribute(QStringLiteral("id")).toLongLong();
                    album.key          = e.attribute(QStringLiteral("Key"));
                    album.title        = htmlToText(e.attribute(QStringLiteral("Title")));
                    album.description  = htmlToText(e.attribute(QStringLiteral("Description")));
                    album.keywords     = htmlToText(e.attribute(QStringLiteral("Keywords")));
                    album.isPublic     = e.attribute(QStringLiteral("Public")) == QStringLiteral("1");
                    album.password     = htmlToText(e.attribute(QStringLiteral("Password")));
                    album.passwordHint = htmlToText(e.attribute(QStringLiteral("PasswordHint")));
                    album.imageCount   = e.attribute(QStringLiteral("ImageCount")).toInt();

                    for (QDomNode nodeC = e.firstChild(); !nodeC.isNull(); nodeC = node.nextSibling())
                    {
                        if (!nodeC.isElement())
                            continue;

                        e = nodeC.toElement();

                        if (e.tagName() == QStringLiteral("Category"))
                        {
                            album.categoryID = e.attribute(QStringLiteral("id")).toLongLong();
                            album.category = htmlToText(e.attribute(QStringLiteral("Name")));
                        }
                        else if (e.tagName() == QStringLiteral("SubCategory"))
                        {
                            album.subCategoryID = e.attribute(QStringLiteral("id")).toLongLong();
                            album.subCategory = htmlToText(e.attribute(QStringLiteral("Name")));
                        }
                    }
                    albumsList.append(album);
                }
            }

            errCode = 0;
        }
        else if (e.tagName() == QStringLiteral("err"))
        {
            errCode = e.attribute(QStringLiteral("code")).toInt();
            errMsg = e.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;

    qSort(albumsList.begin(), albumsList.end(), SmugAlbum::lessThan);

    emit signalBusy(false);
    emit signalListAlbumsDone(errCode, errorToText(errCode, errMsg), albumsList);
}

void SmugTalker::parseResponseListPhotos(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc(QStringLiteral("images.get"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse Photos response:" << endl << data;

    QList <SmugPhoto> photosList;
    QDomElement e = doc.documentElement();

    for (QDomNode node = e.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        e = node.toElement();
	
	if (e.tagName() == QStringLiteral("Album")) {
	    node = e.firstChild();
	    if (!node.isElement())
		continue;
	    e = node.toElement();
	}

        if (e.tagName() == QStringLiteral("Images"))
        {
            for (QDomNode nodeP = e.firstChild(); !nodeP.isNull(); nodeP = nodeP.nextSibling())
            {
                if (!nodeP.isElement())
                    continue;

                e = nodeP.toElement();

                if (e.tagName() == QStringLiteral("Image"))
                {
                    SmugPhoto photo;
                    photo.id       = e.attribute(QStringLiteral("id")).toLongLong();
                    photo.key      = e.attribute(QStringLiteral("Key"));
                    photo.caption  = htmlToText(e.attribute(QStringLiteral("Caption")));
                    photo.keywords = htmlToText(e.attribute(QStringLiteral("Keywords")));
                    photo.thumbURL = e.attribute(QStringLiteral("ThumbURL"));

                    // try to get largest size available
                    if (e.hasAttribute(QStringLiteral("Video1280URL")))
                        photo.originalURL = e.attribute(QStringLiteral("Video1280URL"));
                    else if (e.hasAttribute(QStringLiteral("Video960URL")))
                        photo.originalURL = e.attribute(QStringLiteral("Video960URL"));
                    else if (e.hasAttribute(QStringLiteral("Video640URL")))
                        photo.originalURL = e.attribute(QStringLiteral("Video640URL"));
                    else if (e.hasAttribute(QStringLiteral("Video320URL")))
                        photo.originalURL = e.attribute(QStringLiteral("Video320URL"));
                    else if (e.hasAttribute(QStringLiteral("OriginalURL")))
                        photo.originalURL = e.attribute(QStringLiteral("OriginalURL"));
                    else if (e.hasAttribute(QStringLiteral("X3LargeURL")))
                        photo.originalURL = e.attribute(QStringLiteral("X3LargeURL"));
                    else if (e.hasAttribute(QStringLiteral("X2LargeURL")))
                        photo.originalURL = e.attribute(QStringLiteral("X2LargeURL"));
                    else if (e.hasAttribute(QStringLiteral("XLargeURL")))
                        photo.originalURL = e.attribute(QStringLiteral("XLargeURL"));
                    else if (e.hasAttribute(QStringLiteral("LargeURL")))
                        photo.originalURL = e.attribute(QStringLiteral("LargeURL"));
                    else if (e.hasAttribute(QStringLiteral("MediumURL")))
                        photo.originalURL = e.attribute(QStringLiteral("MediumURL"));
                    else if (e.hasAttribute(QStringLiteral("SmallURL")))
                        photo.originalURL = e.attribute(QStringLiteral("SmallURL"));

                    photosList.append(photo);
                }
            }

            errCode = 0;
        }
        else if (e.tagName() == QStringLiteral("err"))
        {
            errCode = e.attribute(QStringLiteral("code")).toInt();
            errMsg  = e.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;

    emit signalBusy(false);
    emit signalListPhotosDone(errCode, errorToText(errCode, errMsg), photosList);
}

void SmugTalker::parseResponseListAlbumTmpl(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc(QStringLiteral("albumtemplates.get"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse AlbumTemplates response:" << endl << data;

    QList<SmugAlbumTmpl> albumTList;
    QDomElement e = doc.documentElement();

    for (QDomNode node = e.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        e = node.toElement();

        if (e.tagName() == QStringLiteral("AlbumTemplates"))
        {
            for (QDomNode nodeT = e.firstChild(); !nodeT.isNull(); nodeT = nodeT.nextSibling())
            {
                if (!nodeT.isElement())
                    continue;

                QDomElement e = nodeT.toElement();

                if (e.tagName() == QStringLiteral("AlbumTemplate"))
                {
                    SmugAlbumTmpl tmpl;
                    tmpl.id           = e.attribute(QStringLiteral("id")).toLongLong();
                    tmpl.name         = htmlToText(e.attribute(QStringLiteral("AlbumTemplateName")));
                    tmpl.isPublic     = e.attribute(QStringLiteral("Public")) == QStringLiteral("1");
                    tmpl.password     = htmlToText(e.attribute(QStringLiteral("Password")));
                    tmpl.passwordHint = htmlToText(e.attribute(QStringLiteral("PasswordHint")));
                    albumTList.append(tmpl);
                }
            }

            errCode = 0;
        }
        else if (e.tagName() == QStringLiteral("err"))
        {
            errCode = e.attribute(QStringLiteral("code")).toInt();
            errMsg  = e.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;

    emit signalBusy(false);
    emit signalListAlbumTmplDone(errCode, errorToText(errCode, errMsg), albumTList);
}

void SmugTalker::parseResponseListCategories(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc(QStringLiteral("categories.get"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse Categories response:" << endl << data;

    QList <SmugCategory> categoriesList;
    QDomElement e = doc.documentElement();

    for (QDomNode node = e.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        e = node.toElement();

        if (e.tagName() == QStringLiteral("Categories"))
        {
            for (QDomNode nodeC = e.firstChild(); !nodeC.isNull(); nodeC = nodeC.nextSibling())
            {
                if (!nodeC.isElement())
                    continue;

                QDomElement e = nodeC.toElement();

                if (e.tagName() == QStringLiteral("Category"))
                {
                    SmugCategory category;
                    category.id   = e.attribute(QStringLiteral("id")).toLongLong();
                    category.name = htmlToText(e.attribute(QStringLiteral("Name")));
                    categoriesList.append(category);
                }
            }

            errCode = 0;
        }
        else if (e.tagName() == QStringLiteral("err"))
        {
            errCode = e.attribute(QStringLiteral("code")).toInt();
            errMsg  = e.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;

    emit signalBusy(false);
    emit signalListCategoriesDone(errCode, errorToText(errCode, errMsg), categoriesList);
}

void SmugTalker::parseResponseListSubCategories(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc(QStringLiteral("subcategories.get"));

    if (!doc.setContent(data))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Parse SubCategories response:" << endl << data;

    QList <SmugCategory> categoriesList;
    QDomElement e = doc.documentElement();

    for (QDomNode node = e.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        e = node.toElement();

        if (e.tagName() == QStringLiteral("SubCategories"))
        {
            for (QDomNode nodeC = e.firstChild(); !nodeC.isNull(); nodeC = nodeC.nextSibling())
            {
                if (!nodeC.isElement())
                    continue;

                e = nodeC.toElement();

                if (e.tagName() == QStringLiteral("SubCategory"))
                {
                    SmugCategory category;
                    category.id   = e.attribute(QStringLiteral("id")).toLongLong();
                    category.name = htmlToText(e.attribute(QStringLiteral("Name")));
                    categoriesList.append(category);
                }
            }

            errCode = 0;
        }
        else if (e.tagName() == QStringLiteral("err"))
        {
            errCode = e.attribute(QStringLiteral("code")).toInt();
            errMsg  = e.attribute(QStringLiteral("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Error:" << errCode << errMsg;
        }
    }

    if (errCode == 15)  // 15: empty list
        errCode = 0;

    emit signalBusy(false);
    emit signalListSubCategoriesDone(errCode, errorToText(errCode, errMsg), categoriesList);
}


QString SmugTalker::htmlToText(const QString& htmlText)
{
    QTextDocument txtDoc;
    txtDoc.setHtml(htmlText);
    return txtDoc.toPlainText();
}

} // namespace KIPISmugPlugin
