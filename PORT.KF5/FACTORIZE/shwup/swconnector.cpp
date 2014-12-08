/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2009 by Timothée Groleau <kde at timotheegroleau dot com>
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

#include "swconnector.moc"

// C++ includes

#include <ctime>

// Qt includes

#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QtAlgorithms>
#include <QDateTime>
#include <QFile>
#include <QUrl>
#include <QRegExp>

// QCA includes

#include <QtCrypto>

// KDE includes

#include <kcodecs.h>
#include <kdebug.h>
#include <kurl.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>

// Local includes

#include "kpversion.h"
#include "switem.h"

namespace KIPIShwupPlugin
{

bool operator< (const SwAlbum& first, const SwAlbum& second)
{
    return first.title < second.title;
}

SwConnector::SwConnector(QWidget* const parent)
{
    m_parent        = parent;
    m_job           = 0;
    m_resultHandler = 0;

    m_userAgent     = QString("KIPI-Plugin-Shwup/%1 (kde@timotheegroleau.com)").arg(kipiplugins_version);
    m_apiVersion    = "1.0";

    m_apiStartURL   = "http://www.shwup.com/rest/v1/server"; // fixed to v1
    m_apiDomainURL  = "";
    m_apiRestPath   = "";

    m_apiKey        = "shwp_kipi";
    m_apiSecretKey  = "2QnKLEgARWiAH3dcdxJAqGVmID+R5I5z8EnXJ1fj";

    setUser(SwUser());
    m_loggedIn      = false;
}

SwConnector::~SwConnector()
{
    // do not logout - may reuse session for next upload

    if (m_job)
        m_job->kill();
}

SwUser SwConnector::getUser() const
{
    return m_user;
}

void SwConnector::setUser(const SwUser& user)
{
    m_user = user;
}

bool SwConnector::isLoggedIn() const
{
    return m_loggedIn;
}

void SwConnector::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(false);
}

void SwConnector::logout()
{
    m_loggedIn = false;
    m_user.clear();
}

