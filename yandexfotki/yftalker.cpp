/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-14
 * Description : Yandex.Fotki web service backend
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
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

#include "yftalker.h"

// Qt includes

#include <QTextDocument>
#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QPointer>
#include <QFile>
#include <QFileInfo>

// KDE includes

#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/jobuidelegate.h>

// Local includes

#include "kipiplugins_debug.h"
#include "kpversion.h"
#include "yandexauth.h"
#include "yfalbum.h"

class YandexFotkiLogin;

namespace KIPIYandexFotkiPlugin
{
/*
 * static API constants
 */
const QString YandexFotkiTalker::SESSION_URL          = QStringLiteral("http://auth.mobile.yandex.ru/yamrsa/key/");
const QString YandexFotkiTalker::AUTH_REALM           = QStringLiteral("fotki.yandex.ru");
const QString YandexFotkiTalker::TOKEN_URL            = QStringLiteral("http://auth.mobile.yandex.ru/yamrsa/token/");
const QString YandexFotkiTalker::SERVICE_URL          = QStringLiteral("http://api-fotki.yandex.ru/api/users/%1/");
const QString YandexFotkiTalker::USERPAGE_URL         = QStringLiteral("http://fotki.yandex.ru/users/%1/");
const QString YandexFotkiTalker::USERPAGE_DEFAULT_URL = QStringLiteral("http://fotki.yandex.ru/");
const QString YandexFotkiTalker::ACCESS_STRINGS[]     = {
    QStringLiteral("public"),
    QStringLiteral("friends"),
    QStringLiteral("private") };

YandexFotkiTalker::YandexFotkiTalker( QObject* const parent )
    : QObject(parent),
      m_state(STATE_UNAUTHENTICATED),
      m_lastPhoto(0),
      m_job(0)
{
}

YandexFotkiTalker::~YandexFotkiTalker()
{
    reset();
}

void YandexFotkiTalker::getService()
{
    m_state                     = STATE_GETSERVICE;
    KIO::TransferJob* const job = KIO::get(QUrl(SERVICE_URL.arg(m_login)),
                                  KIO::NoReload, KIO::HideProgressInfo);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(handleJobData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseGetService(KJob*)));

    m_job = job;
    m_buffer.resize(0);
    m_job->start();
}

/*
void YandexFotkiTalker::checkToken()
{
    // try to get somthing with our token, if it is invalid catch 401
    KIO::TransferJob* const job = KIO::get(m_apiAlbumsUrl,
                                     KIO::NoReload, KIO::HideProgressInfo);
    job->addMetaData("customHTTPHeader",
                     QString("Authorization: FimpToken realm=\"%1\", token=\"%2\"")
                     .arg(AUTH_REALM).arg(m_token));

    m_state = STATE_CHECKTOKEN;

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(handleJobData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseCheckToken(KJob*)));

    m_job = job;
    m_buffer.resize(0);
    m_job->start();
}
*/

void YandexFotkiTalker::getSession()
{
    if (m_state != STATE_GETSERVICE_DONE)
        return;

    KIO::TransferJob* const job = KIO::get(QUrl(SESSION_URL),
                                  KIO::NoReload, KIO::HideProgressInfo);

    //job->ui()->setWindow(m_parent);

    m_state = STATE_GETSESSION;

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(handleJobData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseGetSession(KJob*)));

    m_job = job;
    m_buffer.resize(0);
    m_job->start();
}

void YandexFotkiTalker::getToken()
{
    if (m_state != STATE_GETSESSION_DONE)
        return;

    const QString credentials = YandexAuth::makeCredentials(m_sessionKey,
                                m_login, m_password);

    // prepare params
    QStringList paramList;

    paramList.append(QLatin1String("request_id=") + m_sessionId);

    paramList.append(QLatin1String("credentials=") + QString::fromUtf8(QUrl::toPercentEncoding(credentials)));

    QString params = paramList.join(QStringLiteral("&"));

    KIO::TransferJob* const job = KIO::http_post(QUrl(TOKEN_URL), params.toUtf8(),
                                           KIO::HideProgressInfo);

    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    m_state = STATE_GETTOKEN;

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(handleJobData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseGetToken(KJob*)));

    m_job = job;
    m_buffer.resize(0);
    m_job->start();
}

void YandexFotkiTalker::listAlbums()
{
    if (isErrorState() || !isAuthenticated())
        return;

    m_albumsNextUrl = m_apiAlbumsUrl;
    m_albums.clear();
    listAlbumsNext();
}

