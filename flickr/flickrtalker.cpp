/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2009 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2009-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "flickrtalker.h"

// C++ includes

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

// Qt includes

#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QMap>
#include <QStringList>
#include <QProgressDialog>
#include <QUrlQuery>
#include <QStandardPaths>
#include <QtWidgets/QApplication>
#include <QDesktopServices>
#include <QMessageBox>

// KDE includes

#include <kjobwidgets.h>
#include <kio/jobuidelegate.h>

// Libkipi includes

#include <KIPI/PluginLoader>

// Local includes

#include "kpversion.h"
#include "mpform.h"
#include "flickritem.h"
#include "flickrwindow.h"
#include "kipiplugins_debug.h"

using namespace KIPIPlugins;

namespace KIPIFlickrPlugin
{

FlickrTalker::FlickrTalker(QWidget* const parent, const QString& serviceName)
{
    m_parent          = parent;
    m_job             = 0;
    m_photoSetsList   = 0;
    m_authProgressDlg = 0;
    m_state           = FE_LOGOUT;
    m_serviceName     = serviceName;
    m_iface           = 0;

    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
    }
        
    if (serviceName == QString::fromLatin1("23"))
    {
        m_apiUrl    = QString::fromLatin1("http://www.23hq.com/services/rest/");
        m_authUrl   = QString::fromLatin1("http://www.23hq.com/services/auth/");
        m_uploadUrl = QString::fromLatin1("http://www.23hq.com/services/upload/");

        // bshanks: do 23 and flickr really share API keys? or does 23 not need
        // one?
        m_apikey    = QString::fromLatin1("49d585bafa0758cb5c58ab67198bf632");
        m_secret    = QString::fromLatin1("34b39925e6273ffd");
    }
    else if (serviceName == QString::fromLatin1("Zooomr"))
    {
        m_apiUrl    = QString::fromLatin1("http://api.zooomr.com/services/rest/");
        m_authUrl   = QString::fromLatin1("http://www.zooomr.com/services/auth/");
        m_uploadUrl = QString::fromLatin1("http://upload.zooomr.com/services/upload/");

        m_apikey    = QString::fromLatin1("18c8db5ce9ed4e15a7b484136f5080c5");
        m_secret    = QString::fromLatin1("1ea4af14e3");
    }
    else
    {
        m_apiUrl    = QString::fromLatin1("https://www.flickr.com/services/rest/");
        m_authUrl   = QString::fromLatin1("https://www.flickr.com/services/auth/");
        m_uploadUrl = QString::fromLatin1("https://api.flickr.com/services/upload/");

        m_apikey    = QString::fromLatin1("49d585bafa0758cb5c58ab67198bf632");
        m_secret    = QString::fromLatin1("34b39925e6273ffd");
    }

    /* Initialize selected photo set as empty. */
    m_selectedPhotoSet = FPhotoSet();

    connect(this, SIGNAL(signalAuthenticate()),
            this, SLOT(slotAuthenticate()));
}

FlickrTalker::~FlickrTalker()
{
    if (m_job)
    {
        m_job->kill();
    }

    delete m_photoSetsList;
}

/** Compute MD5 signature using url queries keys and values following Flickr notice:
    http://www.flickr.com/services/api/auth.spec.html
*/
QString FlickrTalker::getApiSig(const QString& secret, const QUrl& url)
{
    QUrlQuery urlQuery(url.query());
    QList<QPair<QString, QString> > temp_queries = urlQuery.queryItems();
    QMap<QString, QString> queries;

    QPair<QString, QString> pair;

    foreach(pair,temp_queries)
    {
        queries.insert(pair.first,pair.second);
    }

    QString compressed(secret);

    // NOTE: iterator QMap iterator will sort alphabetically items based on key values.
    for (QMap<QString, QString>::iterator it = queries.begin() ; it != queries.end(); ++it)
    {
        compressed.append(it.key());
        compressed.append(it.value());
    }

    QCryptographicHash context(QCryptographicHash::Md5);
    context.addData(compressed.toUtf8());
    return QLatin1String(context.result().toHex().data());
}

QString FlickrTalker::getMaxAllowedFileSize()
{
    return m_maxSize;
}