void SwConnector::setupRequest(KIO::TransferJob* const job, const QString& requestPath, const QString& method,
                               const QString& md5, const QString& type,
                               const QString& length, bool needsPassword = true)
{
    QCA::Initializer init;

    QString date = QDateTime::currentDateTime().toString(Qt::ISODate);

    // compute the authorization header line
    QString authorizationRaw = method + '\n'
        + md5 + '\n'
        + type + '\n'
        + length + '\n'
        + date + '\n'
        + requestPath;

    if (needsPassword)
    {
        QString encodedPassword  = QCA::Hash( "sha1" ).hashToString( m_user.password.toUtf8() );
        authorizationRaw        += ('\n' + encodedPassword);
    }

    QCA::SecureArray key( m_apiSecretKey.toUtf8() );
    QCA::SecureArray message( authorizationRaw.toUtf8() );

    QCA::MessageAuthenticationCode hmacObject( "hmac(sha1)", QCA::SecureArray() );
    QCA::SymmetricKey keyObject( key );
    hmacObject.setup( keyObject );
    hmacObject.update( message );
    QCA::SecureArray authorizationRawHash = hmacObject.final();

    QCA::Base64 encoder;
    QString authorizationEncoded = encoder.arrayToString( authorizationRawHash );

    job->addMetaData("UserAgent", m_userAgent);

    QString customHeader = QString("Content-MD5: %1\r\nContent-Type: %2\r\nDate: %3\r\nAuthorization: Shenton: %4 : %5")
                .arg(md5)
                .arg(type)
                .arg(date)
                .arg(m_apiKey)
                .arg(authorizationEncoded);

    if (method == "GET")
    {
        customHeader += QString("\r\nContent-Length: %1").arg(length);
    }

    job->addMetaData("customHTTPHeader", customHeader);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

void SwConnector::getRestServiceURL()
{
    kDebug() << "getRestServiceURL: " << endl;

    if (m_job)
    {
        m_job->kill();
        m_job           = 0;
        m_resultHandler = 0;
    }

    emit signalBusy(true);

    QString method( "GET" );
    QString md5(    "" );
    QString type(   "text/plain" );
    QString length( "0" );

    m_resultHandler             = &SwConnector::requestRestURLResultHandler;
    KIO::TransferJob* const job = KIO::get(m_apiStartURL, KIO::Reload, KIO::HideProgressInfo);
    setupRequest(job, KUrl(m_apiStartURL).path(), method, md5, type, length, false); // initial request will NOT send password

    connect(job, SIGNAL(redirection(KIO::Job*,KUrl)),
            this, SLOT(slotRequestRestURLRedirection(KIO::Job*,KUrl)));

    m_job   = job;
    m_buffer.resize(0);
}

void SwConnector::slotRequestRestURLRedirection(KIO::Job* job, const KUrl& newUrl)
{
    kDebug() << "slotRequestRestURLRedirection: " << newUrl.prettyUrl() << endl;

    // we need to submit the data differently to the new url
    // so we must kill the existing job to prevent internal redirection
    disconnect(job, SIGNAL(result(KJob*)),
               this, SLOT(slotResult(KJob*)));

    job->kill();
    m_job = 0;

    m_apiStartURL = newUrl.prettyUrl();
    getRestServiceURL();
}

void SwConnector::listAlbums()
{
    if (m_job)
    {
        m_job->kill();
        m_job           = 0;
        m_resultHandler = 0;
    }

    emit signalBusy(true);

    QString requestPath = QString("/user/%1/albums")
                .arg( QString(QUrl::toPercentEncoding(m_user.email)) );

    QString method( "GET" );
    QString md5(    "" );
    QString type(   "text/plain" );
    QString length( "0" );

    m_resultHandler             = &SwConnector::listAlbumsResultHandler;
    KIO::TransferJob* const job = KIO::get(QString(m_apiDomainURL + m_apiRestPath + requestPath), KIO::Reload, KIO::HideProgressInfo);
    setupRequest(job, m_apiRestPath + requestPath, method, md5, type, length);

    m_job = job;
    m_buffer.resize(0);
}

void SwConnector::createAlbum(const SwAlbum& album)
{
    kDebug() << "createAlbum" << endl;

    emit signalBusy(true);

    QString requestPath = QString("/user/%1/album/name/%2")
                .arg( QString(QUrl::toPercentEncoding(m_user.email)) )
                .arg( QString(QUrl::toPercentEncoding(album.title)) );

    QString method( "POST" );
    QString md5(    "" );
    QString type(   "text/plain" );
    QString length( "0" );

    m_resultHandler             = &SwConnector::createAlbumResultHandler;
    KIO::TransferJob* const job = KIO::http_post(QString(m_apiDomainURL + m_apiRestPath + requestPath), QByteArray(), KIO::HideProgressInfo);
    setupRequest(job, m_apiRestPath + requestPath, method, md5, type, length);

    m_job = job;
    m_buffer.resize(0);
}

bool SwConnector::addPhoto(const QString& imgPath, long long albumID, const QString& /*caption*/)
{
    kDebug() << "addPhoto" << endl;

    emit signalBusy(true);

    QString requestPath = QString("/user/%1/album/id/%2/contribution?name=%3")
                .arg( QString(QUrl::toPercentEncoding(m_user.email)) )
                .arg( QString(QUrl::toPercentEncoding(QString::number(albumID))) )
                .arg( QString(QUrl::toPercentEncoding(KUrl(imgPath).fileName())) );

    QFile imageFile(imgPath);

    if (!imageFile.open(QIODevice::ReadOnly))
        return false;

    QByteArray imageData = imageFile.readAll();
    QString file_size    = QString::number(imageFile.size());
    imageFile.close();

    QCA::Initializer init;
    QString method( "POST" );
    QString md5    = QCA::Hash( "md5" ).hashToString( imageData );
    QString type   = KMimeType::findByUrl(imgPath)->name();
    QString length = file_size;

    m_resultHandler             = &SwConnector::addPhotoResultHandler;
    KIO::TransferJob* const job = KIO::http_post(QString(m_apiDomainURL + m_apiRestPath + requestPath), imageData, KIO::HideProgressInfo);
    setupRequest(job, m_apiRestPath + requestPath, method, md5, type, length);

    m_job = job;
    m_buffer.resize(0);

    return true;
}

void SwConnector::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

void SwConnector::slotResult(KJob *kjob)
{
    m_job               = 0;
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);
    (this->*m_resultHandler)( job, m_buffer );
}

