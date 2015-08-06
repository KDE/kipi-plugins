/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2008-2010 by Luka Renko <lure at kubuntu dot org>
 * Copyright (c) 2011      by Dirk Tilger <dirk.kde@miriup.de>
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

#include "fbtalker.moc"

// C++ includes

#include <ctime>

// Qt includes

#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QtAlgorithms>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QList>
#include <qjson/parser.h>

// KDE includes

#include <kcodecs.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>

// Local includes

#include "kpversion.h"
#include "fbitem.h"
#include "mpform.h"

namespace KIPIFacebookPlugin
{

bool operator< (const FbUser& first, const FbUser& second)
{
    return first.name < second.name;
}

bool operator< (const FbAlbum& first, const FbAlbum& second)
{
    return first.title < second.title;
}

// -----------------------------------------------------------------------------

FbTalker::FbTalker(QWidget* const parent)
{
    m_parent          = parent;
    m_job             = 0;
    m_loginInProgress = 0;
    m_sessionExpires  = 0;
    m_state           = FB_GETLOGGEDINUSER;

    m_apiVersion      = "2.4";
    m_apiURL          = KUrl("https://graph.facebook.com");
    m_secretKey       = "5b0b5cd096e110cd4f4c72f517e2c544";
    m_appID           = "400589753481372";
}

FbTalker::~FbTalker()
{
    // do not logout - may reuse session for next upload

    if (m_job)
        m_job->kill();
}

bool FbTalker::loggedIn() const
{
    return !m_accessToken.isEmpty();
}

QString FbTalker::getAccessToken() const
{
    return m_accessToken;
}

unsigned int FbTalker::getSessionExpires() const
{
    return m_sessionExpires;
}

FbUser FbTalker::getUser() const
{
    return m_user;
}

void FbTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(false);
}

/** Compute MD5 signature using url queries keys and values:
    http://wiki.developers.facebook.com/index.php/How_Facebook_Authenticates_Your_Application
    This method was used for the legacy authentication scheme and has been obsoleted with OAuth2 authentication.
*/
/*
QString FbTalker::getApiSig(const QMap<QString, QString>& args)
{
    QString concat;
    // NOTE: QMap iterator will sort alphabetically
    for (QMap<QString, QString>::const_iterator it = args.constBegin();
         it != args.constEnd();
         ++it)
    {
        concat.append(it.key());
        concat.append("=");
        concat.append(it.value());
    }
    if (args["session_key"].isEmpty())
        concat.append(m_secretKey);
    else
        concat.append(m_sessionSecret);

    KMD5 md5(concat.toUtf8());
    return md5.hexDigest().data();
}
*/

QString FbTalker::getCallString(const QMap<QString, QString>& args)
{
    QString concat;
    QUrl url;

    // NOTE: QMap iterator will sort alphabetically
    for (QMap<QString, QString>::const_iterator it = args.constBegin();
         it != args.constEnd();
         ++it)
    {
        /*if (!concat.isEmpty())
            concat.append("&");*/

        /*concat.append(it.key());
        concat.append("=");
        concat.append(it.value());*/
        url.addQueryItem(it.key(), it.value());
    }
    concat.append(url.encodedQuery());

    kDebug() << "CALL: " << concat;

    return concat;
}

void FbTalker::authenticate(const QString &accessToken, unsigned int sessionExpires)
{
    m_loginInProgress = true;

    if (!accessToken.isEmpty() && ( sessionExpires == 0 || sessionExpires > (unsigned int)(time(0) + 900)))
    {
        // sessionKey seems to be still valid for at least 15 minutes
        // - check if it still works
        m_accessToken    = accessToken;
        m_sessionExpires = sessionExpires;

        emit signalLoginProgress(2, 9, i18n("Validate previous session..."));

        // get logged in user - this will check if session is still valid
        getLoggedInUser();
    }
    else
    {
        // session expired -> get new authorization token and session
        doOAuth();
    }
}

/**
 * upgrade session key to OAuth
 *
 * This method (or step) can be removed after June 2012 (a year after its
 * implementation), since it is only a convenience method for those people
 * who just upgraded and have an active session using the old authentication.
 */
