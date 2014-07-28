/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2009 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2009-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "flickrtalker.moc"

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

// KDE includes

#include <kapplication.h>
#include <kcodecs.h>
#include <kdebug.h>
#include <kio/jobuidelegate.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kguiitem.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "kpversion.h"
#include "kpmetadata.h"
#include "mpform.h"
#include "flickritem.h"
#include "flickrwindow.h"

using namespace KIPIPlugins;

namespace KIPIFlickrExportPlugin
{

FlickrTalker::FlickrTalker(QWidget* const parent, const QString& serviceName)
{
    m_parent          = parent;
    m_job             = 0;
    m_photoSetsList   = 0;
    m_authProgressDlg = 0;
    m_state           = FE_LOGOUT;

    m_serviceName = serviceName;

    if (serviceName == QString("23"))
    {
        m_apiUrl    = QString("http://www.23hq.com/services/rest/");
        m_authUrl   = QString("http://www.23hq.com/services/auth/");
        m_uploadUrl = QString("http://www.23hq.com/services/upload/");

        // bshanks: do 23 and flickr really share API keys? or does 23 not need
        // one?
        m_apikey    = "49d585bafa0758cb5c58ab67198bf632";
        m_secret    = "34b39925e6273ffd";
    }
    else if (serviceName == QString("Zooomr"))
    {
        m_apiUrl    = QString("http://api.zooomr.com/services/rest/");
        m_authUrl   = QString("http://www.zooomr.com/services/auth/");
        m_uploadUrl = QString("http://upload.zooomr.com/services/upload/");

        m_apikey    = "18c8db5ce9ed4e15a7b484136f5080c5";
        m_secret    = "1ea4af14e3";
    }
    else
    {
        m_apiUrl    = QString("https://www.flickr.com/services/rest/");
        m_authUrl   = QString("https://www.flickr.com/services/auth/");
        m_uploadUrl = QString("https://api.flickr.com/services/upload/");

        m_apikey    = "49d585bafa0758cb5c58ab67198bf632";
        m_secret    = "34b39925e6273ffd";
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
QString FlickrTalker::getApiSig(const QString& secret, const KUrl& url)
{
    QMap<QString, QString> queries = url.queryItems();
    QString compressed(secret);

    // NOTE: iterator QMap iterator will sort alphabetically items based on key values.
    for (QMap<QString, QString>::iterator it = queries.begin() ; it != queries.end(); ++it)
    {
        compressed.append(it.key());
        compressed.append(it.value());
    }

    KMD5 context(compressed.toUtf8());
    return context.hexDigest().data();
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

    KMD5 context(compressed.toUtf8());
    return context.hexDigest().data();
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

    KUrl url(m_apiUrl);
    url.addQueryItem("method", "flickr.auth.getFrob");
    url.addQueryItem("api_key", m_apikey);
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kDebug() << "Get frob url: " << url;

    KIO::TransferJob* job = 0;

    if (m_serviceName == "Zooomr")
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        QByteArray tmp;
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    KUrl url(m_apiUrl);
    url.addQueryItem("method", "flickr.auth.checkToken");
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("auth_token", token);
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kDebug() << "Check token url: " << url;
    QByteArray tmp;

    KIO::TransferJob* job = 0;

    if (m_serviceName == "Zooomr")
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow the
        // redirect
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    KUrl url(m_authUrl);
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("frob", m_frob);
    url.addQueryItem("perms", "write");
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kDebug() << "Authenticate url: " << url;

    KToolInvocation::invokeBrowser(url.url());
    int valueOk = KMessageBox::questionYesNo(kapp->activeWindow(),
                                             i18n("Please follow the instructions in the browser window, then "
                                                  "return to press corresponding button."),
                                             i18n("%1 Service Web Authorization", m_serviceName),
                                             KGuiItem(i18n("I am authenticated"), "dialog-ok"),
                                             KGuiItem(i18n("I am not authenticated"), "dialog-cancel"));

    if (valueOk == KMessageBox::Yes)
    {
        getToken();
        m_authProgressDlg->setLabelText(i18n("Authenticating the User on web"));
        m_authProgressDlg->setMaximum(4);
        m_authProgressDlg->setValue(2);
        emit signalBusy(false);
    }
    else
    {
        kDebug() << "User didn't proceed with getToken Authorization, cannot proceed further, aborting";
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

    KUrl url(m_apiUrl);
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("method", "flickr.auth.getToken");
    url.addQueryItem("frob", m_frob);
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kDebug() << "Get token url: " << url;

    KIO::TransferJob* job = 0;

    if (m_serviceName == "Zooomr")
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        QByteArray tmp;
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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
    kDebug() << "List photoset invoked";
    KUrl url(m_apiUrl);
    url.addQueryItem("auth_token", m_token);
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("method", "flickr.photosets.getList");
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kDebug() << "List photoset URL" << url;
    QByteArray tmp;
    KIO::TransferJob* job = 0;

    if (m_serviceName == "Zooomr")
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    KUrl url(m_apiUrl);
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("method", method);
    url.addQueryItem("frob", m_frob);

    for (QStringList::const_iterator it = argList.constBegin(); it != argList.constEnd(); ++it)
    {
        QStringList str = (*it).split('=', QString::SkipEmptyParts);
        url.addQueryItem(str[0], str[1]);
    }

    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kDebug() << "Get photo property url: " << url;
    QByteArray tmp;
    KIO::TransferJob* job = 0;

    if (m_serviceName == "Zooomr")
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect.
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    kDebug() << "create photoset invoked";
    KUrl url(m_apiUrl);
    url.addQueryItem("auth_token", m_token);
    url.addQueryItem("api_key", m_apikey);
    url.addQueryItem("method", "flickr.photosets.create");
    url.addQueryItem("title", albumTitle);
    url.addQueryItem("description", albumDescription);
    url.addQueryItem("primary_photo_id", primaryPhotoId);
    QString md5 = getApiSig(m_secret, url);
    url.addQueryItem("api_sig", md5);
    kDebug() << "List photo sets url: " << url;
    QByteArray tmp;
    KIO::TransferJob* job = 0;

    if (m_serviceName == "Zooomr")
    {
        // Zooomr redirects the POST at this url to a GET; KIO doesn't follow
        // the redirect (although this function should never get called when
        // using Zooomr).
        job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    }
    else
    {
        job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
    }


    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

    kDebug() << "addPhotoToPhotoSet invoked";
    KUrl url(m_apiUrl);

    /* If the photoset id starts with the special string "UNDEFINED_", it means
     * it doesn't exist yet on Flickr and needs to be created. Note that it's
     * not necessary to subsequently add the photo to the photo set, as this
     * is done in the set creation call to Flickr. */
    if (photoSetId.startsWith(QLatin1String("UNDEFINED_")))
    {
        createPhotoSet("", m_selectedPhotoSet.title, m_selectedPhotoSet.description, photoId);
    }
    else
    {
        url.addQueryItem("auth_token", m_token);

        url.addQueryItem("photoset_id", photoSetId);

        url.addQueryItem("api_key", m_apikey);

        url.addQueryItem("method", "flickr.photosets.addPhoto");

        url.addQueryItem("photo_id", photoId);

        QString md5 = getApiSig(m_secret, url);
        url.addQueryItem("api_sig", md5);

        QByteArray tmp;
        kDebug() << "Add photo to Photo set url: " << url;
        KIO::TransferJob* job = KIO::http_post(url, tmp, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");

        connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
                this, SLOT(data(KIO::Job*,QByteArray)));

        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));

        m_state = FE_ADDPHOTOTOPHOTOSET;
        m_job   = job;
        m_buffer.resize(0);
        emit signalBusy(true);
    }
}

bool FlickrTalker::addPhoto(const QString& photoPath, const FPhotoInfo& info,
                            bool sendOriginal, bool rescale, int maxDim, int imageQuality)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KUrl    url(m_uploadUrl);

    // We dont' want to modify url as such, we just used the KURL object for storing the query items.
    KUrl  url2("");
    QString path = photoPath;
    MPForm  form;

    form.addPair("auth_token", m_token, "text/plain");
    url2.addQueryItem("auth_token", m_token);

    form.addPair("api_key", m_apikey, "text/plain");
    url2.addQueryItem("api_key", m_apikey);

    QString ispublic = (info.is_public == 1) ? "1" : "0";
    form.addPair("is_public", ispublic, "text/plain");
    url2.addQueryItem("is_public", ispublic);

    QString isfamily = (info.is_family == 1) ? "1" : "0";
    form.addPair("is_family", isfamily, "text/plain");
    url2.addQueryItem("is_family", isfamily);

    QString isfriend = (info.is_friend == 1) ? "1" : "0";
    form.addPair("is_friend", isfriend, "text/plain");
    url2.addQueryItem("is_friend", isfriend);

    QString safetyLevel = QString::number(static_cast<int>(info.safety_level));
    form.addPair("safety_level", safetyLevel, "text/plain");
    url2.addQueryItem("safety_level", safetyLevel);

    QString contentType = QString::number(static_cast<int>(info.content_type));
    form.addPair("content_type", contentType, "text/plain");
    url2.addQueryItem("content_type", contentType);

    QString tags = "\"" + info.tags.join("\" \"") + "\"";

    if (tags.length() > 0)
    {
        form.addPair("tags", tags, "text/plain");
        url2.addQueryItem("tags", tags);
    }

    if (!info.title.isEmpty())
    {
        form.addPair("title", info.title, "text/plain");
        url2.addQueryItem("title", info.title);
    }

    if (!info.description.isEmpty())
    {
        form.addPair("description", info.description, "text/plain");
        url2.addQueryItem("description", info.description);
    }

    QString md5 = getApiSig(m_secret, url2);
    form.addPair("api_sig", md5, "text/plain");
    QImage image;

    // Check if RAW file.
    if (KPMetadata::isRawFile(photoPath))
    {
        KDcrawIface::KDcraw::loadRawPreview(image, photoPath);
    }
    else
    {
        image.load(photoPath);
    }

    if (!image.isNull())
    {
        path = KStandardDirs::locateLocal("tmp", QFileInfo(photoPath).baseName().trimmed() + ".jpg");

        if (sendOriginal)
        {
            QFile imgFile(photoPath);

            if (!imgFile.copy(path))
                return false;
        }
        else
        {
            if (rescale && (image.width() > maxDim || image.height() > maxDim))
                image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            image.save(path, "JPEG", imageQuality);
        }

        // Restore all metadata.

        KPMetadata meta;

        if (meta.load(photoPath))
        {
            meta.setImageDimensions(image.size());

            // NOTE: see B.K.O #153207: Flickr use IPTC keywords to create Tags in web interface
            //       As IPTC do not support UTF-8, we need to remove it.
            meta.removeIptcTag("Iptc.Application2.Keywords", false);

            meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            meta.save(path);
        }
        else
        {
            kWarning() << "(flickrExport::Image doesn't have metadata)";
        }

        kDebug() << "Resizing and saving to temp file: " << path;
    }

    if (!form.addFile("photo", path))
    {
        return false;
    }

    form.finish();

    KIO::TransferJob* const job = KIO::http_post(url, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("content-type", form.contentType());

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

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

void FlickrTalker::data(KIO::Job*, const QByteArray& data)
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

    KMessageBox::error(kapp->activeWindow(),
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
            job->ui()->setWindow(m_parent);
            job->ui()->showErrorMessage();
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

        default:  // FR_LOGOUT
            break;
    }
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

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "frob")
        {
            QDomElement e = node.toElement();    // try to convert the node to an element.
            kDebug() << "Frob is" << e.text();
            m_frob        = e.text();            // this is what is obtained from data.
            success       = true;
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kDebug() << "Checking Error in response";
            errorString = node.toElement().attribute("code");
            kDebug() << "Error code=" << errorString;
            kDebug() << "Msg=" << node.toElement().attribute("msg");
        }

        node = node.nextSibling();
    }