void YandexFotkiTalker::listAlbumsNext()
{
    qCDebug(KIPIPLUGINS_LOG) << "listAlbumsNext";

    KIO::TransferJob* const job = KIO::get(QUrl(m_albumsNextUrl),
                                  KIO::NoReload, KIO::HideProgressInfo);

    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/atom+xml; charset=utf-8; type=feed"));
    job->addMetaData(QStringLiteral("customHTTPHeader"),
                     QStringLiteral("Authorization: FimpToken realm=\"%1\", token=\"%2\"")
                     .arg(AUTH_REALM).arg(m_token));

    m_state = STATE_LISTALBUMS;

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(handleJobData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseListAlbums(KJob*)));

    m_job = job;
    m_buffer.resize(0);
    m_job->start();
}

void YandexFotkiTalker::listPhotos(const YandexFotkiAlbum& album)
{
    if (isErrorState() || !isAuthenticated())
        return;

    m_photosNextUrl = album.m_apiPhotosUrl;
    m_photos.clear();
    listPhotosNext();
}

// protected member
void YandexFotkiTalker::listPhotosNext()
{
    qCDebug(KIPIPLUGINS_LOG) << "listPhotosNext";

    KIO::TransferJob* const job = KIO::get(QUrl(m_photosNextUrl),
                                  KIO::NoReload, KIO::HideProgressInfo);

    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/atom+xml; charset=utf-8; type=feed"));
    job->addMetaData(QStringLiteral("customHTTPHeader"),
                     QStringLiteral("Authorization: FimpToken realm=\"%1\", token=\"%2\"")
                     .arg(AUTH_REALM).arg(m_token));

    m_state = STATE_LISTPHOTOS;

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(handleJobData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseListPhotos(KJob*)));

    m_job = job;
    m_buffer.resize(0);
    m_job->start();
}

void YandexFotkiTalker::updatePhoto(YandexFotkiPhoto& photo, const YandexFotkiAlbum& album)
{
    if (isErrorState() || !isAuthenticated())
        return;

    // sanity check
    if (photo.title().isEmpty())
    {
        photo.setTitle(QFileInfo(photo.localUrl()).baseName().trimmed());
    }

    // move photo to another album (if changed)
    photo.m_apiAlbumUrl = album.m_apiSelfUrl;
    // FIXME: hack
    m_lastPhotosUrl = album.m_apiPhotosUrl;

    if (!photo.remoteUrl().isNull())
    {
        // TODO: updating image file haven't yet supported by API
        // so, just update info
        return updatePhotoInfo(photo);
    }
    else
    {
        // for new images also upload file
        updatePhotoFile(photo);
    }
}

void YandexFotkiTalker::updatePhotoFile(YandexFotkiPhoto& photo)
{
    qCDebug(KIPIPLUGINS_LOG) << "updatePhotoFile" << photo;

    QFile imageFile(photo.localUrl());

    if (!imageFile.open(QIODevice::ReadOnly))
    {
        setErrorState(STATE_UPDATEPHOTO_FILE_ERROR);
        return;
    }

    KIO::TransferJob* const job = KIO::http_post(QUrl(m_lastPhotosUrl), imageFile.readAll());
    //job->ui()->setWindow(m_parent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: image/jpeg"));
    job->addMetaData(QStringLiteral("customHTTPHeader"),
                     QStringLiteral("Authorization: FimpToken realm=\"%1\", token=\"%2\"")
                     .arg(AUTH_REALM).arg(m_token));
    job->addMetaData(QLatin1String("slug"), QLatin1String("Slug: ") +
                     QString::fromUtf8(QUrl::toPercentEncoding(photo.title())) + QLatin1String(".jpg"));

    m_state     = STATE_UPDATEPHOTO_FILE;
    m_lastPhoto = &photo;

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(handleJobData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseUpdatePhotoFile(KJob*)));

    m_job = job;
    m_buffer.resize(0);
    m_job->start();
}