void FbTalker::exchangeSession(const QString& sessionKey)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    emit signalLoginProgress(1, 9, i18n("Upgrading to OAuth..."));

    QMap<QString, QString> args;
    args["client_id"]     = m_appID;
    args["client_secret"] = m_secretKey;
    args["sessions"]      = sessionKey;

    QByteArray tmp(getCallString(args).toUtf8());
    KIO::TransferJob* const job = KIO::http_post(KUrl("https://graph.facebook.com/oauth/exchange_sessions"), tmp, KIO::HideProgressInfo);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FB_EXCHANGESESSION;
    m_job   = job;
    m_buffer.resize(0);
}

/**
 * Authenticate using OAuth
 *
 * TODO (Dirk): There's some GUI code slipped in here,
 * that really doesn't feel like it's belonging here.
 */
void FbTalker::doOAuth()
{
    // just in case
    m_loginInProgress = true;

    // TODO (Dirk): 
    // Find out whether this signalBusy is used here appropriately.
    emit signalBusy(true);

    KUrl url("https://www.facebook.com/dialog/oauth");
    url.addQueryItem("client_id", m_appID);
    url.addQueryItem("redirect_uri", "https://www.facebook.com/connect/login_success.html");
    // TODO (Dirk): Check which of these permissions can be optional.
    url.addQueryItem("scope","user_photos,publish_actions,user_friends");
    url.addQueryItem("response_type", "token");
    kDebug() << "OAuth URL: " << url;
    KToolInvocation::invokeBrowser(url.url());

    emit signalBusy(false);

    KDialog* const window         = new KDialog(kapp->activeWindow(), 0);
    window->setModal(true);
    window->setWindowTitle( i18n("Facebook Application Authorization") );
    window->setButtons(KDialog::Ok | KDialog::Cancel);
    QWidget* const main           = new QWidget(window, 0);
    QLineEdit* const textbox      = new QLineEdit( );
    QPlainTextEdit* const infobox = new QPlainTextEdit( i18n(
        "Please follow the instructions in the browser window. "
        "When done, copy the Internet address from your browser into the textbox below and press \"OK\"."
    ) );

    infobox->setReadOnly(true);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(infobox);
    layout->addWidget(textbox);
    main->setLayout(layout);
    window->setMainWidget(main);

    if( window->exec()  == QDialog::Accepted )
    {
        // Error code and reason from the Facebook service
        QString errorReason;
        QString errorCode;

        url                        = KUrl( textbox->text() );
        QString fragment           = url.fragment();
        kDebug() << "Split out the fragment from the URL: " << fragment;
        QStringList params         = fragment.split('&');
        QList<QString>::iterator i = params.begin();

        while( i != params.end() )
        {
            QStringList keyvalue = (*i).split('=');

            if( keyvalue.size() == 2 )
            {
                if( ! keyvalue[0].compare( "access_token" ) )
                {
                    m_accessToken = keyvalue[1];
                }
                else if( ! keyvalue[0].compare( "expires_in" ) )
                {
                    m_sessionExpires = keyvalue[1].toUInt();

                    if( m_sessionExpires != 0 )
                    {
#if QT_VERSION >= 0x40700
                        m_sessionExpires += QDateTime::currentMSecsSinceEpoch() / 1000;
#else
                        m_sessionExpires += QDateTime::currentDateTime().toTime_t();
#endif
                    }
                }
                else if( ! keyvalue[0].compare( "error_reason" ) )
                {
                    errorReason = keyvalue[1];
                }
                else if( ! keyvalue[0].compare( "error" ) )
                {
                    errorCode = keyvalue[1];
                }
            }

            ++i;
        }

        if( !m_accessToken.isEmpty() && errorCode.isEmpty() && errorReason.isEmpty() )
        {
            return getLoggedInUser();
        }
    }

    authenticationDone(-1, i18n("Canceled by user."));

    // TODO (Dirk): Correct?
    emit signalBusy(false);
}

void FbTalker::getLoggedInUser()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    emit signalLoginProgress(3);
    
    KUrl url("https://graph.facebook.com/me");
    url.addQueryItem("access_token",m_accessToken); 
    url.addQueryItem("fields","id,name,link");
    KIO::TransferJob* const job = KIO::get(url,KIO::NoReload,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FB_GETLOGGEDINUSER;
    m_job   = job;
    m_buffer.resize(0);
}

void FbTalker::logout()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QMap<QString, QString> args;
    args["next"] = QString("http://www.digikam.org");
    args["access_token"] = m_accessToken;
    
    KUrl url("https://www.facebook.com/logout.php");
    url.addQueryItem("next",QString("http://www.digikam.org"));
    url.addQueryItem("access_token",m_accessToken);
    kDebug() << "Logout URL: " << url;
    KToolInvocation::invokeBrowser(url.url());
    
    emit signalBusy(false);
    
}