    kDebug() << "GetFrob finished";
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
    QDomDocument doc("checktoken");

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();
    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "auth")
        {
            e                = node.toElement(); // try to convert the node to an element.
            QDomNode details = e.firstChild();

            while (!details.isNull())
            {
                if (details.isElement())
                {
                    e = details.toElement();

                    if (details.nodeName() == "token")
                    {
                        kDebug() << "Token=" << e.text();
                        m_token = e.text();//this is what is obtained from data.
                    }

                    if (details.nodeName() == "perms")
                    {
                        kDebug() << "Perms=" << e.text();
                        QString perms = e.text();//this is what is obtained from data.

                        if (perms == "write")
                        {
                            transReturn = i18nc("As in the permission to", "write");
                        }
                        else if (perms == "read")
                        {
                            transReturn = i18nc("As in the permission to", "read");
                        }
                        else if (perms == "delete")
                        {
                            transReturn = i18nc("As in the permission to", "delete");
                        }
                    }

                    if (details.nodeName() == "user")
                    {
                        kDebug() << "nsid=" << e.attribute("nsid");
                        m_userId   = e.attribute("nsid");
                        username   = e.attribute("username");
                        m_username = username;
                        kDebug() << "username=" << e.attribute("username");
                        kDebug() << "fullname=" << e.attribute("fullname");
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
            kDebug() << "Checking Error in response";
            errorString = node.toElement().attribute("code");
            kDebug() << "Error code=" << errorString;
            kDebug() << "Msg=" << node.toElement().attribute("msg");

            int valueOk = KMessageBox::questionYesNo(kapp->activeWindow(),
                                                     i18n("Your token is invalid. Would you like to "
                                                          "get a new token to proceed?\n"));

            if (valueOk == KMessageBox::Yes)
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

    kDebug() << "CheckToken finished";
}

void FlickrTalker::parseResponseGetToken(const QByteArray& data)
{
    bool success = false;
    QString errorString;
    QDomDocument doc("gettoken");

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode    node    = docElem.firstChild();
    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "auth")
        {
            e                = node.toElement(); // try to convert the node to an element.
            QDomNode details = e.firstChild();

            while (!details.isNull())
            {
                if (details.isElement())
                {
                    e = details.toElement();

                    if (details.nodeName() == "token")
                    {
                        kDebug() << "Token=" << e.text();
                        m_token = e.text();      //this is what is obtained from data.
                    }

                    if (details.nodeName() == "perms")
                    {
                        kDebug() << "Perms=" << e.text();
                    }

                    if (details.nodeName() == "user")
                    {
                        kDebug() << "nsid=" << e.attribute("nsid");
                        kDebug() << "username=" << e.attribute("username");
                        kDebug() << "fullname=" << e.attribute("fullname");
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
            kDebug() << "Checking Error in response";
            errorString = node.toElement().attribute("code");
            kDebug() << "Error code=" << errorString;
            kDebug() << "Msg=" << node.toElement().attribute("msg");
            //emit signalError(code);
        }

        node = node.nextSibling();
    }

    kDebug() << "GetToken finished";
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
    kDebug() << "Parse response create photoset received " << data;

    //bool success = false;

    QDomDocument doc("getListPhotoSets");

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode    node    = docElem.firstChild();
    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "photoset")
        {
            // Parse the id from the response.
            QString new_id = node.toElement().attribute("id");

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

            kDebug() << "PhotoSet created successfully with id" << new_id;
            emit signalAddPhotoSetSucceeded();
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kDebug() << "Checking Error in response";
            QString code = node.toElement().attribute("code");
            kDebug() << "Error code=" << code;
            QString msg = node.toElement().attribute("msg");
            kDebug() << "Msg=" << msg;
            KMessageBox::error(kapp->activeWindow(), i18n("PhotoSet creation failed: ") + msg);
        }

        node = node.nextSibling();
    }
}

void FlickrTalker::parseResponseListPhotoSets(const QByteArray& data)
{
    kDebug() << "parseResponseListPhotosets" << data;
    bool success = false;
    QDomDocument doc("getListPhotoSets");

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
        if (node.isElement() && node.nodeName() == "photosets")
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

                    if (detailsNode.nodeName() == "photoset")
                    {
                        kDebug() << "id=" << e.attribute("id");
                        photoSet_id              = e.attribute("id");     // this is what is obtained from data.
                        fps.id                   = photoSet_id;
                        QDomNode photoSetDetails = detailsNode.firstChild();
                        QDomElement e_detail;

                        while (!photoSetDetails.isNull())
                        {
                            e_detail = photoSetDetails.toElement();

                            if (photoSetDetails.nodeName() == "title")
                            {
                                kDebug() << "Title=" << e_detail.text();
                                photoSet_title = e_detail.text();
                                fps.title      = photoSet_title;
                            }
                            else if (photoSetDetails.nodeName() == "description")
                            {
                                kDebug() << "Description =" << e_detail.text();
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

        if (node.isElement() && node.nodeName() == "err")
        {
            kDebug() << "Checking Error in response";
            QString code = node.toElement().attribute("code");
            kDebug() << "Error code=" << code;
            kDebug() << "Msg=" << node.toElement().attribute("msg");
            emit signalError(code);
        }

        node = node.nextSibling();
    }

    kDebug() << "GetPhotoList finished";

    if (!success)
    {
        emit signalListPhotoSetsFailed(i18n("Failed to fetch list of photo sets."));
    }
    else
    {
        emit signalListPhotoSetsSucceeded();
    }
}

void FlickrTalker::parseResponseListPhotos(const QByteArray& data)
{
    QDomDocument doc("getPhotosList");

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
    QDomDocument doc("getCreateAlbum");

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
    QDomDocument doc("AddPhoto Response");

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
        if (node.isElement() && node.nodeName() == "photoid")
        {
            e                = node.toElement();           // try to convert the node to an element.
            QDomNode details = e.firstChild();
            photoId          = e.text();
            kDebug() << "Photoid= " << photoId;
            success          = true;
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kDebug() << "Checking Error in response";
            QString code = node.toElement().attribute("code");
            kDebug() << "Error code=" << code;
            kDebug() << "Msg=" << node.toElement().attribute("msg");
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

        if (photoSetId == "-1")
        {
            kDebug() << "PhotoSet Id not set, not adding the photo to any photoset";
            emit signalAddPhotoSucceeded();
        }
        else
        {
            if (m_serviceName == "Zooomr")
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
    QDomDocument doc("Photos Properties");

    if (!doc.setContent(data))
    {
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode    node    = docElem.firstChild();
    QDomElement e;

    while (!node.isNull())
    {
        if (node.isElement() && node.nodeName() == "photoid")
        {
            e       = node.toElement();                 // try to convert the node to an element.
            QDomNode details = e.firstChild();
            kDebug() << "Photoid=" << e.text();
            success = true;
        }

        if (node.isElement() && node.nodeName() == "err")
        {
            kDebug() << "Checking Error in response";
            QString code = node.toElement().attribute("code");
            kDebug() << "Error code=" << code;
            kDebug() << "Msg=" << node.toElement().attribute("msg");
            emit signalError(code);
        }

        node = node.nextSibling();
    }

    kDebug() << "GetToken finished";

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
    kDebug() << "parseResponseListPhotosets" << data;
    emit signalAddPhotoSucceeded();
}

} // namespace KIPIFlickrExportPlugin