void YandexFotkiTalker::updatePhotoInfo(YandexFotkiPhoto& photo)
{
    QDomDocument doc;
    QDomProcessingInstruction instr = doc.createProcessingInstruction(
        QStringLiteral("xml"),
        QStringLiteral("version='1.0' encoding='UTF-8'"));

    doc.appendChild(instr);
    QDomElement entryElem = doc.createElement(QStringLiteral("entry"));
    entryElem.setAttribute(QStringLiteral("xmlns"), QStringLiteral("http://www.w3.org/2005/Atom"));
    entryElem.setAttribute(QStringLiteral("xmlns:f"), QStringLiteral("yandex:fotki"));
    doc.appendChild(entryElem);

    QDomElement urn = doc.createElement(QStringLiteral("urn"));
    urn.appendChild(doc.createTextNode(photo.urn()));
    entryElem.appendChild(urn);

    QDomElement title = doc.createElement(QStringLiteral("title"));
    title.appendChild(doc.createTextNode(photo.title()));
    entryElem.appendChild(title);

    QDomElement linkAlbum = doc.createElement(QStringLiteral("link"));
    linkAlbum.setAttribute(QStringLiteral("href"), photo.m_apiAlbumUrl);
    linkAlbum.setAttribute(QStringLiteral("rel"), QStringLiteral("album"));
    entryElem.appendChild(linkAlbum);

    QDomElement summary = doc.createElement(QStringLiteral("summary"));
    summary.appendChild(doc.createTextNode(photo.summary()));
    entryElem.appendChild(summary);

    QDomElement adult = doc.createElement(QStringLiteral("f:xxx"));
    adult.setAttribute(QStringLiteral("value"), photo.isAdult() ? QStringLiteral("true") : QStringLiteral("false"));
    entryElem.appendChild(adult);

    QDomElement hideOriginal = doc.createElement(QStringLiteral("f:hide_original"));
    hideOriginal.setAttribute(QStringLiteral("value"),
                              photo.isHideOriginal() ? QStringLiteral("true") : QStringLiteral("false"));
    entryElem.appendChild(hideOriginal);

    QDomElement disableComments = doc.createElement(QStringLiteral("f:disable_comments"));
    disableComments.setAttribute(QStringLiteral("value"),
                                 photo.isDisableComments() ? QStringLiteral("true") : QStringLiteral("false"));
    entryElem.appendChild(disableComments);

    QDomElement access = doc.createElement(QStringLiteral("f:access"));
    access.setAttribute(QStringLiteral("value"), ACCESS_STRINGS[photo.access()]);
    entryElem.appendChild(access);

    // FIXME: undocumented API
    foreach(const QString& t, photo.tags)
    {
        QDomElement tag = doc.createElement(QStringLiteral("category"));
        tag.setAttribute(QStringLiteral("scheme"), m_apiTagsUrl);
        tag.setAttribute(QStringLiteral("term"), t);
        entryElem.appendChild(tag);
    }

    m_buffer = doc.toString(1).toUtf8(); // with idents

    qCDebug(KIPIPLUGINS_LOG) << "Prepared data: " << m_buffer;
    m_lastPhoto = &photo;

    m_state = STATE_UPDATEPHOTO_INFO;

    /*
     * KIO::put uses dataReq slot for getting data
     * It's really unsuable, but anyway...
     */
    KIO::TransferJob* const job = KIO::put(QUrl(photo.m_apiEditUrl), -1/*, KIO::HideProgressInfo*/);
    job->addMetaData(QStringLiteral("customHTTPHeader"),
                     QStringLiteral("Authorization: FimpToken realm=\"%1\", token=\"%2\"")
                     .arg(AUTH_REALM).arg(m_token));
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/atom+xml; charset=utf-8; type=entry"));
    job->addMetaData(QStringLiteral("content-length"),
                     QStringLiteral("Content-Length: %1").arg(m_buffer.size()));

    // no result data in this method, but reading from m_buffer
    connect(job, SIGNAL(dataReq(KIO::Job*,QByteArray&)),
            this, SLOT(handleJobReq(KIO::Job*,QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseUpdatePhotoInfo(KJob*)));

    m_job = job;
    m_job->start();
}

void YandexFotkiTalker::updateAlbum(YandexFotkiAlbum& album)
{
    if (isErrorState() || !isAuthenticated())
        return;

    if (album.urn().isEmpty())
    {
        // new album
        return updateAlbumCreate(album);
    }
    else
    {
        qCCritical(KIPIPLUGINS_LOG) << "Updating albums is not yet supported";
    }
}

void YandexFotkiTalker::updateAlbumCreate(YandexFotkiAlbum& album)
{
    QDomDocument doc;
    QDomProcessingInstruction instr = doc.createProcessingInstruction(
        QStringLiteral("xml"),
        QStringLiteral("version='1.0' encoding='UTF-8'"));

    doc.appendChild(instr);
    QDomElement entryElem = doc.createElement(QStringLiteral("entry"));
    entryElem.setAttribute(QStringLiteral("xmlns"), QStringLiteral("http://www.w3.org/2005/Atom"));
    entryElem.setAttribute(QStringLiteral("xmlns:f"), QStringLiteral("yandex:fotki"));
    doc.appendChild(entryElem);

    QDomElement title = doc.createElement(QStringLiteral("title"));
    title.appendChild(doc.createTextNode(album.title()));
    entryElem.appendChild(title);

    QDomElement summary = doc.createElement(QStringLiteral("summary"));
    summary.appendChild(doc.createTextNode(album.summary()));
    entryElem.appendChild(summary);

    QDomElement password = doc.createElement(QStringLiteral("f:password"));
    password.appendChild(doc.createTextNode(album.m_password));
    entryElem.appendChild(password);

    const QByteArray postData = doc.toString(1).toUtf8(); // with idents
    qCDebug(KIPIPLUGINS_LOG) << "Prepared data: " << postData;
    qCDebug(KIPIPLUGINS_LOG) << "Url" << m_apiAlbumsUrl;

    KIO::TransferJob* const job = KIO::http_post(QUrl(m_apiAlbumsUrl), postData,
                                  KIO::HideProgressInfo);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/atom+xml; charset=utf-8; type=entry"));
    job->addMetaData(QStringLiteral("customHTTPHeader"),
                     QStringLiteral("Authorization: FimpToken realm=\"%1\", token=\"%2\"")
                     .arg(AUTH_REALM).arg(m_token));

    m_state = STATE_UPDATEALBUM;

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(handleJobData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(parseResponseUpdateAlbum(KJob*)));

    m_job = job;
    m_buffer.resize(0);
    m_job->start();
}

void YandexFotkiTalker::reset()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    m_token.clear();
    m_state = STATE_UNAUTHENTICATED;
}

void YandexFotkiTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    if (isAuthenticated())
    {
        m_state = STATE_AUTHENTICATED;
    }
    else
    {
        m_token.clear();
        m_state = STATE_UNAUTHENTICATED;
    }
}

void YandexFotkiTalker::setErrorState(State state)
{
    m_state = state;
    emit signalError();
}

bool YandexFotkiTalker::prepareJobResult(KJob* job, State error)
{
    m_job = 0;

    KIO::TransferJob* const transferJob = static_cast<KIO::TransferJob*>(job);

    if (transferJob->error() || transferJob->isErrorPage())
    {
        const QString code = transferJob->queryMetaData(QStringLiteral("responsecode"));
        qCDebug(KIPIPLUGINS_LOG) << "Transfer Error" << code << transferJob->errorString();
        qCDebug(KIPIPLUGINS_LOG) << "Buffer:" << m_buffer;

        if (code == QStringLiteral("401") || code == QStringLiteral("403")) // auth required
        {
            setErrorState(STATE_INVALID_CREDENTIALS);
        }
        else
        {
            setErrorState(error);
        }

        return false;
    }

    return true;
}

void YandexFotkiTalker::handleJobData(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
    {
        return;
    }

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

void YandexFotkiTalker::handleJobReq(KIO::Job*, QByteArray& data)
{
    // is anybody using such ugly KIO API?
    data = m_buffer;
    m_buffer.clear();
}

void YandexFotkiTalker::parseResponseGetService(KJob* job)
{
    m_job = 0;

    KIO::TransferJob* const transferJob = static_cast<KIO::TransferJob*>(job);

    if (transferJob->isErrorPage())
    {
        if (transferJob->queryMetaData(QStringLiteral("responsecode")) == QStringLiteral("404")) // user not found
        {
            return setErrorState(STATE_INVALID_CREDENTIALS);
        }
        else
        {
            return setErrorState(STATE_GETSERVICE_ERROR);
        }
    }

    QDomDocument doc(QStringLiteral("service"));

    if ( !doc.setContent( m_buffer ) )
    {
        qCCritical(KIPIPLUGINS_LOG) << "Invalid XML: parse error" << m_buffer;
        return setErrorState(STATE_GETSERVICE_ERROR);
    }

    const QDomElement rootElem = doc.documentElement();

    QDomElement workspaceElem = rootElem.firstChildElement(QStringLiteral("app:workspace"));

    // FIXME: workaround for Yandex xml namespaces bugs
    QString prefix = QStringLiteral("app:");

    if (workspaceElem.isNull())
    {
        workspaceElem = rootElem.firstChildElement(QStringLiteral("workspace"));
        prefix = QString();
        qCCritical(KIPIPLUGINS_LOG) << "Service document without namespaces found";
    }

    if (workspaceElem.isNull())
    {
        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML data: workspace element";
        return setErrorState(STATE_GETSERVICE_ERROR);
    }

    QString apiAlbumsUrl;
    QString apiPhotosUrl;
    QString apiTagsUrl;

    QDomElement collectionElem = workspaceElem.firstChildElement(prefix + QStringLiteral("collection"));

    for ( ; !collectionElem.isNull();
          collectionElem = collectionElem.nextSiblingElement(prefix + QStringLiteral("collection")))
    {

        const QDomElement acceptElem = collectionElem.firstChildElement(prefix + QStringLiteral("accept"));

        if (acceptElem.isNull()) // invalid section, ignore
        {
            continue;
        }

        // FIXME: id attribute is undocumented
        if (collectionElem.attribute(QStringLiteral("id")) == QStringLiteral("album-list"))
        {
            apiAlbumsUrl = collectionElem.attribute(QStringLiteral("href"));
        }
        else if (collectionElem.attribute(QStringLiteral("id")) == QStringLiteral("photo-list"))
        {
            apiPhotosUrl = collectionElem.attribute(QStringLiteral("href"));
        }
        else if (collectionElem.attribute(QStringLiteral("id")) == QStringLiteral("tag-list"))
        {
            apiTagsUrl = collectionElem.attribute(QStringLiteral("href"));
        } // else skip unknown section
    }

    if (apiAlbumsUrl.isNull() || apiPhotosUrl.isNull())
    {
        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML data: service URLs";
        return setErrorState(STATE_GETSERVICE_ERROR);
    }

    m_apiAlbumsUrl = apiAlbumsUrl;
    m_apiPhotosUrl = apiPhotosUrl;
    m_apiTagsUrl = apiTagsUrl;

    qCDebug(KIPIPLUGINS_LOG) << "ServiceUrls:";
    qCDebug(KIPIPLUGINS_LOG) << "Albums" << m_apiAlbumsUrl;
    qCDebug(KIPIPLUGINS_LOG) << "Photos" << m_apiPhotosUrl;
    qCDebug(KIPIPLUGINS_LOG) << "Tags" << m_apiTagsUrl;

    m_state = STATE_GETSERVICE_DONE;
    emit signalGetServiceDone();
}

/*
void YandexFotkiTalker::parseResponseCheckToken(KJob *job)
{
    m_job = 0;

    qCDebug(KIPIPLUGINS_LOG) << "checkToken" << job->error() << job->errorString() << job->errorText();

    if (job->error())
        return setErrorState(STATE_CHECKTOKEN_INVALID);

    // token still valid, skip getSession and getToken
    m_state = STATE_GETTOKEN_DONE;
    emit signalGetTokenDone();
}
*/

void YandexFotkiTalker::parseResponseGetSession(KJob* job)
{
    if (!prepareJobResult(job, STATE_GETSESSION_ERROR))
        return;

    QDomDocument doc(QStringLiteral("session"));

    if ( !doc.setContent( m_buffer ) )
    {
        return setErrorState(STATE_GETSESSION_ERROR);
    }

    const QDomElement rootElem = doc.documentElement();

    const QDomElement keyElem =  rootElem.firstChildElement(QStringLiteral("key"));

    const QDomElement requestIdElem =  rootElem.firstChildElement(QStringLiteral("request_id"));

    if (keyElem.isNull() || keyElem.nodeType() != QDomNode::ElementNode ||
        requestIdElem.isNull() || requestIdElem.nodeType() != QDomNode::ElementNode)
    {

        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML" << m_buffer;
        return setErrorState(STATE_GETSESSION_ERROR);
    }

    m_sessionKey = keyElem.text();
    m_sessionId  = requestIdElem.text();

    qCDebug(KIPIPLUGINS_LOG) << "Session started" << m_sessionKey << m_sessionId;

    m_state = STATE_GETSESSION_DONE;
    emit signalGetSessionDone();
}

void YandexFotkiTalker::parseResponseGetToken(KJob* job)
{
    if (!prepareJobResult(job, STATE_GETTOKEN_ERROR))
        return;

    QDomDocument doc(QStringLiteral("response"));

    if ( !doc.setContent( m_buffer ) )
    {
        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML: parse error" << m_buffer;
        return setErrorState(STATE_GETTOKEN_ERROR);
    }

    const QDomElement rootElem  = doc.documentElement();
    const QDomElement tokenElem =  rootElem.firstChildElement(QStringLiteral("token"));

    if (tokenElem.isNull() || tokenElem.nodeType() != QDomNode::ElementNode)
    {
        const QDomElement errorElem =  rootElem.firstChildElement(QStringLiteral("error"));

        if (errorElem.isNull() || errorElem.nodeType() != QDomNode::ElementNode)
        {
            qCDebug(KIPIPLUGINS_LOG) << "Auth unknown error";
            return setErrorState(STATE_GETTOKEN_ERROR);
        }

        /*
          // checked by HTTP error code in prepareJobResult
        const QString errorCode = errorElem.attribute("code", "0");
        qCDebug(KIPIPLUGINS_LOG) << QString("Auth error: %1, code=%2").arg(errorElem.text()).arg(errorCode);

        if (errorCode == "2")  { // Invalid credentials
            return setErrorState(STATE_GETTOKEN_INVALID_CREDENTIALS);
        }
        */

        return;
    }

    m_token = tokenElem.text();

    qCDebug(KIPIPLUGINS_LOG) << "Token got" << m_token;
    m_state = STATE_GETTOKEN_DONE;
    emit signalGetTokenDone();
}


void YandexFotkiTalker::parseResponseListAlbums(KJob* job)
{
    if (!prepareJobResult(job, STATE_LISTALBUMS_ERROR))
        return;

    QDomDocument doc(QStringLiteral("feed"));

    if ( !doc.setContent( m_buffer ) )
    {
        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML: parse error";
        return setErrorState(STATE_LISTALBUMS_ERROR);
    }

    bool errorOccurred         = false;
    const QDomElement rootElem = doc.documentElement();

    // find next page link
    m_albumsNextUrl.clear();
    QDomElement linkElem = rootElem.firstChildElement(QStringLiteral("link"));

    for ( ; !linkElem.isNull();
          linkElem = linkElem.nextSiblingElement(QStringLiteral("link")))
    {
        if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("next") &&
            !linkElem.attribute(QStringLiteral("href")).isNull())
        {
            m_albumsNextUrl = linkElem.attribute(QStringLiteral("href"));
            break;
        }
    }

    QDomElement entryElem = rootElem.firstChildElement(QStringLiteral("entry"));

    for ( ; !entryElem.isNull();
          entryElem = entryElem.nextSiblingElement(QStringLiteral("entry")))
    {

        const QDomElement urn       = entryElem.firstChildElement(QStringLiteral("id"));
        const QDomElement author    = entryElem.firstChildElement(QStringLiteral("author"));
        const QDomElement title     = entryElem.firstChildElement(QStringLiteral("title"));
        const QDomElement summary   = entryElem.firstChildElement(QStringLiteral("summary"));
        const QDomElement published = entryElem.firstChildElement(QStringLiteral("published"));
        const QDomElement edited    = entryElem.firstChildElement(QStringLiteral("app:edited"));
        const QDomElement updated   = entryElem.firstChildElement(QStringLiteral("updated"));
        const QDomElement prot      = entryElem.firstChildElement(QStringLiteral("protected"));

        QDomElement linkSelf;
        QDomElement linkEdit;
        QDomElement linkPhotos;

        QDomElement linkElem = entryElem.firstChildElement(QStringLiteral("link"));

        for ( ; !linkElem.isNull();
              linkElem = linkElem.nextSiblingElement(QStringLiteral("link")))
        {

            if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("self"))
                linkSelf = linkElem;
            else if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("edit"))
                linkEdit = linkElem;
            else if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("photos"))
                linkPhotos = linkElem;
            // else skip <link>
        }

        if (urn.isNull() || title.isNull() ||
            linkSelf.isNull() || linkEdit.isNull() || linkPhotos.isNull())
        {
            errorOccurred = true;
            qCDebug(KIPIPLUGINS_LOG) << "Invalid XML data: invalid entry on line" << entryElem.lineNumber();
            // simple skip this record, no addtional messages to user
            continue;
        }

        QString password;

        if (!prot.isNull() && prot.attribute(QStringLiteral("value"), QStringLiteral("false")) == QStringLiteral("true"))
        {
            password = QStringLiteral(""); // set not null value
        }

        m_albums.append(YandexFotkiAlbum(
                            urn.text(),
                            author.text(),
                            title.text(),
                            summary.text(),
                            linkEdit.attribute(QStringLiteral("href")),
                            linkSelf.attribute(QStringLiteral("href")),
                            linkPhotos.attribute(QStringLiteral("href")),
                            QDateTime::fromString(published.text(), QStringLiteral("yyyy-MM-ddTHH:mm:ssZ")),
                            QDateTime::fromString(edited.text(), QStringLiteral("yyyy-MM-ddTHH:mm:ssZ")),
                            QDateTime::fromString(updated.text(), QStringLiteral("yyyy-MM-ddTHH:mm:ssZ")),
                            password
                        ));

        qCDebug(KIPIPLUGINS_LOG) << "Found album:" << m_albums.last();
    }

    // TODO: pagination like listPhotos

    // if an error has occurred and we didn't find anything => notify user
    if (errorOccurred && m_albums.empty())
    {
        qCDebug(KIPIPLUGINS_LOG) << "No result and errors have occurred";
        return setErrorState(STATE_LISTALBUMS_ERROR);
    }

    // we have next page
    if (!m_albumsNextUrl.isNull())
    {
        return listAlbumsNext();
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "List albums done: " << m_albums.size();
        m_state = STATE_LISTALBUMS_DONE;
        emit signalListAlbumsDone(m_albums);
    }
}

