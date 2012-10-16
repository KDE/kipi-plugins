/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "imgurtalkerauth.h"

// QtKOAuth includes

#include <QtKOAuth>

// Qt includes

#include <QImageReader>

// qJson include

#include <qjson/parser.h>

// KDE includes

#include <kdebug.h>

// Local includes

#include "kpversion.h"
#include "plugin_imgurexport.h"

namespace KIPIImgurExportPlugin
{

class ImgurTalkerAuth::ImgurTalkerAuthPriv
{
public:

    ImgurTalkerAuthPriv()
    {
        userAgent = QString("KIPI-Plugins-" + Plugin_ImgurExport::name() + "/" + kipipluginsVersion());

        const char _imgurApiConsumerKey[] = _IMGUR_API_CONSUMER_KEY;
        consumerKey = QByteArray( _imgurApiConsumerKey );

        const char _imgurApiConsumerSecret[] = _IMGUR_API_CONSUMER_SECRET;
        consumerSecret = QByteArray( _imgurApiConsumerSecret );

        continueUpload = true;
    }

    QString        userAgent;

    KQOAuthManager *m_OAuthService;
    KQOAuthRequest *m_OAuthRequest;

    QByteArray consumerKey;
    QByteArray consumerSecret;
    QByteArray oauthToken;
    QByteArray oauthTokenSecret;