// NOTE: This function will only list the friends who also use digiKam app. This is mentioned in https://developers.facebook.com/docs/apps/changelog#v2_0
void FbTalker::listFriends()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    KUrl url("https://graph.facebook.com/me/friends");
    url.addQueryItem("access_token",m_accessToken);
    KIO::TransferJob* const job = KIO::get(url,KIO::NoReload,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FB_LISTFRIENDS;
    m_job   = job;
    m_buffer.resize(0);
}

void FbTalker::listAlbums(long long userID)
{
    kDebug() << "Requesting albums for user " << userID;

    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    KUrl url("https://graph.facebook.com/me/albums");
    url.addQueryItem("fields","id,name,description,privacy,link,location");
    url.addQueryItem("access_token",m_accessToken);
    KIO::TransferJob* const job = KIO::get(url,KIO::NoReload,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FB_LISTALBUMS;
    m_job   = job;
    m_buffer.resize(0);
}

//NOTE: Re-implement this method using current Graph API when import to facebook is implemented. This method is obsolete.
void FbTalker::listPhotos(long long userID, const QString &albumID)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    QMap<QString, QString> args;
    args["access_token"] = m_accessToken;

    if (!albumID.isEmpty())
        args["aid"]      = albumID;
    else if (userID != 0)
        args["subj_id"]  = QString::number(userID);
    else
        args["subj_id"]  = QString::number(m_user.id);

    QByteArray tmp(getCallString(args).toUtf8());
    KIO::TransferJob* const job = KIO::http_post(KUrl(m_apiURL,"photos.get"), tmp, KIO::HideProgressInfo);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FB_LISTPHOTOS;
    m_job   = job;
    m_buffer.resize(0);
}

void FbTalker::createAlbum(const FbAlbum& album)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    QMap<QString, QString> args;
    args["access_token"] = m_accessToken;
    args["name"]         = album.title;

    if (!album.location.isEmpty())
        args["location"] = album.location;
    if (!album.description.isEmpty())
        args["description"] = album.description;

    // TODO (Dirk): Wasn't that a requested feature in Bugzilla?
    switch (album.privacy)
    {
        case FB_ME:
            args["privacy"] = "{'value':'SELF'}";
            break;
        case FB_FRIENDS:
            args["visible"] = "friends";
            break;
        case FB_FRIENDS_OF_FRIENDS:
            args["visible"] = "friends-of-friends";
            break;
        case FB_NETWORKS:
            args["visible"] = "networks";
            break;
        case FB_EVERYONE:
            args["visible"] = "everyone";
            break;
        case FB_CUSTOM:
            //TODO
            args["privacy"] = "{'value':'CUSTOM'}";
            break;
    }

    QByteArray tmp(getCallString(args).toUtf8());
    KIO::TransferJob* const job = KIO::http_post(KUrl("https://graph.facebook.com/me/albums"), tmp, KIO::HideProgressInfo);
    job->addMetaData("content-type",
                     "Content-Type: application/x-www-form-urlencoded");

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FB_CREATEALBUM;
    m_job   = job;
    m_buffer.resize(0);
}

bool FbTalker::addPhoto(const QString& imgPath, const QString& albumID, const QString& caption)
{
    kDebug() << "Adding photo " << imgPath << " to album with id "
             << albumID << " using caption '" << caption << "'";

    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    QMap<QString, QString> args;
    args["access_token"] = m_accessToken;
    args["name"]         = KUrl(imgPath).fileName();

    if (!caption.isEmpty())
        args["message"]  = caption;

    MPForm form;

    for (QMap<QString, QString>::const_iterator it = args.constBegin();
         it != args.constEnd();
         ++it)
    {
        form.addPair(it.key(), it.value());
    }

    if (!form.addFile(args["name"], imgPath))
    {
        emit signalBusy(false);
        return false;
    }

    form.finish();

    kDebug() << "FORM: " << endl << form.formData();

    KIO::TransferJob* const job = KIO::http_post(KUrl("https://graph.facebook.com/v2.4/"+QString(albumID)+QString("/photos")), form.formData(), KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FB_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    return true;
}

void FbTalker::getPhoto(const QString& imgPath)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    KIO::TransferJob* const job = KIO::get(imgPath, KIO::Reload, KIO::HideProgressInfo);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FB_GETPHOTO;
    m_job   = job;
    m_buffer.resize(0);
}