bool YandexFotkiTalker::parsePhotoXml(const QDomElement& entryElem, YandexFotkiPhoto& photo)
{

    const QDomElement urn             = entryElem.firstChildElement(QStringLiteral("id"));
    const QDomElement author          = entryElem.firstChildElement(QStringLiteral("author"));
    const QDomElement title           = entryElem.firstChildElement(QStringLiteral("title"));
    const QDomElement summary         = entryElem.firstChildElement(QStringLiteral("summary"));
    const QDomElement published       = entryElem.firstChildElement(QStringLiteral("published"));
    const QDomElement edited          = entryElem.firstChildElement(QStringLiteral("app:edited"));
    const QDomElement updated         = entryElem.firstChildElement(QStringLiteral("updated"));
    const QDomElement created         = entryElem.firstChildElement(QStringLiteral("f:created"));
    const QDomElement accessAttr      = entryElem.firstChildElement(QStringLiteral("f:access"));
    const QDomElement hideOriginal    = entryElem.firstChildElement(QStringLiteral("f:hide_original"));
    const QDomElement disableComments = entryElem.firstChildElement(QStringLiteral("f:disable_comments"));
    const QDomElement adult           = entryElem.firstChildElement(QStringLiteral("f:xxx"));
    const QDomElement content         = entryElem.firstChildElement(QStringLiteral("content"));

    QDomElement linkSelf;
    QDomElement linkEdit;
    QDomElement linkMedia;
    QDomElement linkAlbum;

    QDomElement linkElem = entryElem.firstChildElement(QStringLiteral("link"));

    for ( ; !linkElem.isNull();
          linkElem = linkElem.nextSiblingElement(QStringLiteral("link")))
    {

        if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("self"))
            linkSelf = linkElem;
        else if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("edit"))
            linkEdit = linkElem;
        else if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("edit-media"))
            linkMedia = linkElem;
        else if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("album"))
            linkAlbum = linkElem;
        // else skip <link>
    }

    // XML sanity checks
    if (urn.isNull() || title.isNull() ||
        linkSelf.isNull() || linkEdit.isNull() ||
        linkMedia.isNull() || linkAlbum.isNull() ||
        !content.hasAttribute(QStringLiteral("src")) ||
        !accessAttr.hasAttribute(QStringLiteral("value")))
    {

        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML data, error on line" << entryElem.lineNumber();
        // simple skip this record, no addtional messages to user
        return false;
    }

    const QString accessString = accessAttr.attribute(QStringLiteral("value"));

    YandexFotkiPhoto::Access access;

    if (accessString == ACCESS_STRINGS[YandexFotkiPhoto::ACCESS_PRIVATE])
        access = YandexFotkiPhoto::ACCESS_PRIVATE;
    else if (accessString == ACCESS_STRINGS[YandexFotkiPhoto::ACCESS_FRIENDS])
        access = YandexFotkiPhoto::ACCESS_FRIENDS;
    else if (accessString == ACCESS_STRINGS[YandexFotkiPhoto::ACCESS_PUBLIC])
        access = YandexFotkiPhoto::ACCESS_PUBLIC;
    else
    {
        qCCritical(KIPIPLUGINS_LOG) << "Unknown photo access level: " << accessString;
        access = YandexFotkiPhoto::ACCESS_PUBLIC;
    }

    photo.m_urn    = urn.text();
    photo.m_author = author.text();

    photo.setTitle(title.text());
    photo.setSummary(summary.text());
    photo.m_apiEditUrl    = linkEdit.attribute(QStringLiteral("href"));
    photo.m_apiSelfUrl    = linkSelf.attribute(QStringLiteral("href"));
    photo.m_apiMediaUrl   = linkMedia.attribute(QStringLiteral("href"));
    photo.m_apiAlbumUrl   = linkAlbum.attribute(QStringLiteral("href"));
    photo.m_publishedDate = QDateTime::fromString(published.text(), QStringLiteral("yyyy-MM-ddTHH:mm:ssZ"));
    photo.m_editedDate    = QDateTime::fromString(edited.text(), QStringLiteral("yyyy-MM-ddTHH:mm:ssZ"));
    photo.m_updatedDate   = QDateTime::fromString(updated.text(), QStringLiteral("yyyy-MM-ddTHH:mm:ssZ"));
    photo.m_createdDate   = QDateTime::fromString(created.text(), QStringLiteral("yyyy-MM-ddTHH:mm:ss"));

    photo.setAccess(access);
    photo.setHideOriginal(hideOriginal.attribute(
        QStringLiteral("value"), QStringLiteral("false")) == QStringLiteral("true"));
    photo.setDisableComments(disableComments.attribute(
        QStringLiteral("value"), QStringLiteral("false")) == QStringLiteral("true"));
    photo.setAdult(adult.attribute(
        QStringLiteral("value"), QStringLiteral("false")) == QStringLiteral("true"));

    photo.m_remoteUrl = content.attribute(QStringLiteral("src"));

    /*
     * FIXME: tags part of the API is not documented by Yandex
     */

    // reload all tags from the response
    photo.tags.clear();
    QDomElement category = entryElem.firstChildElement(QStringLiteral("category"));

    for ( ; !category.isNull();
         category = category.nextSiblingElement(QStringLiteral("category")))
    {
        if (category.hasAttribute(QStringLiteral("term")) &&
            category.hasAttribute(QStringLiteral("scheme")) &&
            // FIXME: I have no idea how to make its better, usable API is needed
            category.attribute(QStringLiteral("scheme")) == m_apiTagsUrl)
        {
            photo.tags.append(category.attribute(QStringLiteral("term")));
        }
    }

    return true;
}