void FlickrTalker::maxAllowedFileSize()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QUrl url(m_apiUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("auth_token"), m_token);
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
    urlQuery.addQueryItem(QString::fromLatin1("method"), QString::fromLatin1("flickr.people.getLimits"));
    url.setQuery(urlQuery);
    QString md5 = getApiSig(m_secret, url);
    urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "Get max file size url: " << url;

    KIO::TransferJob* job = 0;
    
    if (m_serviceName == QString::fromLatin1("Zooomr"))
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        QByteArray tmp;
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData(QString::fromLatin1("content-type"), QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_GETMAXSIZE;
    m_authProgressDlg->setLabelText(i18n("Getting the maximum allowed file size."));
    m_authProgressDlg->setMaximum(4);
    m_authProgressDlg->setValue(1);
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

// MD5 signature of the request.
/*
QString FlickrTalker::getApiSig(const QString& secret, const QStringList &headers)
{
    QMap<QString, QString> queries = url.queryItems();
    QString compressed(secret);

    // NOTE: iterator QMap iterator will sort alphabetically items based on key values.
    for (QMap<QString, QString>::iterator it = queries.begin() ; it != queries.end(); ++it)
    {
        compressed.append(it.key());
        compressed.append(it.value());
    }

    QCryptographicHash context(QCryptographicHash::Md5);
    context.addData(compressed.toUtf8());
    return context.result().toHex().data();
}
*/

/**get the API sig and send it to the server server should return a frob.
*/
void FlickrTalker::getFrob()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QUrl url(m_apiUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("method"), QString::fromLatin1("flickr.auth.getFrob"));
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
    url.setQuery(urlQuery);

    QString md5 = getApiSig(m_secret, url);
    urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "Get frob url: " << url;

    KIO::TransferJob* job = 0;

    if (m_serviceName == QString::fromLatin1("Zooomr"))
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        QByteArray tmp;
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData(QString::fromLatin1("content-type"), QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_GETFROB;
    m_authProgressDlg->setLabelText(i18n("Getting the Frob"));
    m_authProgressDlg->setMaximum(4);
    m_authProgressDlg->setValue(1);
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

    QUrl url(m_apiUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("method"), QString::fromLatin1("flickr.auth.checkToken"));
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
    urlQuery.addQueryItem(QString::fromLatin1("auth_token"), token);
    url.setQuery(urlQuery);
    QString md5 = getApiSig(m_secret, url);
    urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
    url.setQuery(urlQuery);

    qCDebug(KIPIPLUGINS_LOG) << "Check token url: " << url;
    QByteArray tmp;

    KIO::TransferJob* job = 0;

    if (m_serviceName == QString::fromLatin1("Zooomr"))
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow the
        // redirect
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData(QString::fromLatin1("content-type"), QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_CHECKTOKEN;
    m_authProgressDlg->setLabelText(i18n("Checking if previous token is still valid"));
    m_authProgressDlg->setMaximum(4);
    m_authProgressDlg->setValue(1);
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

void FlickrTalker::slotAuthenticate()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QUrl url(m_authUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
    urlQuery.addQueryItem(QString::fromLatin1("frob"), m_frob);
    urlQuery.addQueryItem(QString::fromLatin1("perms"), QString::fromLatin1("write"));
    url.setQuery(urlQuery);

    QString md5 = getApiSig(m_secret, url);
    urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "Authenticate url: " << url;

    QDesktopServices::openUrl(url);

    QMessageBox quest(QMessageBox::Question,
                      i18n("%1 Service Web Authorization", m_serviceName),
                      i18n("Please follow the instructions in the browser window, then "
                           "return to press corresponding button."),
                      QMessageBox::Yes | QMessageBox::No);

    (quest.button(QMessageBox::Yes))->setText(i18n("I am authenticated"));
    (quest.button(QMessageBox::No))->setText(i18n("I am not authenticated"));

    if (quest.exec() == QMessageBox::Yes)
    {
        getToken();
        m_authProgressDlg->setLabelText(i18n("Authenticating the User on web"));
        m_authProgressDlg->setMaximum(4);
        m_authProgressDlg->setValue(2);
        emit signalBusy(false);
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "User didn't proceed with getToken Authorization, cannot proceed further, aborting";
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

    QUrl url(m_apiUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
    urlQuery.addQueryItem(QString::fromLatin1("method"), QString::fromLatin1("flickr.auth.getToken"));
    urlQuery.addQueryItem(QString::fromLatin1("frob"), m_frob);
    url.setQuery(urlQuery);
    QString md5 = getApiSig(m_secret, url);
    urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "Get token url: " << url;

    KIO::TransferJob* job = 0;

    if (m_serviceName == QString::fromLatin1("Zooomr"))
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        QByteArray tmp;
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData(QString::fromLatin1("content-type"), QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_GETTOKEN;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    m_authProgressDlg->setLabelText(i18n("Getting the Token from the server"));
    m_authProgressDlg->setMaximum(4);
    m_authProgressDlg->setValue(3);
}

void FlickrTalker::listPhotoSets()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    qCDebug(KIPIPLUGINS_LOG) << "List photoset invoked";
    QUrl url(m_apiUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("auth_token"), m_token);
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
    urlQuery.addQueryItem(QString::fromLatin1("method"), QString::fromLatin1("flickr.photosets.getList"));
    url.setQuery(urlQuery);
    QString md5 = getApiSig(m_secret, url);
    urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "List photoset URL" << url;
    QByteArray tmp;
    KIO::TransferJob* job = 0;

    if (m_serviceName == QString::fromLatin1("Zooomr"))
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData(QString::fromLatin1("content-type"), QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

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

    QUrl url(m_apiUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
    urlQuery.addQueryItem(QString::fromLatin1("method"), method);
    urlQuery.addQueryItem(QString::fromLatin1("frob"), m_frob);

    for (QStringList::const_iterator it = argList.constBegin(); it != argList.constEnd(); ++it)
    {
        QStringList str = (*it).split(QLatin1Char('='), QString::SkipEmptyParts);
        urlQuery.addQueryItem(str[0], str[1]);
    }

    url.setQuery(urlQuery);
    QString md5 = getApiSig(m_secret, url);
    urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "Get photo property url: " << url;
    QByteArray tmp;
    KIO::TransferJob* job = 0;

    if (m_serviceName == QString::fromLatin1("Zooomr"))
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData(QString::fromLatin1("content-type"), QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_GETPHOTOPROPERTY;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);

    //  m_authProgressDlg->setLabelText("Getting the Token from the server");
    //  m_authProgressDlg->setProgress(3,4);
}

void FlickrTalker::listPhotos(const QString& /*albumName*/)
{
    // TODO
}

void FlickrTalker::createPhotoSet(const QString& /*albumName*/, const QString& albumTitle,
                                  const QString& albumDescription, const QString& primaryPhotoId)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    qCDebug(KIPIPLUGINS_LOG) << "create photoset invoked";
    QUrl url(m_apiUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("auth_token"), m_token);
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
    urlQuery.addQueryItem(QString::fromLatin1("method"), QString::fromLatin1("flickr.photosets.create"));
    urlQuery.addQueryItem(QString::fromLatin1("title"), albumTitle);
    urlQuery.addQueryItem(QString::fromLatin1("description"), albumDescription);
    urlQuery.addQueryItem(QString::fromLatin1("primary_photo_id"), primaryPhotoId);
    url.setQuery(urlQuery);

    QString md5 = getApiSig(m_secret, url);
    urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "List photo sets url: " << url;
    QByteArray tmp;
    KIO::TransferJob* job = 0;

    if (m_serviceName == QString::fromLatin1("Zooomr"))
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect (although this function should never get called when
        // using Zooomr).
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData(QString::fromLatin1("content-type"), QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));
    }


    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_CREATEPHOTOSET;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

void FlickrTalker::addPhotoToPhotoSet(const QString& photoId,
                                      const QString& photoSetId)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    qCDebug(KIPIPLUGINS_LOG) << "addPhotoToPhotoSet invoked";
    QUrl url(m_apiUrl);
    QUrlQuery urlQuery;

    /* If the photoset id starts with the special string "UNDEFINED_", it means
     * it doesn't exist yet on Flickr and needs to be created. Note that it's
     * not necessary to subsequently add the photo to the photo set, as this
     * is done in the set creation call to Flickr. */
    if (photoSetId.startsWith(QLatin1String("UNDEFINED_")))
    {
        createPhotoSet(QString::fromLatin1(""), m_selectedPhotoSet.title, m_selectedPhotoSet.description, photoId);
    }
    else
    {
        urlQuery.addQueryItem(QString::fromLatin1("auth_token"), m_token);
        urlQuery.addQueryItem(QString::fromLatin1("photoset_id"), photoSetId);
        urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);
        urlQuery.addQueryItem(QString::fromLatin1("method"), QString::fromLatin1("flickr.photosets.addPhoto"));
        urlQuery.addQueryItem(QString::fromLatin1("photo_id"), photoId);
        url.setQuery(urlQuery);

        QString md5 = getApiSig(m_secret, url);
        urlQuery.addQueryItem(QString::fromLatin1("api_sig"), md5);
        url.setQuery(urlQuery);

        QByteArray tmp;
        qCDebug(KIPIPLUGINS_LOG) << "Add photo to Photo set url: " << url;
        KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData(QString::fromLatin1("content-type"), QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));

        connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
                this, SLOT(slotData(KIO::Job*,QByteArray)));

        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));

        m_state = FE_ADDPHOTOTOPHOTOSET;
        m_job   = job;
        m_buffer.resize(0);
        emit signalBusy(true);
    }
}