void FbTalker::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

QString FbTalker::errorToText(int errCode, const QString &errMsg)
{
    QString transError;
    kDebug() << "errorToText: " << errCode << ": " << errMsg;

    switch (errCode)
    {
        case 0:
            transError = "";
            break;
        case 2:
            transError = i18n("The service is not available at this time.");
            break;
        case 4:
            transError = i18n("The application has reached the maximum number of requests allowed.");
            break;
        case 102:
            transError = i18n("Invalid session key or session expired. Try to log in again.");
            break;
        case 120:
            transError = i18n("Invalid album ID.");
            break;
        case 321:
            transError = i18n("Album is full.");
            break;
        case 324:
            transError = i18n("Missing or invalid file.");
            break;
        case 325:
            transError = i18n("Too many unapproved photos pending.");
            break;
        default:
            transError = errMsg;
            break;
    }

    return transError;
}

void FbTalker::slotResult(KJob* kjob)
{
    m_job               = 0;
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

    if (job->error())
    {
        if (m_loginInProgress)
        {
            authenticationDone(job->error(), job->errorText());
        }
        else if (m_state == FB_ADDPHOTO)
        {
            emit signalBusy(false);
            emit signalAddPhotoDone(job->error(), job->errorText());
        }
        else if (m_state == FB_GETPHOTO)
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
        case(FB_EXCHANGESESSION):
            parseExchangeSession(m_buffer);
            break;
        case(FB_GETLOGGEDINUSER):
            parseResponseGetLoggedInUser(m_buffer);
            break;
        case(FB_LISTFRIENDS):
            parseResponseListFriends(m_buffer);
            break;
        case(FB_LISTALBUMS):
            parseResponseListAlbums(m_buffer);
            break;
        case(FB_LISTPHOTOS):
            parseResponseListPhotos(m_buffer);
            break;
        case(FB_CREATEALBUM):
            parseResponseCreateAlbum(m_buffer);
            break;
        case(FB_ADDPHOTO):
            parseResponseAddPhoto(m_buffer);
            break;
        case(FB_GETPHOTO):
            // all we get is data of the image
            emit signalBusy(false);
            emit signalGetPhotoDone(0, QString(), m_buffer);
            break;
    }
}

void FbTalker::authenticationDone(int errCode, const QString &errMsg)
{
    if (errCode != 0)
    {
        m_accessToken.clear();
        m_user.clear();
    }

    emit signalBusy(false);
    emit signalLoginDone(errCode, errMsg);
    m_loginInProgress = false;
}

int FbTalker::parseErrorResponse(const QDomElement& e, QString& errMsg)
{
    int errCode = -1;

    for (QDomNode node = e.firstChild();
         !node.isNull();
         node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        if (node.nodeName() == "error_code")
        {
            errCode = node.toElement().text().toInt();
            kDebug() << "Error Code:" << errCode;
        }
        else if (node.nodeName() == "error_msg")
        {
            errMsg = node.toElement().text();
            kDebug() << "Error Text:" << errMsg;
        }
    }

    return errCode;
}

void FbTalker::parseExchangeSession(const QByteArray& data)
{
    bool ok;
    QJson::Parser parser;

    kDebug() << "Parse exchange_session response:" << endl << data;

    QVariantList result = parser.parse (data, &ok).toList();

    if(ok)
    {
        QVariantMap session = result[0].toMap();
        m_accessToken       = session["access_token"].toString();
        m_sessionExpires    = session["expires"].toUInt();

        if( m_sessionExpires != 0 )
        {
#if QT_VERSION >= 0x40700
            m_sessionExpires += QDateTime::currentMSecsSinceEpoch() / 1000;
#else
            m_sessionExpires += QDateTime::currentDateTime().toTime_t();
#endif
        }

        if( m_accessToken.isEmpty() )
            // Session did not convert. Reauthenticate.
            doOAuth();
        else
            // Session converted to OAuth. Proceed normally.
            getLoggedInUser();
    }
    else
    {
        /// @todo Error code is not filled with a useful value anywhere
        int errCode = -1;
        QString errMsg;
        authenticationDone(errCode, errorToText(errCode, errMsg));
    }
}