void YandexFotkiTalker::parseResponseListPhotos(KJob* job)
{
    if (!prepareJobResult(job, STATE_LISTPHOTOS_ERROR))
        return;

    QDomDocument doc(QStringLiteral("feed"));

    if ( !doc.setContent( m_buffer ) )
    {
        qCCritical(KIPIPLUGINS_LOG) << "Invalid XML, parse error: " << m_buffer;
        return setErrorState(STATE_LISTPHOTOS_ERROR);
    }

    int initialSize    = m_photos.size();
    bool errorOccurred = false;

    const QDomElement rootElem = doc.documentElement();

    // find next page link
    m_photosNextUrl.clear();
    QDomElement linkElem = rootElem.firstChildElement(QStringLiteral("link"));

    for ( ; !linkElem.isNull();
          linkElem = linkElem.nextSiblingElement(QStringLiteral("link")))
    {
        if (linkElem.attribute(QStringLiteral("rel")) == QStringLiteral("next") &&
            !linkElem.attribute(QStringLiteral("href")).isNull())
        {
            m_photosNextUrl = linkElem.attribute(QStringLiteral("href"));
            break;
        }
    }

    QDomElement entryElem = rootElem.firstChildElement(QStringLiteral("entry"));
    for ( ; !entryElem.isNull();
          entryElem = entryElem.nextSiblingElement(QStringLiteral("entry")))
    {

        YandexFotkiPhoto photo;

        if (parsePhotoXml(entryElem, photo))
        {
            m_photos.append(photo);
        }
        else
        {
            // set error mark and conintinue
            errorOccurred = true;
        }
    }

    // if an error has occurred and we didn't find anything => notify user
    if (errorOccurred && initialSize == m_photos.size())
    {
        qCCritical(KIPIPLUGINS_LOG) << "No photos found, some XML errors have occurred";
        return setErrorState(STATE_LISTPHOTOS_ERROR);
    }

    // we have next page
    if (!m_photosNextUrl.isNull())
    {
        return listPhotosNext();
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "List photos done: " << m_photos.size();
        m_state = STATE_LISTPHOTOS_DONE;
        emit signalListPhotosDone(m_photos);
    }
}

