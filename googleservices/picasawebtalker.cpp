/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
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

#include "picasawebtalker.h"

// C++ includes

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

// Qt includes

#include <QTextDocument>
#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QLinkedList>
#include <QStringList>
#include <QUrl>
#include <QPointer>
#include <QtAlgorithms>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QUrlQuery>

// KDE includes

#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/jobuidelegate.h>
#include <klocalizedstring.h>

// LibKDcraw includes

#include <KDCRAW/KDcraw>

// Local includes

#include "kpversion.h"
#include "kpmetadata.h"
#include "mpform_picasa.h"
#include "kipiplugins_debug.h"

namespace KIPIGoogleServicesPlugin
{

static bool picasaLessThan(GSFolder& p1, GSFolder& p2)
{
    return (p1.title.toLower() < p2.title.toLower());
}

PicasawebTalker::PicasawebTalker(QWidget* const parent)
    : Authorize(parent, QStringLiteral("https://picasaweb.google.com/data/"))
    , m_job(0)
    , m_state(FE_LOGOUT)
{
    connect(this, SIGNAL(signalError(QString)),
            this, SLOT(slotError(QString)));
}

PicasawebTalker::~PicasawebTalker()
{
    if (m_job)
        m_job->kill();
}

/** PicasaWeb's Album listing request/response
  * First a request is sent to the url below and then we might(?) get a redirect URL
  * WE then need to send the GET request to the Redirect url (this however gets taken care off by the
  * KIO libraries.
  * This uses the authenticated album list fetching to get all the albums included the unlisted-albums
  * which is not returned for an unauthorised request as done without the Authorization header.
*/
void PicasawebTalker::listAlbums()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    QUrl url(QStringLiteral("https://picasaweb.google.com/data/feed/api/user/default"));
    KIO::TransferJob* const job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    KIO::JobUiDelegate* const job_ui = static_cast<KIO::JobUiDelegate*>(job->ui());
    job_ui->setWindow(m_parent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/json"));

    if (!m_access_token.isEmpty())
    {
        QString auth_string = QStringLiteral("Authorization: ") + m_bearer_access_token;
        job->addMetaData(QStringLiteral("customHTTPHeader"), auth_string);
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_LISTALBUMS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void PicasawebTalker::listPhotos(const QString& albumId,
                                 const QString& imgmax)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QUrl url(QStringLiteral("https://picasaweb.google.com/data/feed/api/user/default/albumid/") + albumId);

    QUrlQuery q(url);
    q.addQueryItem(QStringLiteral("thumbsize"), QStringLiteral("200"));

    if (!imgmax.isNull())
    {
        q.addQueryItem(QStringLiteral("imgmax"), imgmax);
    }

    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    KIO::JobUiDelegate* const job_ui = static_cast<KIO::JobUiDelegate*>(job->ui());
    job_ui->setWindow(m_parent);
    job->addMetaData(QStringLiteral("content-type"),
                     QStringLiteral("Content-Type: application/x-www-form-urlencoded"));

    if (!m_access_token.isEmpty())
    {
        QString auth_string = QStringLiteral("Authorization: ") + m_bearer_access_token;
        job->addMetaData(QStringLiteral("customHTTPHeader"), auth_string);
    }

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_LISTPHOTOS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void PicasawebTalker::createAlbum(const GSFolder& album)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    //Create the Body in atom-xml
    QDomDocument docMeta;
    QDomProcessingInstruction instr = docMeta.createProcessingInstruction(
        QStringLiteral("xml"), QStringLiteral("version='1.0' encoding='UTF-8'"));
    docMeta.appendChild(instr);
    QDomElement entryElem = docMeta.createElement(QStringLiteral("entry"));
    docMeta.appendChild(entryElem);
    entryElem.setAttribute(QStringLiteral("xmlns"), QStringLiteral("http://www.w3.org/2005/Atom"));
    QDomElement titleElem = docMeta.createElement(QStringLiteral("title"));
    entryElem.appendChild(titleElem);
    QDomText titleText = docMeta.createTextNode(album.title);
    titleElem.appendChild(titleText);
    QDomElement summaryElem = docMeta.createElement(QStringLiteral("summary"));
    entryElem.appendChild(summaryElem);
    QDomText summaryText = docMeta.createTextNode(album.description);
    summaryElem.appendChild(summaryText);
    QDomElement locationElem = docMeta.createElementNS(
        QStringLiteral("http://schemas.google.com/photos/2007"),
        QStringLiteral("gphoto:location"));
    entryElem.appendChild(locationElem);
    QDomText locationText = docMeta.createTextNode(album.location);
    locationElem.appendChild(locationText);
    QDomElement accessElem = docMeta.createElementNS(
        QStringLiteral("http://schemas.google.com/photos/2007"),
        QStringLiteral("gphoto:access"));
    entryElem.appendChild(accessElem);
    QDomText accessText = docMeta.createTextNode(album.access);
    accessElem.appendChild(accessText);
    QDomElement commentElem = docMeta.createElementNS(
        QStringLiteral("http://schemas.google.com/photos/2007"),
        QStringLiteral("gphoto:commentingEnabled"));
    entryElem.appendChild(commentElem);
    QDomText commentText = docMeta.createTextNode(
        album.canComment ? QStringLiteral("true") : QStringLiteral("false"));
    commentElem.appendChild(commentText);
    QDomElement timestampElem = docMeta.createElementNS(
        QStringLiteral("http://schemas.google.com/photos/2007"),
        QStringLiteral("gphoto:timestamp"));
    entryElem.appendChild(timestampElem);
    QDomText timestampText = docMeta.createTextNode(album.timestamp);
    timestampElem.appendChild(timestampText);
    QDomElement categoryElem = docMeta.createElement(QStringLiteral("category"));
    entryElem.appendChild(categoryElem);
    categoryElem.setAttribute(
        QStringLiteral("scheme"),
        QStringLiteral("http://schemas.google.com/g/2005#kind"));
    categoryElem.setAttribute(
        QStringLiteral("term"),
        QStringLiteral("http://schemas.google.com/photos/2007#album"));
    QDomElement mediaGroupElem = docMeta.createElementNS(
        QStringLiteral("http://search.yahoo.com/mrss/"),
        QStringLiteral("media:group"));
    entryElem.appendChild(mediaGroupElem);
    QDomElement mediaKeywordsElem = docMeta.createElementNS(
        QStringLiteral("http://search.yahoo.com/mrss/"),
        QStringLiteral("media:keywords"));
    mediaGroupElem.appendChild(mediaKeywordsElem);
    QDomText mediaKeywordsText = docMeta.createTextNode(album.tags.join(QStringLiteral(",")));
    mediaKeywordsElem.appendChild(mediaKeywordsText);

    QByteArray buffer;
    buffer.append(docMeta.toString().toUtf8());

    QUrl url(QStringLiteral("https://picasaweb.google.com/data/feed/api/user/default"));
    QString auth_string = QStringLiteral("Authorization: ") + m_bearer_access_token;
    KIO::TransferJob* const job = KIO::http_post(url, buffer, KIO::HideProgressInfo);
    KIO::JobUiDelegate* const job_ui = static_cast<KIO::JobUiDelegate*>(job->ui());
    job_ui->setWindow(m_parent);
    job->addMetaData(
        QStringLiteral("content-type"),
        QStringLiteral("Content-Type: application/atom+xml"));
    job->addMetaData(
        QStringLiteral("content-length"),
        QStringLiteral("Content-Length: %1").arg(buffer.length()));
    job->addMetaData(QStringLiteral("customHTTPHeader"), auth_string);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_CREATEALBUM;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

bool PicasawebTalker::addPhoto(const QString& photoPath, GSPhoto& info, const QString& albumId,bool rescale,int maxDim,int imageQuality)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QUrl url(QStringLiteral("https://picasaweb.google.com/data/feed/api/user/default/albumid/") + albumId);
    QString     auth_string = QStringLiteral("Authorization: ") + m_bearer_access_token;
    MPForm_Picasa      form;
    
    QString path = photoPath;
    QImage image;

    if(KIPIPlugins::KPMetadata::isRawFile(QUrl::fromLocalFile(photoPath)))
    {
        KDcrawIface::KDcraw::loadRawPreview(image,photoPath);
    }
    else
    {
        image.load(photoPath);
    }

    if(image.isNull())
    {
        return false;
    }
    
    path = QDir::tempPath() + QLatin1Char('/') + QFileInfo(photoPath).baseName().trimmed() + QStringLiteral(".jpg");
    int imgQualityToApply = 100;

    if(rescale)
    {
        if(image.width() > maxDim || image.height() > maxDim)
            image = image.scaled(maxDim,maxDim,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        
        imgQualityToApply = imageQuality;
    }

    image.save(path,"JPEG",imgQualityToApply);

    KIPIPlugins::KPMetadata meta;

    if(meta.load(photoPath))
    {
        meta.setImageDimensions(image.size());
        meta.setImageProgramId(QStringLiteral("Kipi-plugins"), kipipluginsVersion());
        meta.save(path);
    }

    //Create the Body in atom-xml
    QDomDocument docMeta;
    QDomProcessingInstruction instr = docMeta.createProcessingInstruction(
        QStringLiteral("xml"),
        QStringLiteral("version='1.0' encoding='UTF-8'"));
    docMeta.appendChild(instr);
    QDomElement entryElem = docMeta.createElement(QStringLiteral("entry"));
    docMeta.appendChild(entryElem);
    entryElem.setAttribute(QStringLiteral("xmlns"), QStringLiteral("http://www.w3.org/2005/Atom"));
    QDomElement titleElem = docMeta.createElement(QStringLiteral("title"));
    entryElem.appendChild(titleElem);
    QDomText titleText = docMeta.createTextNode(QFileInfo(path).fileName()); // NOTE: Do not use info.title as arg here to set titleText because we change the format of image as .jpg before uploading.
    titleElem.appendChild(titleText);
    QDomElement summaryElem = docMeta.createElement(QStringLiteral("summary"));
    entryElem.appendChild(summaryElem);
    QDomText summaryText = docMeta.createTextNode(info.description);
    summaryElem.appendChild(summaryText);
    QDomElement categoryElem = docMeta.createElement(QStringLiteral("category"));
    entryElem.appendChild(categoryElem);
    categoryElem.setAttribute(
        QStringLiteral("scheme"),
        QStringLiteral("http://schemas.google.com/g/2005#kind"));
    categoryElem.setAttribute(
        QStringLiteral("term"),
        QStringLiteral("http://schemas.google.com/photos/2007#photo"));
    QDomElement mediaGroupElem = docMeta.createElementNS(
        QStringLiteral("http://search.yahoo.com/mrss/"),
        QStringLiteral("media:group"));
    entryElem.appendChild(mediaGroupElem);
    QDomElement mediaKeywordsElem = docMeta.createElementNS(
        QStringLiteral("http://search.yahoo.com/mrss/"),
        QStringLiteral("media:keywords"));
    mediaGroupElem.appendChild(mediaKeywordsElem);
    QDomText mediaKeywordsText = docMeta.createTextNode(info.tags.join(QStringLiteral(",")));
    mediaKeywordsElem.appendChild(mediaKeywordsText);

    if (!info.gpsLat.isEmpty() && !info.gpsLon.isEmpty())
    {
        QDomElement whereElem = docMeta.createElementNS(
            QStringLiteral("http://www.georss.org/georss"),
            QStringLiteral("georss:where"));
        entryElem.appendChild(whereElem);
        QDomElement pointElem = docMeta.createElementNS(
            QStringLiteral("http://www.opengis.net/gml"),
            QStringLiteral("gml:Point"));
        whereElem.appendChild(pointElem);
        QDomElement gpsElem = docMeta.createElementNS(
            QStringLiteral("http://www.opengis.net/gml"),
            QStringLiteral("gml:pos"));
        pointElem.appendChild(gpsElem);
        QDomText gpsVal = docMeta.createTextNode(info.gpsLat + QLatin1Char(' ') + info.gpsLon);
        gpsElem.appendChild(gpsVal);
    }

    form.addPair(QStringLiteral("descr"), docMeta.toString(), QStringLiteral("application/atom+xml"));

    if (!form.addFile(QStringLiteral("photo"), path))
        return false;

    form.finish();

    KIO::TransferJob* const job = KIO::http_post(url, form.formData(), KIO::HideProgressInfo);
    KIO::JobUiDelegate* const job_ui = static_cast<KIO::JobUiDelegate*>(job->ui());
    job_ui->setWindow(m_parent);
    job->addMetaData(QStringLiteral("content-type"), form.contentType());
    job->addMetaData(QStringLiteral("content-length"),
                     QStringLiteral("Content-Length: %1").arg(form.formData().length()));
    job->addMetaData(QStringLiteral("customHTTPHeader"),
                     auth_string + QStringLiteral("\nMIME-version: 1.0"));

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

bool PicasawebTalker::updatePhoto(const QString& photoPath, GSPhoto& info/*, const QString& albumId*/,bool rescale,int maxDim,int imageQuality)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    MPForm_Picasa      form;
    
    QString path = photoPath;
    QImage image;

    if(KIPIPlugins::KPMetadata::isRawFile(QUrl::fromLocalFile(photoPath)))
    {
        KDcrawIface::KDcraw::loadRawPreview(image,photoPath);
    }
    else
    {
        image.load(photoPath);
    }

    if(image.isNull())
    {
        return false;
    }
    
    path = QDir::tempPath() + QLatin1Char('/') + QFileInfo(photoPath).baseName().trimmed() + QStringLiteral(".jpg");
    
    int imgQualityToApply = 100;

    if(rescale)
    {
        if(image.width() > maxDim || image.height() > maxDim)
            image = image.scaled(maxDim,maxDim,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        
        imgQualityToApply = imageQuality;
    }

    image.save(path,"JPEG",imgQualityToApply);

    KIPIPlugins::KPMetadata meta;

    if(meta.load(photoPath))
    {
        meta.setImageDimensions(image.size());
        meta.setImageProgramId(QStringLiteral("Kipi-plugins"), kipipluginsVersion());
        meta.save(path);
    }

    //Create the Body in atom-xml
    QDomDocument docMeta;
    QDomProcessingInstruction instr = docMeta.createProcessingInstruction(
        QStringLiteral("xml"),
        QStringLiteral("version='1.0' encoding='UTF-8'"));
    docMeta.appendChild(instr);
    QDomElement entryElem = docMeta.createElement(QStringLiteral("entry"));
    docMeta.appendChild(entryElem);
    entryElem.setAttribute(
        QStringLiteral("xmlns"),
        QStringLiteral("http://www.w3.org/2005/Atom"));
    QDomElement titleElem = docMeta.createElement(QStringLiteral("title"));
    entryElem.appendChild(titleElem);
    QDomText titleText = docMeta.createTextNode(QFileInfo(path).fileName());
    titleElem.appendChild(titleText);
    QDomElement summaryElem = docMeta.createElement(QStringLiteral("summary"));
    entryElem.appendChild(summaryElem);
    QDomText summaryText = docMeta.createTextNode(info.description);
    summaryElem.appendChild(summaryText);
    QDomElement categoryElem = docMeta.createElement(QStringLiteral("category"));
    entryElem.appendChild(categoryElem);
    categoryElem.setAttribute(
        QStringLiteral("scheme"),
        QStringLiteral("http://schemas.google.com/g/2005#kind"));
    categoryElem.setAttribute(
        QStringLiteral("term"),
        QStringLiteral("http://schemas.google.com/photos/2007#photo"));
    QDomElement mediaGroupElem = docMeta.createElementNS(
        QStringLiteral("http://search.yahoo.com/mrss/"),
        QStringLiteral("media:group"));
    entryElem.appendChild(mediaGroupElem);
    QDomElement mediaKeywordsElem = docMeta.createElementNS(
        QStringLiteral("http://search.yahoo.com/mrss/"),
        QStringLiteral("media:keywords"));
    mediaGroupElem.appendChild(mediaKeywordsElem);
    QDomText mediaKeywordsText = docMeta.createTextNode(info.tags.join(QStringLiteral(",")));
    mediaKeywordsElem.appendChild(mediaKeywordsText);

    if (!info.gpsLat.isEmpty() && !info.gpsLon.isEmpty())
    {
        QDomElement whereElem = docMeta.createElementNS(
            QStringLiteral("http://www.georss.org/georss"),
            QStringLiteral("georss:where"));
        entryElem.appendChild(whereElem);
        QDomElement pointElem = docMeta.createElementNS(
            QStringLiteral("http://www.opengis.net/gml"),
            QStringLiteral("gml:Point"));
        whereElem.appendChild(pointElem);
        QDomElement gpsElem = docMeta.createElementNS(
            QStringLiteral("http://www.opengis.net/gml"),
            QStringLiteral("gml:pos"));
        pointElem.appendChild(gpsElem);
        QDomText gpsVal = docMeta.createTextNode(info.gpsLat + QLatin1Char(' ') + info.gpsLon);
        gpsElem.appendChild(gpsVal);
    }

    form.addPair(QStringLiteral("descr"), docMeta.toString(), QStringLiteral("application/atom+xml"));

    if (!form.addFile(QStringLiteral("photo"), path))
        return false;

    form.finish();

    QString auth_string = QStringLiteral("Authorization: ") + m_bearer_access_token;
    KIO::TransferJob* const job = KIO::put(info.editUrl, -1, KIO::HideProgressInfo);
    KIO::JobUiDelegate* const job_ui = static_cast<KIO::JobUiDelegate*>(job->ui());
    job_ui->setWindow(m_parent);
    job->addMetaData(QStringLiteral("content-type"), form.contentType());
    job->addMetaData(QStringLiteral("content-length"),
                     QStringLiteral("Content-Length: %1").arg(form.formData().length()));
    job->addMetaData(QStringLiteral("customHTTPHeader"),
                     auth_string + QStringLiteral("\nIf-Match: *"));

    m_jobData.insert(job, form.formData());

    connect(job, SIGNAL(dataReq(KIO::Job*,QByteArray&)),
            this, SLOT(dataReq(KIO::Job*,QByteArray&)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_UPDATEPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    return true;
}

void PicasawebTalker::getPhoto(const QString& imgPath)
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);

    KIO::TransferJob* const job = KIO::get(QUrl(imgPath), KIO::Reload, KIO::HideProgressInfo);
    //job->addMetaData("customHTTPHeader", "Authorization: " + auth_string );

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    m_state = FE_GETPHOTO;
    m_job   = job;
    m_buffer.resize(0);
}

QString PicasawebTalker::getUserName() const
{
    return m_username;
}

QString PicasawebTalker::getUserEmailId() const
{
    return m_userEmailId;
}

QString PicasawebTalker::getLoginName() const
{
    return m_loginName;
}

void PicasawebTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(false);
}

void PicasawebTalker::info(KIO::Job* /*job*/, const QString& /*str*/)
{
}

void PicasawebTalker::dataReq(KIO::Job* job, QByteArray& data)
{
    if (m_jobData.contains(job))
    {
        data = m_jobData.value(job);
        m_jobData.remove(job);
    }
}

void PicasawebTalker::slotError(const QString & error)
{
    QString transError;
    int     errorNo = 0;

    if (!error.isEmpty())
        errorNo = error.toInt();

    switch (errorNo)
    {
        case 2:
            transError=i18n("No photo specified");break;
        case 3:
            transError=i18n("General upload failure");break;
        case 4:
            transError=i18n("File-size was zero");break;
        case 5:
            transError=i18n("File-type was not recognized");break;
        case 6:
            transError=i18n("User exceeded upload limit");break;
        case 96:
            transError=i18n("Invalid signature"); break;
        case 97:
            transError=i18n("Missing signature"); break;
        case 98:
            transError=i18n("Login failed / Invalid auth token"); break;
        case 100:
            transError=i18n("Invalid API Key"); break;
        case 105:
            transError=i18n("Service currently unavailable");break;
        case 108:
            transError=i18n("Invalid Frob");break;
        case 111:
            transError=i18n("Format \"xxx\" not found"); break;
        case 112:
            transError=i18n("Method \"xxx\" not found"); break;
        case 114:
            transError=i18n("Invalid SOAP envelope");break;
        case 115:
            transError=i18n("Invalid XML-RPC Method Call");break;
        case 116:
            transError=i18n("The POST method is now required for all setters."); break;
        default:
            transError=i18n("Unknown error");
    };
    
    QMessageBox::critical(QApplication::activeWindow(), i18nc("@title:window", "Error"),
                          i18n("Error occurred: %1\nUnable to proceed further.",transError + error));
}

void PicasawebTalker::slotResult(KJob *job)
{
    m_job = 0;
    emit signalBusy(false);

    if (job->error())
    {
        if (m_state == FE_ADDPHOTO)
        {
            emit signalAddPhotoDone(job->error(), job->errorText(), QStringLiteral("-1"));
        }
        else
        {
            static_cast<KIO::Job*>(job)->ui()->showErrorMessage();
        }

        return;
    }

    switch(m_state)
    {
        case(FE_LOGOUT):
            break;        
        case(FE_CREATEALBUM):
            parseResponseCreateAlbum(m_buffer);
            break;
        case(FE_LISTALBUMS):
            parseResponseListAlbums(m_buffer);
            break;
        case(FE_LISTPHOTOS):
            parseResponseListPhotos(m_buffer);
            break;
        case(FE_ADDPHOTO):
            parseResponseAddPhoto(m_buffer);
            break;
        case(FE_UPDATEPHOTO):
            emit signalAddPhotoDone(1, QStringLiteral(""), QStringLiteral(""));
            break;
        case(FE_GETPHOTO):
            // all we get is data of the image
            emit signalGetPhotoDone(1, QString(), m_buffer);
            break;
    }
}

void PicasawebTalker::parseResponseListAlbums(const QByteArray& data)
{
    QDomDocument doc(QStringLiteral("feed"));
    QString err;
    int line;
    int columns;

    if ( !doc.setContent( data, false, &err, &line, &columns ) )
    {
        qCCritical(KIPIPLUGINS_LOG)<<"error is "<<err<< " at line "<<line<<" at columns "<<columns;
        emit signalListAlbumsDone(0, i18n("Failed to fetch photo-set list"), QList<GSFolder>());
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
    QDomElement e;

    QList<GSFolder> albumList;

    while (!node.isNull())
    {
        if(node.isElement() && node.nodeName() == QStringLiteral("gphoto:nickname"))
        {
            m_loginName = node.toElement().text();
        }
        
        if(node.isElement() && node.nodeName() == QStringLiteral("gphoto:user"))
        {
            m_username = node.toElement().text();
        }


        if (node.isElement() && node.nodeName() == QStringLiteral("entry"))
        {
            e = node.toElement();
            QDomNode details=e.firstChild();
            GSFolder fps;
            QDomNode detailsNode = details;

            while(!detailsNode.isNull())
            {
                if(detailsNode.isElement())
                {
                    //kDebug()<<"Node name is "<< detailsNode.nodeName()<<" and its value is "<<detailsNode.toElement().text();
                    if(detailsNode.nodeName() == QStringLiteral("gphoto:id"))
                    {
                        fps.id = detailsNode.toElement().text();
                    }

                    if(detailsNode.nodeName() == QStringLiteral("title"))
                    {
                        fps.title = detailsNode.toElement().text();
                    }

                    if(detailsNode.nodeName() == QStringLiteral("gphoto:access"))
                    {
                        fps.access = detailsNode.toElement().text();
                    }
                }
                detailsNode = detailsNode.nextSibling();
            }
            albumList.append(fps);
        }
        node = node.nextSibling();
    }

    qSort(albumList.begin(), albumList.end(), picasaLessThan);
    emit signalListAlbumsDone(1, QStringLiteral(""), albumList);
}

void PicasawebTalker::parseResponseListPhotos(const QByteArray& data)
{
    QDomDocument doc(QStringLiteral("feed"));
    if ( !doc.setContent( data ) )
    {
        emit signalListPhotosDone(0, i18n("Failed to fetch photo-set list"), QList<GSPhoto>());
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode node       = docElem.firstChild();

    QList<GSPhoto> photoList;

    while(!node.isNull())
    {
        if (node.isElement() && node.nodeName() == QStringLiteral("entry"))
        {
            QDomNode details     = node.firstChild();
            GSPhoto fps;
            QDomNode detailsNode = details;

            while(!detailsNode.isNull())
            {
                if(detailsNode.isElement())
                {
                    QDomElement detailsElem = detailsNode.toElement();

                    if(detailsNode.nodeName() == QStringLiteral("gphoto:id"))
                    {
                        fps.id = detailsElem.text();
                    }

                    if(detailsNode.nodeName() == QStringLiteral("title"))
                    {
                        //fps.title = detailsElem.text();
                    }
                    
                    if(detailsNode.nodeName() == QStringLiteral("summary"))
                    {
                        fps.description = detailsElem.text();
                    }

                    if(detailsNode.nodeName() == QStringLiteral("gphoto:access"))
                    {
                        fps.access = detailsElem.text();
                    }

                    if (detailsNode.nodeName() == QStringLiteral("link") &&
                        detailsElem.attribute(QStringLiteral("rel")) == QStringLiteral("edit-media"))
                    {
                        fps.editUrl = QUrl(detailsElem.attribute(QStringLiteral("href")));
                    }

                    if(detailsNode.nodeName() == QStringLiteral("georss:where"))
                    {
                        QDomNode geoPointNode = detailsElem.namedItem(QStringLiteral("gml:Point"));

                        if (!geoPointNode.isNull() && geoPointNode.isElement())
                        {
                            QDomNode geoPosNode = geoPointNode.toElement().namedItem(
                                QStringLiteral("gml:pos"));

                            if (!geoPosNode.isNull() && geoPosNode.isElement())
                            {
                                QStringList value = geoPosNode.toElement().text().split(QLatin1Char(' '));

                                if (value.size() == 2)
                                {
                                    fps.gpsLat = value[0];
                                    fps.gpsLon = value[1];
                                }
                            }
                        }
                    }

                    if(detailsNode.nodeName() == QStringLiteral("media:group"))
                    {
                        QDomNode thumbNode = detailsElem.namedItem(QStringLiteral("media:thumbnail"));

                        if (!thumbNode.isNull() && thumbNode.isElement())
                        {
                            fps.thumbURL = QUrl(thumbNode.toElement().attribute(
                                QStringLiteral("url"), QStringLiteral("")));
                        }

                        QDomNode keywordNode = detailsElem.namedItem(QStringLiteral("media:keywords"));

                        if (!keywordNode.isNull() && keywordNode.isElement())
                        {
                            fps.tags = keywordNode.toElement().text().split(QLatin1Char(','));
                        }

                        QDomNodeList contentsList = detailsElem.elementsByTagName(
                            QStringLiteral("media:content"));

                        for(int i = 0; i < contentsList.size(); ++i)
                        {
                            QDomElement contentElem = contentsList.at(i).toElement();

                            if (!contentElem.isNull())
                            {
                                if ((contentElem.attribute(QStringLiteral("medium")) == QStringLiteral("image")) &&
                                    fps.originalURL.isEmpty())
                                {
                                    fps.originalURL = QUrl(contentElem.attribute(QStringLiteral("url")));
                                    fps.title = fps.originalURL.fileName();
                                    fps.mimeType = contentElem.attribute(QStringLiteral("type"));
                                }

                                if ((contentElem.attribute(QStringLiteral("medium")) == QStringLiteral("video")) &&
                                    (contentElem.attribute(QStringLiteral("type")) == QStringLiteral("video/mpeg4")))
                                {
                                    fps.originalURL = QUrl(contentElem.attribute(QStringLiteral("url")));
                                    fps.title = fps.originalURL.fileName();
                                    fps.mimeType = contentElem.attribute(QStringLiteral("type"));
                                }
                            }
                        }
                    }
                }

                detailsNode = detailsNode.nextSibling();
            }

            photoList.append(fps);
        }

        node = node.nextSibling();
    }

    emit signalListPhotosDone(1, QStringLiteral(""), photoList);
}

void PicasawebTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    bool success = false;

    QDomDocument doc(QStringLiteral("AddPhoto Response"));

    if ( !doc.setContent( data ) )
    {
        signalCreateAlbumDone(0, i18n("Failed to create album"), QStringLiteral("-1"));
        return;
    }

    // parse the new album name
    QDomElement docElem = doc.documentElement();
    QString albumId(QStringLiteral(""));

    if (docElem.nodeName() == QStringLiteral("entry"))
    {
        success       = true;
        QDomNode node = docElem.firstChild(); //this should mean <entry>

        while( !node.isNull() )
        {
            if ( node.isElement())
            {
                if (node.nodeName() == QStringLiteral("gphoto:id"))
                {
                    albumId = node.toElement().text();
                }
             }
            node = node.nextSibling();
        }
    }

    if(success == true)
    {
        signalCreateAlbumDone(1, QStringLiteral(""), albumId);
    }
    else
    {
        signalCreateAlbumDone(0, i18n("Failed to create album"), QStringLiteral("-1"));
    }
}

void PicasawebTalker::parseResponseAddPhoto(const QByteArray& data)
{
    QDomDocument doc(QStringLiteral("AddPhoto Response"));

    if ( !doc.setContent( data ) )
    {
        emit signalAddPhotoDone(0, i18n("Failed to upload photo"), QStringLiteral("-1"));
        return;
    }
    
    // parse the new album name
    QDomElement docElem = doc.documentElement();
    QString photoId(QStringLiteral(""));

    if (docElem.nodeName() == QStringLiteral("entry"))
    {
        QDomNode node = docElem.firstChild(); //this should mean <entry>

        while(!node.isNull())
        {
            if (node.isElement())
            {
                if (node.nodeName() == QStringLiteral("gphoto:id"))
                {
                    photoId = node.toElement().text();
                }
            }
            node = node.nextSibling();
        }
    }

    emit signalAddPhotoDone(1, QStringLiteral(""), photoId);
}

} // KIPIGoogleServicesPlugin