    bool       continueUpload;
};

ImgurTalkerAuth::ImgurTalkerAuth (Interface* const interface, QWidget* const parent)
    : ImgurTalker(interface, parent), d(new ImgurTalkerAuthPriv)
{
    d->m_OAuthRequest = new KQOAuthRequest();
    d->m_OAuthService = new KQOAuthManager(this);
}

ImgurTalkerAuth::~ImgurTalkerAuth()
{
    delete d;
}

const QString ImgurTalkerAuth::getAuthError (KQOAuthManager::KQOAuthError error)
{
    /**/
    switch (error) {
    case KQOAuthManager::NetworkError:               // Network error: timeout, cannot connect.
        return QString ("Network error: timeout, cannot connect.");
        break;
    case KQOAuthManager::RequestEndpointError:       // Request endpoint is not valid.
        return QString ("Request endpoint is not valid.");
        break;
    case KQOAuthManager::RequestValidationError:     // Request is not valid: some parameter missing?
        return QString ("Request is not valid: some parameter missing?");
        break;
    case KQOAuthManager::RequestUnauthorized:        // Authorization error: trying to access a resource without tokens.
        return QString ("Authorization error: trying to access a resource without tokens.");
        break;
    case KQOAuthManager::RequestError:               // The given request to KQOAuthManager is invalid: NULL?,
        return QString ("The given request is invalid.");
        break;
    case KQOAuthManager::ManagerError:                // Manager error, cannot use for sending requests.
        return QString ("Manager error, cannot use for sending requests.");
        break;
    case KQOAuthManager::NoError:                    // No error
    default:
        return QString ("No error");
        break;
    }
    /**/
}

void ImgurTalkerAuth::cancel()
{
    if (d->m_OAuthRequest)
    {
        d->m_OAuthRequest->clearRequest();
    }

    emit signalBusy(false);
}

void ImgurTalkerAuth::imageUpload (const KUrl& filePath)
{
    kDebug() << "Authenticated" << (d->m_OAuthService->isAuthorized() ? "Yes" : "No");
    setCurrentUrl(filePath);

    kDebug() << "Authenticated upload of" << currentUrl();

    state = IE_ADDPHOTO;

    emit signalUploadStart(filePath);
    emit signalBusy(true);

    QByteArray contentType = QImageReader::imageFormat(filePath.path());

    QFile imageFile(filePath.path());
    if (!imageFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    QByteArray imageData = imageFile.readAll();

    d->m_OAuthRequest->setEnableDebugOutput (false);
    d->m_OAuthRequest->initRequest(KQOAuthRequest::AuthorizedRequest, KUrl(ImgurConnection::APIuploadURL()));
    d->m_OAuthRequest->setConsumerKey(d->consumerKey.data() );
    d->m_OAuthRequest->setConsumerSecretKey(d->consumerSecret.data() );
    d->m_OAuthRequest->setToken( d->oauthToken.data() );
    d->m_OAuthRequest->setTokenSecret( d->oauthTokenSecret.data() );

    KQOAuthParameters params;
    params.insert("name", filePath.fileName());
    params.insert("title", filePath.fileName());
    params.insert("content-type", contentType.data());
    params.insert("content-length", QString("Content-Length: %1").arg(imageData.toBase64().length()));
    params.insert("UserAgent", d->userAgent);
    params.insert("image", imageData.toBase64());

    d->m_OAuthRequest->setAdditionalParameters(params);
    d->m_OAuthService->executeRequest(d->m_OAuthRequest);

//    connect(d->m_OAuthService, SIGNAL(requestReady(QByteArray)),
//            this, SLOT(slotRequestReady(QByteArray)));
//    connect(d->m_OAuthService, SIGNAL(authorizedRequestDone()),
//            this, SLOT(slotAuthorizedRequestDone()));
}

void ImgurTalkerAuth::slotOAuthLogin ()
{
    state = IE_LOGIN;

    d->m_OAuthRequest->initRequest(KQOAuthRequest::TemporaryCredentials, KUrl(ImgurConnection::OAuthTokenEndPoint()));
    d->m_OAuthRequest->setConsumerKey(d->consumerKey.data());
    d->m_OAuthRequest->setConsumerSecretKey(d->consumerSecret.data());

    d->m_OAuthRequest->setEnableDebugOutput(true);

    connect(d->m_OAuthService, SIGNAL(temporaryTokenReceived(QString,QString)),
            this, SLOT(slotTemporaryTokenReceived(QString, QString)));

    connect(d->m_OAuthService, SIGNAL(authorizationReceived(QString,QString)),
            this, SLOT(slotAuthorizationReceived(QString, QString)));

    connect(d->m_OAuthService, SIGNAL(accessTokenReceived(QString,QString)),
            this, SLOT(slotAccessTokenReceived(QString,QString)));

    connect(d->m_OAuthService, SIGNAL(requestReady(QByteArray)),
            this, SLOT(slotRequestReady(QByteArray)));

    d->m_OAuthService->setHandleUserAuthorization(true);

    d->m_OAuthService->executeRequest(d->m_OAuthRequest);

    emit signalBusy(true);
}

void ImgurTalkerAuth::slotTemporaryTokenReceived(QString token, QString tokenSecret)
{
    kDebug() << "Temporary token received: " << token << tokenSecret;

    if( d->m_OAuthService->lastError() == KQOAuthManager::NoError)
    {
        kDebug() << "Asking for user's permission to access protected resources. Opening URL: " << ImgurConnection::OAuthAuthorizationEndPoint();
        d->m_OAuthService->getUserAuthorization(KUrl(ImgurConnection::OAuthAuthorizationEndPoint()));
    }

    if (d->m_OAuthService->lastError() != KQOAuthManager::NoError)
    {
//        emit signalAuthenticated(false, getAuthError(d->m_OAuthService->lastError()));
//        emit signalBusy(false);
        kDebug() << "Error :" << getAuthError(d->m_OAuthService->lastError());

    }

}

void ImgurTalkerAuth::slotAuthorizationReceived(QString token, QString verifier) {
    kDebug() << "User authorization received: " << token << verifier;

    if (d->m_OAuthService->lastError() == KQOAuthManager::NoError)
    {
        d->m_OAuthService->getUserAccessTokens(KUrl(ImgurConnection::OAuthAccessEndPoint()));

    }
    if( d->m_OAuthService->lastError() != KQOAuthManager::NoError)
    {
        emit signalAuthenticated(false, getAuthError(d->m_OAuthService->lastError()));
        emit signalBusy(false);
        d->m_OAuthRequest->clearRequest();
        kDebug() << "Auth error :" << getAuthError(d->m_OAuthService->lastError());
    }
}

void ImgurTalkerAuth::slotAccessTokenReceived(QString token, QString tokenSecret) {
    kDebug() << "Access token received: " << token << tokenSecret;

    d->oauthToken = token.toAscii();
    d->oauthTokenSecret = tokenSecret.toAscii();

    emit signalAuthenticated(true, "OK");
    emit signalBusy(false);
    kDebug() << "Access tokens now stored";
}

void ImgurTalkerAuth::slotAuthorizedRequestDone() {
    kDebug() << "Request received from Imgur!";
}

void ImgurTalkerAuth::slotRequestReady(QByteArray response) {
//    kDebug() << "Authorized: " << d->m_OAuthService->isAuthorized();
//    kDebug() << "Verified: " << d->m_OAuthService->isAuthorized();
//    kDebug() << "End point: " << d->m_OAuthRequest->requestEndpoint();

//    kDebug() << "Response from the service: " << response;

    if (d->m_OAuthService->isAuthorized() && d->m_OAuthService->isVerified() &&
        d->m_OAuthRequest->requestEndpoint() == QUrl(ImgurConnection::APIuploadURL())
       )
    {
        ImgurTalker::parseResponse (response);
    }

    return;
}

void ImgurTalkerAuth::slotContinueUpload(bool yes)
{
    d->continueUpload = yes;
    if (yes && !m_queue->isEmpty()) {
        if (!d->m_OAuthService->isAuthorized())
        {
            // not authenticated, we upload anonymously
            ImgurTalker::imageUpload (m_queue->first());
        } else
        {
            // the top of the queue was already removed - first() is a new image
            imageUpload (m_queue->first());
        }
    }
    return;
}

} // namespace KIPIImgurExportPlugin