void FbTalker::parseResponseGetLoggedInUser(const QByteArray& data)
{
    QJson::Parser parser;
    int errCode = -1;
    QString errMsg;
    bool ok;
    QVariant result     = parser.parse(data,&ok);
    QVariantMap rmap    = result.toMap();
    QList<QString> keys = rmap.uniqueKeys();
    QString temp;

    for(int i=0;i<rmap.size();i++)
    {
        kDebug()<<"Keys is "<<keys[i];
        if(keys[i] == "id")
        {
            m_user.id = (rmap[keys[i]].value<QString>()).toLongLong();
            errCode = 0;
            kDebug()<<"User id is"<<m_user.id;
        }
        if(keys[i] == "name")
        {
            m_user.name = rmap[keys[i]].value<QString>();
            kDebug()<<"User name is"<<m_user.name;
        }
        if(keys[i]=="link")
        {
            m_user.profileURL = rmap[keys[i]].value<QString>();
            kDebug()<<"Link is"<<m_user.profileURL;            
        }
        if(keys[i] == "error")
        {
            errCode = rmap[keys[i]].value<QVariant>().toMap().value("code").value<QString>().toLongLong();
            errMsg  = rmap[keys[i]].value<QVariant>().toMap().value("message").value<QString>();
        }
    }

    if(errCode!=0)
    {
        // it seems that session expired -> create new token and session
        m_accessToken.clear();
        m_sessionExpires = 0;
        m_user.clear();

        doOAuth();
    }
    else
        authenticationDone(0,"");
}

void FbTalker::parseResponseAddPhoto(const QByteArray& data)
{
    kDebug()<<"Parse Add Photo data is "<<data;
    int errCode = -1;
    QString errMsg;
    QJson::Parser parser;
    bool ok;
    QVariant result     = parser.parse(data,&ok);
    QVariantMap rmap    = result.toMap();
    QList<QString> keys = rmap.uniqueKeys();
    QString temp;

    for(int i=0;i<rmap.size();i++)
    {
        kDebug()<<"Keys is "<<keys[i];
        if(keys[i] == "id")
        {
            kDebug()<<"Id of photo exported is"<<rmap[keys[i]].value<QString>();
            errCode = 0;
        }
        if(keys[i] == "error")
        {
            errCode = rmap[keys[i]].value<QVariant>().toMap().value("code").value<QString>().toLongLong();
            errMsg  = rmap[keys[i]].value<QVariant>().toMap().value("message").value<QString>();
        }
    }

    emit signalBusy(false);
    emit signalAddPhotoDone(errCode, errorToText(errCode, errMsg));
}

void FbTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    kDebug()<<"Parse Create album data is"<<data;
    int errCode = -1;
    QString errMsg;
    QString newAlbumID;
    QJson::Parser parser;
    bool ok;
    QVariant result     = parser.parse(data,&ok);
    QVariantMap rmap    = result.toMap();
    QList<QString> keys = rmap.uniqueKeys();
    QString temp;

    for(int i=0;i<rmap.size();i++)
    {
        kDebug()<<"Keys is "<<keys[i];
        if(keys[i] == "id")
        {
            newAlbumID = rmap[keys[i]].value<QString>();
            kDebug()<<"Id of album created is"<<newAlbumID;
            errCode = 0;
        }
        if(keys[i] == "error")
        {
            errCode = rmap[keys[i]].value<QVariant>().toMap().value("code").value<QString>().toLongLong();
            errMsg  = rmap[keys[i]].value<QVariant>().toMap().value("message").value<QString>();
        }
    }

    emit signalBusy(false);
    emit signalCreateAlbumDone(errCode, errorToText(errCode, errMsg),
                               newAlbumID);
}

//TODO: Re-Write this method if needed. Parsing will return list of friends and also their info like id, name etc.
void FbTalker::parseResponseListFriends(const QByteArray& data)
{
//     int errCode = -1;
//     QString errMsg;
//     QDomDocument doc("getFriends");
// 
//     if (!doc.setContent(data))
//         return;
// 
//     kDebug() << "Parse Friends response:" << endl << data;
// 
//     QDomElement docElem = doc.documentElement();
//     QString friendsUIDs;
// 
//     if (docElem.tagName() == "friends_get_response")
//     {
//         for (QDomNode node = docElem.firstChild();
//              !node.isNull();
//              node = node.nextSibling())
//         {
//             if (!node.isElement())
//                 continue;
// 
//             if (node.nodeName() == "uid")
//             {
//                 if (!friendsUIDs.isEmpty())
//                     friendsUIDs.append(',');
//                 friendsUIDs.append(node.toElement().text());
//             }
//         }
//         errCode = 0;
//     }
//     else if (docElem.tagName() == "error_response")
//     {
//         errCode = parseErrorResponse(docElem, errMsg);
//     }
// 
//     if (friendsUIDs.isEmpty())
//     {
//         emit signalBusy(false);
// 
//         QList<FbUser> noFriends;
//         emit signalListFriendsDone(errCode, errorToText(errCode, errMsg),
//                                    noFriends);
//     }
//     else
//     {
//         // get user info for those users
//         //getUserInfo(friendsUIDs);
//     }
}