QDomElement SwConnector::getResponseDoc(KIO::Job* const job, const QByteArray& data) const
{
    QDomDocument failureDoc("failure");
    failureDoc.setContent( QString("<failure />") );
    QDomElement failureDocElem = failureDoc.documentElement();

    // check for transfer errors
    if (job->error())
    {
        kDebug() << "getResponseDoc: job error"  << endl;
        // show error message
        job->ui()->setWindow(m_parent);
        job->ui()->showErrorMessage();
        return failureDocElem;
    }

    // check for HTTP errors - how to do that?

    // check response content
    QDomDocument doc("response");

    if (!doc.setContent(data))
    {
        // not XML data!
        kDebug() << "getResponseDoc: response data is not XML"  << endl;
        return failureDocElem;
    }

    QDomElement docElem = doc.documentElement();

    if (docElem.tagName() != "resp")
    {
        // unexpected xml content
        kDebug() << "getResponseDoc: unexpected xml content"  << endl;
        return failureDocElem;
    }

    QString stat( docElem.attribute("stat") );

    if ("error" == stat)
    {
        QDomNode errorNode = docElem.firstChild();
        QString errorCode  = errorNode.toElement().attribute("code");
        kDebug() << "getResponseDoc: service error: " << errorCode << endl;
        // more work needed...

        // application level error
        // there are some expected errors we can handle here for all requests
        if ("access.key.invalid" == errorCode)
        {
            emit signalShwupKipiBlackListed();
            return failureDocElem;
        }

        if ("signature.not.matched" == errorCode)
        {
            // mot likely cause is wrong password...
            emit signalShwupInvalidCredentials();
            // emit signalShwupSignatureError();
            return failureDocElem;
        }

        if ("authorization.invalid" == errorCode)
        {
            emit signalShwupInvalidCredentials();
            return failureDocElem;
        }
    }

    // response is valid, handler will use data.
    kDebug() << "getResponseDoc: valid response: " << data << endl;
    return docElem;
}

void SwConnector::requestRestURLResultHandler(KIO::Job* const job, const QByteArray& data)
{
    kDebug() << "requestRestURLResultHandler: " << endl;

    QDomElement docElem = getResponseDoc(job, data);

    if (docElem.tagName() == "failure")
    {
        // there was an error, handle specifics now
        emit signalBusy(false);
        // emit signalListAlbumsDone(1, "", albumsList);
        return;
    }
    else if (docElem.attribute("stat") != "ok")
    {
        // no specific error code to handle here, just return...
        emit signalBusy(false);
        emit signalRequestRestURLDone(1, "service error");
        return;
    }

    QDomNodeList urlNodes = docElem.elementsByTagName("serviceUrl");

    if (urlNodes.length() == 1)
    {
        QRegExp rx("^(https?://[^/]+)(.+)$");
        if ( rx.exactMatch( urlNodes.at(0).toElement().text() ) )
        {
            m_apiDomainURL = rx.cap(1);
            m_apiRestPath  = rx.cap(2);
            kDebug() << "requestRestURLResultHandler: " << m_apiDomainURL << m_apiRestPath << endl;
            emit signalBusy(false);
            emit signalRequestRestURLDone(0, "");
            return;
        }

        // invalid url?
        emit signalBusy(false);
        emit signalRequestRestURLDone(3, "invalid url provided in service response");
    }
    else
    {
        // unexpected error
        emit signalBusy(false);
        emit signalRequestRestURLDone(2, "unexpected error");
    }
}