void YandexFotkiTalker::parseResponseUpdatePhotoFile(KJob* job)
{
    if (!prepareJobResult(job, STATE_UPDATEPHOTO_FILE_ERROR))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Uploaded photo document" << m_buffer;
    QDomDocument doc(QStringLiteral("entry"));

    if ( !doc.setContent( m_buffer ) )
    {
        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML, parse error" << m_buffer;
        return setErrorState(STATE_UPDATEPHOTO_INFO_ERROR);
    }

    YandexFotkiPhoto& photo = *m_lastPhoto;

    YandexFotkiPhoto tmpPhoto;
    const QDomElement entryElem = doc.documentElement();

    if (!parsePhotoXml(entryElem, tmpPhoto))
    {
        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML, entry not found" << m_buffer;
        return setErrorState(STATE_UPDATEPHOTO_INFO_ERROR);
    }

    photo.m_urn         = tmpPhoto.m_urn;
    photo.m_apiEditUrl  = tmpPhoto.m_apiEditUrl;
    photo.m_apiSelfUrl  = tmpPhoto.m_apiSelfUrl;
    photo.m_apiMediaUrl = tmpPhoto.m_apiMediaUrl;
    photo.m_remoteUrl   = tmpPhoto.m_remoteUrl;
    photo.m_remoteUrl   = tmpPhoto.m_remoteUrl;
    photo.m_author      = tmpPhoto.m_author;

    // update info
    updatePhotoInfo(photo);
}