void FbTalker::parseResponseListAlbums(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    kDebug()<<"Albums data is "<<data;
    QJson::Parser parser;
    bool ok;
    QVariant result     = parser.parse(data,&ok);
    QVariantMap rmap    = result.toMap();
    QList<QString> keys = rmap.uniqueKeys();
    QList <FbAlbum> albumsList;

    for(int i=0;i<rmap.size();i++)
    {
        if(keys[i]=="data")
        {
            QVariantList res = rmap[keys[i]].value<QVariantList>();
            foreach(QVariant record, res) 
            {
                FbAlbum album;
                QVariantMap map   = record.toMap();
                album.id          = map.value("id").value<QString>();
                album.title       = map.value("name").value<QString>();
                album.location    = map.value("location").value<QString>();
                album.url         = map.value("link").value<QString>();
                album.description = map.value("description").value<QString>();

                if (QString::compare(map.value("privacy").value<QString>(), QString("friends"), Qt::CaseInsensitive) == 0)
                    album.privacy = FB_FRIENDS;
                else if (QString::compare(map.value("privacy").value<QString>(), QString("custom"), Qt::CaseInsensitive) == 0)
                    album.privacy = FB_CUSTOM;
                else if (QString::compare(map.value("privacy").value<QString>(), QString("everyone"), Qt::CaseInsensitive) == 0)
                    album.privacy = FB_EVERYONE;
                
                albumsList.append(album);
            }
            errCode = 0;
        }
        if(keys[i]=="error")
        {
            errCode = rmap[keys[i]].value<QVariant>().toMap().value("code").value<QString>().toLongLong();
            errMsg  = rmap[keys[i]].value<QVariant>().toMap().value("message").value<QString>();
        }
    }
    
    qSort(albumsList.begin(), albumsList.end());

    emit signalBusy(false);
    emit signalListAlbumsDone(errCode, errorToText(errCode, errMsg),
                              albumsList);
}

//NOTE: Re-implement this method using current Graph API when import to facebook is implemented. This method is obsolete.
void FbTalker::parseResponseListPhotos(const QByteArray& data)
{
    int errCode = -1;
    QString errMsg;
    QDomDocument doc("getPhotos");

    if (!doc.setContent(data))
        return;

    kDebug() << "Parse Photos response:" << endl << data;

    QDomElement docElem = doc.documentElement();
    QList <FbPhoto> photosList;

    if (docElem.tagName() == "photos_get_response")
    {
        for (QDomNode node = docElem.firstChild();
             !node.isNull();
             node = node.nextSibling())
        {
            if (!node.isElement())
                continue;

            if (node.nodeName() == "photo")
            {
                FbPhoto photo;

                for (QDomNode nodeP = node.toElement().firstChild();
                     !nodeP.isNull();
                     nodeP = nodeP.nextSibling())
                {
                    if (!nodeP.isElement())
                        continue;

                    if (nodeP.nodeName() == "pid")
                        photo.id = nodeP.toElement().text().trimmed();
                    else if (nodeP.nodeName() == "caption")
                        photo.caption = nodeP.toElement().text();
                    else if (nodeP.nodeName() == "src_small")
                        photo.thumbURL = nodeP.toElement().text();
                    else if (nodeP.nodeName() == "src_big")
                        photo.originalURL = nodeP.toElement().text();
                    else if (nodeP.nodeName() == "src"
                             && photo.originalURL.isEmpty())
                        photo.originalURL = nodeP.toElement().text();
                }

                photosList.append(photo);
            }
        }
        errCode = 0;
    }
    else if (docElem.tagName() == "error_response")
    {
        errCode = parseErrorResponse(docElem, errMsg);
    }

    emit signalBusy(false);
    emit signalListPhotosDone(errCode, errorToText(errCode, errMsg),
                              photosList);
}

} // namespace KIPIFacebookPlugin