void SwConnector::listAlbumsResultHandler(KIO::Job* const job, const QByteArray& data)
{
    QList <SwAlbum> albumsList;

    QDomElement docElem = getResponseDoc(job, data);

    if (docElem.tagName() == "failure")
    {
        // there was an error, handle specifics now
        emit signalBusy(false);
        // emit signalListAlbumsDone(1, "", albumsList);
        return;
    }
    else if (docElem.attribute("stat") != "ok")
    {
        // no specific error code to handle here, just return...
        emit signalBusy(false);
        emit signalListAlbumsDone(1, "", albumsList);
        return;
    }

    // call was successful, credentials are valid, user is now logged in
    m_loggedIn              = true;
    QDomNodeList albumNodes = docElem.elementsByTagName("album");

    for (uint idx=0; idx < albumNodes.length(); ++idx)
    {
        QDomNode albumNode = albumNodes.at(idx);

        // album found, can user upload to it?
        // in list view response, we expect a single node "userCanUpload" corresponding to current user
        QDomNodeList canUploadNodes = albumNode.toElement().elementsByTagName("userCanUpload");

        if (canUploadNodes.length() == 1 && canUploadNodes.at(0).toElement().text().toLower() != "true")
        {
            // user is now allowed to upload in that album, ignoring it for album list of export plugin
            continue;
        }
        else
        {
            // this case shouldn't happen, we assume user can upload to given album
        }

        // album is valid, extract album info
        SwAlbum album;

        for (QDomNode albumInfoNode = albumNode.toElement().firstChild(); !albumInfoNode.isNull();
            albumInfoNode = albumInfoNode.nextSibling())
        {
            if (!albumInfoNode.isElement())
                continue;

            QString nodeName = albumInfoNode.nodeName().toLower();

            if ("id" == nodeName)
                album.id = albumInfoNode.toElement().text().toLongLong();

            else if ("token" == nodeName)
                album.token = albumInfoNode.toElement().text();

            else if ("title" == nodeName)
                album.title = albumInfoNode.toElement().text();

            else if ("description" == nodeName)
                album.description = albumInfoNode.toElement().text();

            else if ("albumimage" == nodeName)
                album.albumThumbnailUrl = albumInfoNode.toElement().text();

            else if ("albumurl" == nodeName)
                album.albumUrl = albumInfoNode.toElement().text();
        }

        albumsList.append(album);
    }

    qSort(albumsList.begin(), albumsList.end());

    emit signalBusy(false);
    emit signalListAlbumsDone(0, "", albumsList);
}

void SwConnector::createAlbumResultHandler(KIO::Job* const job, const QByteArray& data)
{
    SwAlbum newAlbum;

    QDomElement docElem = getResponseDoc(job, data);

    if (docElem.tagName() == "failure")
    {
        // there was an error, handle specifics now
        emit signalBusy(false);
        emit signalCreateAlbumDone(1, "", newAlbum);
        return;
    }
    else if (docElem.attribute("stat") != "ok")
    {
        // no specific error code to handle here, just return...
        emit signalBusy(false);
        emit signalCreateAlbumDone(2, "", newAlbum);
        return;
    }

    QDomNode albumNode = docElem.firstChild();

    if (albumNode.isElement() && albumNode.nodeName() == "album")
    {
        for (QDomNode albumInfoNode = albumNode.toElement().firstChild(); !albumInfoNode.isNull();
             albumInfoNode = albumInfoNode.nextSibling())
        {
            if (!albumInfoNode.isElement())
                continue;

            QString nodeName = albumInfoNode.nodeName().toLower();

            if ("id" == nodeName)
                newAlbum.id = albumInfoNode.toElement().text().toLongLong();

            else if ("token" == nodeName)
                newAlbum.token = albumInfoNode.toElement().text();

            else if ("title" == nodeName)
                newAlbum.title = albumInfoNode.toElement().text();

            else if ("description" == nodeName)
                newAlbum.description = albumInfoNode.toElement().text();

            else if ("albumimage" == nodeName)
                newAlbum.albumThumbnailUrl = albumInfoNode.toElement().text();

            else if ("albumurl" == nodeName)
                newAlbum.albumUrl = albumInfoNode.toElement().text();
        }
    }

    emit signalBusy(false);
    emit signalCreateAlbumDone(0, "", newAlbum);
}

void SwConnector::addPhotoResultHandler(KIO::Job* const job, const QByteArray& data)
{
    QDomElement docElem = getResponseDoc(job, data);

    if (docElem.tagName() == "failure")
    {
        // there was an error, handle specifics now
        emit signalBusy(false);
        emit signalAddPhotoDone(1, "");
        return;
    }
    else if (docElem.attribute("stat") != "ok")
    {
        // no specific error code to handle here, just return...
        emit signalBusy(false);
        emit signalAddPhotoDone(2, "");
        return;
    }

    // photo was added successfulyy, we don't care about details:)
    emit signalBusy(false);
    emit signalAddPhotoDone(0, "");
}

} // namespace KIPIShwupPlugin