void YandexFotkiTalker::parseResponseUpdatePhotoInfo(KJob* job)
{
    if (!prepareJobResult(job, STATE_UPDATEPHOTO_INFO_ERROR))
        return;

    YandexFotkiPhoto& photo = *m_lastPhoto;

    /*
    // reload all information
    QDomDocument doc("entry");
    if ( !doc.setContent( m_buffer ) )
    {
        qCDebug(KIPIPLUGINS_LOG) << "Invalid XML: parse error" << m_buffer;
        return setErrorState(STATE_UPDATEPHOTO_INFO_ERROR);
    }

    const QDomElement entryElem = doc.documentElement();
    if(!parsePhotoXml(entryElem, photo))
    {
        qCDebug(KIPIPLUGINS_LOG) << "Can't reload photo after uploading";
        return setErrorState(STATE_UPDATEPHOTO_INFO_ERROR);
    }*/

    m_state     = STATE_UPDATEPHOTO_DONE;
    m_lastPhoto = 0;
    emit signalUpdatePhotoDone(photo);
}

void YandexFotkiTalker::parseResponseUpdateAlbum(KJob* job)
{
    qCDebug(KIPIPLUGINS_LOG) << "!!!";

    if (!prepareJobResult(job, STATE_UPDATEALBUM_ERROR))
        return;

    qCDebug(KIPIPLUGINS_LOG) << "Updated album" << m_buffer;

    m_state     = STATE_UPDATEALBUM_DONE;
    m_lastPhoto = 0;

    emit signalUpdateAlbumDone();
}

} // namespace KIPIYandexFotkiPlugin