bool FlickrTalker::addPhoto(const QString& photoPath, const FPhotoInfo& info,
                            bool rescale, int maxDim, int imageQuality)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QUrl    url(m_uploadUrl);

    // We dont' want to modify url as such, we just used the KURL object for storing the query items.
    QUrl  url2(QString::fromLatin1(""));
    QUrlQuery urlQuery;
    QString path = photoPath;
    MPForm  form;

    form.addPair(QString::fromLatin1("auth_token"), m_token, QString::fromLatin1("text/plain"));
    urlQuery.addQueryItem(QString::fromLatin1("auth_token"), m_token);

    form.addPair(QString::fromLatin1("api_key"), m_apikey, QString::fromLatin1("text/plain"));
    urlQuery.addQueryItem(QString::fromLatin1("api_key"), m_apikey);

    QString ispublic = (info.is_public == 1) ? QString::fromLatin1("1") : QString::fromLatin1("0");
    form.addPair(QString::fromLatin1("is_public"), ispublic, QString::fromLatin1("text/plain"));
    urlQuery.addQueryItem(QString::fromLatin1("is_public"), ispublic);

    QString isfamily = (info.is_family == 1) ? QString::fromLatin1("1") : QString::fromLatin1("0");
    form.addPair(QString::fromLatin1("is_family"), isfamily, QString::fromLatin1("text/plain"));
    urlQuery.addQueryItem(QString::fromLatin1("is_family"), isfamily);

    QString isfriend = (info.is_friend == 1) ? QString::fromLatin1("1") : QString::fromLatin1("0");
    form.addPair(QString::fromLatin1("is_friend"), isfriend, QString::fromLatin1("text/plain"));
    urlQuery.addQueryItem(QString::fromLatin1("is_friend"), isfriend);

    QString safetyLevel = QString::number(static_cast<int>(info.safety_level));
    form.addPair(QString::fromLatin1("safety_level"), safetyLevel, QString::fromLatin1("text/plain"));
    urlQuery.addQueryItem(QString::fromLatin1("safety_level"), safetyLevel);

    QString contentType = QString::number(static_cast<int>(info.content_type));
    form.addPair(QString::fromLatin1("content_type"), contentType, QString::fromLatin1("text/plain"));
    urlQuery.addQueryItem(QString::fromLatin1("content_type"), contentType);

    QString tags = QString::fromLatin1("\"") + info.tags.join(QString::fromLatin1("\" \"")) + QString::fromLatin1("\"");

    if (tags.length() > 0)
    {
        form.addPair(QString::fromLatin1("tags"), tags, QString::fromLatin1("text/plain"));
        urlQuery.addQueryItem(QString::fromLatin1("tags"), tags);
    }

    if (!info.title.isEmpty())
    {
        form.addPair(QString::fromLatin1("title"), info.title, QString::fromLatin1("text/plain"));
        urlQuery.addQueryItem(QString::fromLatin1("title"), info.title);
    }

    if (!info.description.isEmpty())
    {
        form.addPair(QString::fromLatin1("description"), info.description, QString::fromLatin1("text/plain"));
        urlQuery.addQueryItem(QString::fromLatin1("description"), info.description);
    }

    url2.setQuery(urlQuery);
    QString md5 = getApiSig(m_secret, url2);
    form.addPair(QString::fromLatin1("api_sig"), md5, QString::fromLatin1("text/plain"));
    QImage image;

    // Check if RAW file.

    if (m_iface)
    {
        QPointer<RawProcessor> rawdec = m_iface->createRawProcessor();

        // check if its a RAW file.
        if (rawdec && rawdec->isRawFile(QUrl::fromLocalFile(photoPath)))
        {
            rawdec->loadRawPreview(QUrl::fromLocalFile(photoPath), image);
        }
    }
    
    if (image.isNull())
    {
        image.load(photoPath);
    }

    if (!image.isNull())
    {
        path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QString::fromLatin1("/") + QFileInfo(photoPath).baseName().trimmed() + QString::fromLatin1(".jpg");

        if (rescale)
        {
            if (image.width() > maxDim || image.height() > maxDim)
                image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            image.save(path, "JPEG", imageQuality);
        }

        // Restore all metadata.

        if (m_iface)
        {
            QPointer<MetadataProcessor> meta = m_iface->createMetadataProcessor();

            if (meta && meta->load(QUrl::fromLocalFile(photoPath)))
            {
                meta->setImageDimensions(image.size());

                // NOTE: see bug #153207: Flickr use IPTC keywords to create Tags in web interface
                //       As IPTC do not support UTF-8, we need to remove it.
                meta->removeIptcTags(QStringList() << QLatin1String("Iptc.Application2.Keywords"));

                meta->setImageProgramId(QString::fromLatin1("Kipi-plugins"), kipipluginsVersion());
                meta->save(QUrl::fromLocalFile(path));
            }
            else
            {
                qCWarning(KIPIPLUGINS_LOG) << "flickrExport::Image doesn't have metadata";
            }
        }

        qCDebug(KIPIPLUGINS_LOG) << "Resizing and saving to temp file: " << path;
    }

    QFileInfo tempFileInfo(path);

    qCDebug(KIPIPLUGINS_LOG) << "QUrl path is " << QUrl::fromLocalFile(path) << "Image size after resizing (in bytes) is "<< tempFileInfo.size();

    if (tempFileInfo.size() > (getMaxAllowedFileSize().toLongLong()))
    {
        emit signalAddPhotoFailed(i18n("File Size exceeds maximum allowed file size."));
        return false;
    }

    if (!form.addFile(QString::fromLatin1("photo"), path))
    {
        return false;
    }

    form.finish();

    KIO::TransferJob* const job = KIO::http_post(url, form.formData(), KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"), form.contentType());

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotData(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    return true;
}

QString FlickrTalker::getUserName() const
{
    return m_username;
}

QString FlickrTalker::getUserId() const
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
    {
        m_authProgressDlg->hide();
    }
}

void FlickrTalker::slotData(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
    {
        return;
    }

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data() + oldSize, data.data(), data.size());
}

void FlickrTalker::slotError(const QString& error)
{
    QString transError;
    int errorNo = error.toInt();

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
            transError = i18n("Filetype was not recognized");
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

    QMessageBox::critical(QApplication::activeWindow(),
                          i18n("Error"),
                          i18n("Error Occurred: %1\nCannot proceed any further.", transError));
}

void FlickrTalker::slotResult(KJob* kjob)
{
    m_job               = 0;
    emit signalBusy(false);
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

    if (job->error())
    {
        if (m_state == FE_ADDPHOTO)
        {
            emit signalAddPhotoFailed(job->errorString());
        }
        else
        {
            KIO::JobUiDelegate* const job_ui = static_cast<KIO::JobUiDelegate*>(job->ui());
            KJobWidgets::setWindow(job, m_parent);
            job_ui->showErrorMessage();
        }

        return;
    }

    switch (m_state)
    {
        case (FE_LOGIN):
            //parseResponseLogin(m_buffer);
            break;

        case (FE_LISTPHOTOSETS):
            parseResponseListPhotoSets(m_buffer);
            break;

        case (FE_GETFROB):
            parseResponseGetFrob(m_buffer);
            break;

        case (FE_GETTOKEN):
            parseResponseGetToken(m_buffer);
            break;

        case (FE_CHECKTOKEN):
            parseResponseCheckToken(m_buffer);
            break;

        case (FE_GETAUTHORIZED):
            //parseResponseGetToken(m_buffer);
            break;

        case (FE_LISTPHOTOS):
            parseResponseListPhotos(m_buffer);
            break;

        case (FE_GETPHOTOPROPERTY):
            parseResponsePhotoProperty(m_buffer);
            break;

        case (FE_ADDPHOTO):
            parseResponseAddPhoto(m_buffer);
            break;

        case (FE_ADDPHOTOTOPHOTOSET):
            parseResponseAddPhotoToPhotoSet(m_buffer);
            break;

        case (FE_CREATEPHOTOSET):
            parseResponseCreatePhotoSet(m_buffer);
            break;

        case (FE_GETMAXSIZE):
            parseResponseMaxSize(m_buffer);
            break;

        default:  // FR_LOGOUT
            break;
    }
}

void FlickrTalker::parseResponseMaxSize(const QByteArray& data)
{
    QString errorString;
    QDomDocument doc(QString::fromLatin1("mydocument"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QString::fromLatin1("person"))
        {
            e                = node.toElement();
            QDomNode details = e.firstChild();

            while (!details.isNull())
            {
                if (details.isElement())
                {
                    e = details.toElement();

                    if (details.nodeName() == QString::fromLatin1("photos"))
                    {
                        QDomAttr a = e.attributeNode(QString::fromLatin1("maxupload"));
                        m_maxSize = a.value();
                        qCDebug(KIPIPLUGINS_LOG) << "Max upload size is"<<m_maxSize;
                    } 
                }

                details = details.nextSibling();
            }
        }

        if (node.isElement() && node.nodeName() == QString::fromLatin1("err"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Checking Error in response";
            errorString = node.toElement().attribute(QString::fromLatin1("code"));
            qCDebug(KIPIPLUGINS_LOG) << "Error code=" << errorString;
            qCDebug(KIPIPLUGINS_LOG) << "Msg=" << node.toElement().attribute(QString::fromLatin1("msg"));
        }

        node = node.nextSibling();
    }
}

void FlickrTalker::parseResponseGetFrob(const QByteArray& data)
{
    bool success = false;
    QString errorString;
    QDomDocument doc(QString::fromLatin1("mydocument"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QString::fromLatin1("frob"))
        {
            QDomElement e = node.toElement();    // try to convert the node to an element.
            qCDebug(KIPIPLUGINS_LOG) << "Frob is" << e.text();
            m_frob        = e.text();            // this is what is obtained from data.
            success       = true;
        }

        if (node.isElement() && node.nodeName() == QString::fromLatin1("err"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Checking Error in response";
            errorString = node.toElement().attribute(QString::fromLatin1("code"));
            qCDebug(KIPIPLUGINS_LOG) << "Error code=" << errorString;
            qCDebug(KIPIPLUGINS_LOG) << "Msg=" << node.toElement().attribute(QString::fromLatin1("msg"));
        }

        node = node.nextSibling();
    }

    qCDebug(KIPIPLUGINS_LOG) << "GetFrob finished";
    m_authProgressDlg->setMaximum(4);
    m_authProgressDlg->setValue(2);
    m_state = FE_GETAUTHORIZED;

    if (success)
    {
        emit signalAuthenticate();
    }
    else
    {
        emit signalError(errorString);
    }
}

void FlickrTalker::parseResponseCheckToken(const QByteArray& data)
{
    bool         success = false;
    QString      errorString;
    QString      username;
    QString      transReturn;
    QDomDocument doc(QString::fromLatin1("checktoken"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QString::fromLatin1("auth"))
        {
            e                = node.toElement(); // try to convert the node to an element.
            QDomNode details = e.firstChild();

            while (!details.isNull())
            {
                if (details.isElement())
                {
                    e = details.toElement();

                    if (details.nodeName() == QString::fromLatin1("token"))
                    {
                        qCDebug(KIPIPLUGINS_LOG) << "Token=" << e.text();
                        m_token = e.text();//this is what is obtained from data.
                    }

                    if (details.nodeName() == QString::fromLatin1("perms"))
                    {
                        qCDebug(KIPIPLUGINS_LOG) << "Perms=" << e.text();
                        QString perms = e.text();//this is what is obtained from data.

                        if (perms == QString::fromLatin1("write"))
                        {
                            transReturn = i18nc("As in the permission to", "write");
                        }
                        else if (perms == QString::fromLatin1("read"))
                        {
                            transReturn = i18nc("As in the permission to", "read");
                        }
                        else if (perms == QString::fromLatin1("delete"))
                        {
                            transReturn = i18nc("As in the permission to", "delete");
                        }
                    }

                    if (details.nodeName() == QString::fromLatin1("user"))
                    {
                        qCDebug(KIPIPLUGINS_LOG) << "nsid=" << e.attribute(QString::fromLatin1("nsid"));
                        m_userId   = e.attribute(QString::fromLatin1("nsid"));
                        username   = e.attribute(QString::fromLatin1("username"));
                        m_username = username;
                        qCDebug(KIPIPLUGINS_LOG) << "username=" << e.attribute(QString::fromLatin1("username"));
                        qCDebug(KIPIPLUGINS_LOG) << "fullname=" << e.attribute(QString::fromLatin1("fullname"));
                    }
                }

                details = details.nextSibling();
            }

            m_authProgressDlg->hide();
            emit signalTokenObtained(m_token);
            success = true;
        }

        if (node.isElement() && node.nodeName() == QString::fromLatin1("err"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Checking Error in response";
            errorString = node.toElement().attribute(QString::fromLatin1("code"));
            qCDebug(KIPIPLUGINS_LOG) << "Error code=" << errorString;
            qCDebug(KIPIPLUGINS_LOG) << "Msg=" << node.toElement().attribute(QString::fromLatin1("msg"));

            int valueOk = QMessageBox::question(QApplication::activeWindow(),
                                                i18n("Invalid Token"),
                                                i18n("Your token is invalid. Would you like to "
                                                      "get a new token to proceed?\n"));

            if (valueOk == QMessageBox::Yes)
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

    if (!success)
    {
        emit signalError(errorString);
    }

    qCDebug(KIPIPLUGINS_LOG) << "CheckToken finished";
}

void FlickrTalker::parseResponseGetToken(const QByteArray& data)
{
    bool success = false;
    QString errorString;
    QDomDocument doc(QString::fromLatin1("gettoken"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode    node    = docElem.firstChild();
    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QString::fromLatin1("auth"))
        {
            e                = node.toElement(); // try to convert the node to an element.
            QDomNode details = e.firstChild();

            while (!details.isNull())
            {
                if (details.isElement())
                {
                    e = details.toElement();

                    if (details.nodeName() == QString::fromLatin1("token"))
                    {
                        qCDebug(KIPIPLUGINS_LOG) << "Token=" << e.text();
                        m_token = e.text();      //this is what is obtained from data.
                    }

                    if (details.nodeName() == QString::fromLatin1("perms"))
                    {
                        qCDebug(KIPIPLUGINS_LOG) << "Perms=" << e.text();
                    }

                    if (details.nodeName() == QString::fromLatin1("user"))
                    {
                        qCDebug(KIPIPLUGINS_LOG) << "nsid=" << e.attribute(QString::fromLatin1("nsid"));
                        qCDebug(KIPIPLUGINS_LOG) << "username=" << e.attribute(QString::fromLatin1("username"));
                        qCDebug(KIPIPLUGINS_LOG) << "fullname=" << e.attribute(QString::fromLatin1("fullname"));
                        m_username = e.attribute(QString::fromLatin1("username"));
                        m_userId   = e.attribute(QString::fromLatin1("nsid"));
                    }
                }

                details = details.nextSibling();
            }

            success = true;
        }
        else if (node.isElement() && node.nodeName() == QString::fromLatin1("err"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Checking Error in response";
            errorString = node.toElement().attribute(QString::fromLatin1("code"));
            qCDebug(KIPIPLUGINS_LOG) << "Error code=" << errorString;
            qCDebug(KIPIPLUGINS_LOG) << "Msg=" << node.toElement().attribute(QString::fromLatin1("msg"));
            //emit signalError(code);
        }

        node = node.nextSibling();
    }

    qCDebug(KIPIPLUGINS_LOG) << "GetToken finished";
    //emit signalBusy( false );
    m_authProgressDlg->hide();

    if (success)
    {
        emit signalTokenObtained(m_token);
    }
    else
    {
        emit signalError(errorString);
    }
}

void FlickrTalker::parseResponseCreatePhotoSet(const QByteArray& data)
{
    qCDebug(KIPIPLUGINS_LOG) << "Parse response create photoset received " << data;

    //bool success = false;

    QDomDocument doc(QString::fromLatin1("getListPhotoSets"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode    node    = docElem.firstChild();
    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QString::fromLatin1("photoset"))
        {
            // Parse the id from the response.
            QString new_id = node.toElement().attribute(QString::fromLatin1("id"));

            // Set the new id in the photo sets list.
            QLinkedList<FPhotoSet>::iterator it = m_photoSetsList->begin();

            while (it != m_photoSetsList->end())
            {
                if (it->id == m_selectedPhotoSet.id)
                {
                    it->id = new_id;
                    break;
                }

                ++it;
            }

            // Set the new id in the selected photo set.
            m_selectedPhotoSet.id = new_id;

            qCDebug(KIPIPLUGINS_LOG) << "PhotoSet created successfully with id" << new_id;
            emit signalAddPhotoSetSucceeded();
        }

        if (node.isElement() && node.nodeName() == QString::fromLatin1("err"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Checking Error in response";
            QString code = node.toElement().attribute(QString::fromLatin1("code"));
            qCDebug(KIPIPLUGINS_LOG) << "Error code=" << code;
            QString msg = node.toElement().attribute(QString::fromLatin1("msg"));
            qCDebug(KIPIPLUGINS_LOG) << "Msg=" << msg;
            QMessageBox::critical(QApplication::activeWindow(), i18n("Error"), i18n("PhotoSet creation failed: ") + msg);
        }

        node = node.nextSibling();
    }
}

void FlickrTalker::parseResponseListPhotoSets(const QByteArray& data)
{
    qCDebug(KIPIPLUGINS_LOG) << "parseResponseListPhotosets" << data;
    bool success = false;
    QDomDocument doc(QString::fromLatin1("getListPhotoSets"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode    node    = docElem.firstChild();
    QDomElement e;

    QString photoSet_id, photoSet_title, photoSet_description;
    m_photoSetsList = new QLinkedList <FPhotoSet> ();

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QString::fromLatin1("photosets"))
        {
            e                    = node.toElement();
            QDomNode details     = e.firstChild();
            FPhotoSet fps;
            QDomNode detailsNode = details;

            while (!detailsNode.isNull())
            {
                if (detailsNode.isElement())
                {
                    e = detailsNode.toElement();

                    if (detailsNode.nodeName() == QString::fromLatin1("photoset"))
                    {
                        qCDebug(KIPIPLUGINS_LOG) << "id=" << e.attribute(QString::fromLatin1("id"));
                        photoSet_id              = e.attribute(QString::fromLatin1("id"));     // this is what is obtained from data.
                        fps.id                   = photoSet_id;
                        QDomNode photoSetDetails = detailsNode.firstChild();
                        QDomElement e_detail;

                        while (!photoSetDetails.isNull())
                        {
                            e_detail = photoSetDetails.toElement();

                            if (photoSetDetails.nodeName() == QString::fromLatin1("title"))
                            {
                                qCDebug(KIPIPLUGINS_LOG) << "Title=" << e_detail.text();
                                photoSet_title = e_detail.text();
                                fps.title      = photoSet_title;
                            }
                            else if (photoSetDetails.nodeName() == QString::fromLatin1("description"))
                            {
                                qCDebug(KIPIPLUGINS_LOG) << "Description =" << e_detail.text();
                                photoSet_description = e_detail.text();
                                fps.description      = photoSet_description;
                            }

                            photoSetDetails = photoSetDetails.nextSibling();
                        }

                        m_photoSetsList->append(fps);
                    }
                }

                detailsNode = detailsNode.nextSibling();
            }

            details = details.nextSibling();
            success = true;
        }

        if (node.isElement() && node.nodeName() == QString::fromLatin1("err"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Checking Error in response";
            QString code = node.toElement().attribute(QString::fromLatin1("code"));
            qCDebug(KIPIPLUGINS_LOG) << "Error code=" << code;
            qCDebug(KIPIPLUGINS_LOG) << "Msg=" << node.toElement().attribute(QString::fromLatin1("msg"));
            emit signalError(code);
        }

        node = node.nextSibling();
    }

    qCDebug(KIPIPLUGINS_LOG) << "GetPhotoList finished";

    if (!success)
    {
        emit signalListPhotoSetsFailed(i18n("Failed to fetch list of photo sets."));
    }
    else
    {
        emit signalListPhotoSetsSucceeded();
        maxAllowedFileSize();
    }
}

void FlickrTalker::parseResponseListPhotos(const QByteArray& data)
{
    QDomDocument doc(QString::fromLatin1("getPhotosList"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    //QDomElement e;
    //TODO
}

void FlickrTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    QDomDocument doc(QString::fromLatin1("getCreateAlbum"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    //TODO
}

void FlickrTalker::parseResponseAddPhoto(const QByteArray& data)
{
    bool    success = false;
    QString line;
    QDomDocument doc(QString::fromLatin1("AddPhoto Response"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;
    QString photoId;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QString::fromLatin1("photoid"))
        {
            e                = node.toElement();           // try to convert the node to an element.
            QDomNode details = e.firstChild();
            photoId          = e.text();
            qCDebug(KIPIPLUGINS_LOG) << "Photoid= " << photoId;
            success          = true;
        }

        if (node.isElement() && node.nodeName() == QString::fromLatin1("err"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Checking Error in response";
            QString code = node.toElement().attribute(QString::fromLatin1("code"));
            qCDebug(KIPIPLUGINS_LOG) << "Error code=" << code;
            qCDebug(KIPIPLUGINS_LOG) << "Msg=" << node.toElement().attribute(QString::fromLatin1("msg"));
            emit signalError(code);
        }

        node = node.nextSibling();
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        QString photoSetId = m_selectedPhotoSet.id;

        if (photoSetId == QString::fromLatin1("-1"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "PhotoSet Id not set, not adding the photo to any photoset";
            emit signalAddPhotoSucceeded();
        }
        else
        {
            if (m_serviceName == QString::fromLatin1("Zooomr"))
            {
                // addPhotoToPhotoSet not supported by Zooomr (Zooomr only has
                // smart folder-type photosets); silently fail
                emit signalAddPhotoSucceeded();
            }
            else
            {
                addPhotoToPhotoSet(photoId, photoSetId);
            }
        }
    }
}

void FlickrTalker::parseResponsePhotoProperty(const QByteArray& data)
{
    bool         success = false;
    QString      line;
    QDomDocument doc(QString::fromLatin1("Photos Properties"));

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode    node    = docElem.firstChild();
    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QString::fromLatin1("photoid"))
        {
            e                = node.toElement();                 // try to convert the node to an element.
            QDomNode details = e.firstChild();
            success          = true;
            qCDebug(KIPIPLUGINS_LOG) << "Photoid=" << e.text();
        }

        if (node.isElement() && node.nodeName() == QString::fromLatin1("err"))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Checking Error in response";
            QString code = node.toElement().attribute(QString::fromLatin1("code"));
            qCDebug(KIPIPLUGINS_LOG) << "Error code=" << code;
            qCDebug(KIPIPLUGINS_LOG) << "Msg=" << node.toElement().attribute(QString::fromLatin1("msg"));
            emit signalError(code);
        }

        node = node.nextSibling();
    }

    qCDebug(KIPIPLUGINS_LOG) << "GetToken finished";

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to query photo information"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }
}

void FlickrTalker::parseResponseAddPhotoToPhotoSet(const QByteArray& data)
{
    qCDebug(KIPIPLUGINS_LOG) << "parseResponseListPhotosets" << data;
    emit signalAddPhotoSucceeded();
}

} // namespace KIPIFlickrPlugin
