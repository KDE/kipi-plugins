/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2010-02-15
* Description : a plugin to export to a remote Piwigo server.
*
* Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
* Copyright (C) 2006      by Colin Guthrie <kde@colin.guthr.ie>
* Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
* Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2010      by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#include "piwigotalker.moc"

// Qt includes

#include <QByteArray>
#include <QImage>
#include <QRegExp>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QCryptographicHash>

// KDE includes

#include <kdebug.h>
#include <kio/jobuidelegate.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "piwigoitem.h"
#include "kpversion.h"
#include "kpmetadata.h"
#include "kpimageinfo.h"

using namespace KIPIPlugins;

namespace KIPIPiwigoExportPlugin
{

QString PiwigoTalker::s_authToken = "";

PiwigoTalker::PiwigoTalker(QWidget* const parent)
    : m_parent(parent),  m_job(0),  m_loggedIn(false), m_version(-1)
{
}

PiwigoTalker::~PiwigoTalker()
{
    if (m_job)
        m_job->kill();
}

QByteArray PiwigoTalker::computeMD5Sum(const QString& filepath)
{
    QFile file(filepath);

    file.open(QIODevice::ReadOnly);
    QByteArray md5sum = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
    file.close();

    return md5sum;
}

bool PiwigoTalker::loggedIn() const
{
    return m_loggedIn;
}

void PiwigoTalker::login(const KUrl& url, const QString& name, const QString& passwd)
{
    m_job   = 0;
    m_url   = url;
    m_state = GE_LOGIN;
    m_talker_buffer.resize(0);

    // Add the page to the URL
    if (!m_url.url().endsWith(QLatin1String(".php")))
    {
        m_url.addPath("/ws.php");
    }

    QString auth = name + QString(":") + passwd;
    s_authToken = "Basic " + auth.toUtf8().toBase64();

    QStringList qsl;
    qsl.append("password=" + passwd);
    qsl.append("method=pwg.session.login");
    qsl.append("username=" + name);
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());
    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);
}

void PiwigoTalker::listAlbums()
{
    m_job   = 0;
    m_state = GE_LISTALBUMS;
    m_talker_buffer.resize(0);

    QStringList qsl;
    qsl.append("method=pwg.categories.getList");
    qsl.append("recursive=true");
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);
}

bool PiwigoTalker::addPhoto(int   albumId,
                            const QString& photoPath,
                            bool  rescale,
                            int   maxWidth,
                            int   maxHeight,
                            int   thumbDim)
{
    KUrl photoUrl = KUrl(photoPath);

    m_job     = 0;
    m_state   = GE_CHECKPHOTOEXIST;
    m_talker_buffer.resize(0);

    m_path    = photoPath;
    m_albumId = albumId;
    m_md5sum  = computeMD5Sum(photoPath);

    if (!rescale)
    {
        m_hqpath = photoPath;
        kDebug() << "Download HQ version: " << m_hqpath;
    }
    else
    {
        m_hqpath = "";
    }

    kDebug() << photoPath << " " << m_md5sum.toHex();

    QImage image;

    // Check if RAW file.
    if (KPMetadata::isRawFile(photoPath))
        KDcrawIface::KDcraw::loadDcrawPreview(image, photoPath);
    else
        image.load(photoPath);

    if (!image.isNull())
    {
        QFileInfo fi(photoPath);
        QImage thumbnail = image.scaled(thumbDim, thumbDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_thumbpath      = KStandardDirs::locateLocal("tmp", "thumb-" + KUrl(photoPath).fileName());
        thumbnail.save(m_thumbpath, "JPEG", 95);

        kDebug() << "Thumbnail to temp file: " << m_thumbpath ;

        // image file - see if we need to rescale it
        if (image.width() > maxWidth || image.height() > maxHeight)
        {
            image = image.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        m_path = KStandardDirs::locateLocal("tmp", KUrl(photoPath).fileName());
        image.save(m_path, "JPEG", 95);

        kDebug() << "Resizing and saving to temp file: " << m_path ;


        // Complete name and comment for summary sending
        m_title = fi.completeBaseName();
        m_comment = "";
        m_author = "";
        m_date    = fi.created();
        
        // Look in the Digikam database
        KPImageInfo info(photoUrl);
        if (info.hasTitle() && !info.title().isEmpty())
            m_title = info.title();
        if (info.hasDescription() && !info.description().isEmpty())
            m_comment = info.description();
        if (info.hasCreators() && !info.creators().isEmpty())
            m_author = info.creators().join(" / ");
        if (info.hasDate())
            m_date = info.date();
        kDebug() << "Title: " << m_title;
        kDebug() << "Comment: " << m_comment;
        kDebug() << "Author: " << m_author;
        kDebug() << "Date: " << m_date;
        
        // Restore all metadata with EXIF
        // in the resized version
        KPMetadata meta;
        if (meta.load(photoPath))
        {
            meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            meta.setImageDimensions(image.size());
            meta.save(m_path);
        }
        else
        {
            kDebug() << "Image " << photoPath << " has no exif data";
        }
    }
    else
    {
        // Invalid image
        return false;
    }

    QStringList qsl;
    qsl.append("method=pwg.images.exist");
    qsl.append("md5sum_list=" + m_md5sum.toHex());
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    emit signalProgressInfo( i18n("Check if %1 already exists", KUrl(m_path).fileName()) );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);

    return true;
}

void PiwigoTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
}

void PiwigoTalker::slotTalkerData(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_talker_buffer.size();
    m_talker_buffer.resize(oldSize + data.size());
    memcpy(m_talker_buffer.data() + oldSize, data.data(), data.size());
}

void PiwigoTalker::slotResult(KJob* job)
{
    KIO::Job* tempjob = static_cast<KIO::Job*>(job);
    State state = m_state; // Can change in the treatment itself
                           // so we cache it
    
    if (tempjob->error())
    {
        if (state == GE_LOGIN)
        {
            emit signalLoginFailed(tempjob->errorString());
            kDebug() << tempjob->errorString();
        }
        else if (state == GE_GETVERSION) {
            kDebug() << tempjob->errorString();
            // Version isn't mandatory and errors can be ignored
            // As login succeeded, albums can be listed
            listAlbums();
        }
        else if (state == GE_CHECKPHOTOEXIST || state == GE_GETINFO ||
                 state == GE_SETINFO         || state == GE_ADDPHOTOCHUNK ||
                 state == GE_ADDPHOTOSUMMARY || state == GE_OLD_ADDPHOTOCHUNK ||
                 state == GE_OLD_ADDTHUMB    || state == GE_OLD_ADDHQ ||
                 state == GE_OLD_ADDPHOTOSUMMARY)
        {
            emit signalAddPhotoFailed(tempjob->errorString());
        }
        else
        {
            tempjob->ui()->setWindow(m_parent);
            tempjob->ui()->showErrorMessage();
        }
        emit signalBusy(false);
        return;
    }

    switch (state)
    {
        case(GE_LOGIN):
            parseResponseLogin(m_talker_buffer);
            break;
        case(GE_GETVERSION):
            parseResponseGetVersion(m_talker_buffer);
            break;
        case(GE_LISTALBUMS):
            parseResponseListAlbums(m_talker_buffer);
            break;
        case(GE_CHECKPHOTOEXIST):
            parseResponseDoesPhotoExist(m_talker_buffer);
            break;
        case(GE_GETINFO):
            parseResponseGetInfo(m_talker_buffer);
            break;
        case(GE_SETINFO):
            parseResponseSetInfo(m_talker_buffer);
            break;
        // Support for Web API >= 2.4
        case(GE_ADDPHOTOCHUNK):
            parseResponseAddPhotoChunk(m_talker_buffer);
            break;
        case(GE_ADDPHOTOSUMMARY):
            parseResponseAddPhotoSummary(m_talker_buffer);
            break;
        // Support for Web API < 2.4
        case(GE_OLD_ADDPHOTOCHUNK):
            parseResponseOldAddPhoto(m_talker_buffer);
            break;
        case(GE_OLD_ADDTHUMB):
            parseResponseOldAddThumbnail(m_talker_buffer);
            break;
        case(GE_OLD_ADDHQ):
            parseResponseOldAddHQPhoto(m_talker_buffer);
            break;
        case(GE_OLD_ADDPHOTOSUMMARY):
            parseResponseOldAddPhotoSummary(m_talker_buffer);
            break;
    }

    tempjob->kill();
    m_job = 0;

    if (state == GE_GETVERSION && m_loggedIn)
    {
        listAlbums();
    }
    emit signalBusy(false);
}

void PiwigoTalker::parseResponseLogin(const QByteArray& data)
{
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    m_loggedIn         = false;

    kDebug() << "parseResponseLogin: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            foundResponse = true;
            if (ts.name() == "rsp" && ts.attributes().value("stat") == "ok")
            {
                m_loggedIn = true;

                /** Request Version */
                m_state = GE_GETVERSION;
                m_talker_buffer.resize(0);
                m_version = -1;
                
                QByteArray buffer = "method=pwg.getVersion";
                m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
                m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
                m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

                connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
                        this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

                connect(m_job, SIGNAL(result(KJob*)),
                        this, SLOT(slotResult(KJob*)));

                emit signalBusy(true);

                return;
            }
        }
    }

    if (!foundResponse)
    {
        emit signalLoginFailed(i18n("Piwigo URL probably incorrect"));
        return;
    }

    if (!m_loggedIn)
    {
        emit signalLoginFailed(i18n("Incorrect username or password specified"));
    }
}

void PiwigoTalker::parseResponseGetVersion(const QByteArray& data)
{
    QXmlStreamReader ts(data);
    QString line;
    QRegExp verrx(".?(\\d)\\.(\\d).*");
    
    bool foundResponse = false;

    kDebug() << "parseResponseGetVersion: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            foundResponse = true;
            if (ts.name() == "rsp" && ts.attributes().value("stat") == "ok")
            {
                QString v = ts.readElementText();
                
                if (verrx.exactMatch(v)) {
                    QStringList qsl = verrx.capturedTexts();
                    m_version = qsl[1].toInt() * 10 + qsl[2].toInt();
                    kDebug() << "Version: " << m_version;
                    break;
                }
            }
        }
    }
}

void PiwigoTalker::parseResponseListAlbums(const QByteArray& data)
{
    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    typedef QList<GAlbum> GAlbumList;
    GAlbumList albumList;
    GAlbumList::iterator iter = albumList.begin();

    kDebug() << "parseResponseListAlbums";

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isEndElement() && ts.name() == "categories")
            break;

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp" && ts.attributes().value("stat") == "ok")
            {
                foundResponse = true;
            }
            if (ts.name() == "categories")
            {
                success = true;
            }
            if (ts.name() == "category")
            {
                GAlbum album;
                album.ref_num = ts.attributes().value("id").toString().toInt();
                album.parent_ref_num = -1;

                kDebug() << album.ref_num << "\n";

                iter = albumList.insert(iter, album);
            }
            if (ts.name() == "name")
            {
                (*iter).name = ts.readElementText();
                kDebug() << (*iter).name << "\n";
            }
            if (ts.name() == "uppercats")
            {
                QString uppercats   = ts.readElementText();
                QStringList catlist = uppercats.split(',');

                if (catlist.size() > 1 && catlist.at(catlist.size() - 2).toInt() != (*iter).ref_num)
                {
                    (*iter).parent_ref_num = catlist.at(catlist.size() - 2).toInt();
                    kDebug() << (*iter).parent_ref_num << "\n";
                }
            }
        }
    }

    if (!foundResponse)
    {
        emit signalError(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (!success)
    {
        emit signalError(i18n("Failed to list albums"));
        return;
    }

    // We need parent albums to come first for rest of the code to work
    qSort(albumList);

    emit signalAlbums(albumList);
}

void PiwigoTalker::parseResponseDoesPhotoExist(const QByteArray& data)
{
    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    kDebug() << "parseResponseDoesPhotoExist: " << QString(data);

    while (!ts.atEnd())
    {

        ts.readNext();

        if (ts.name() == "rsp")
        {
            QRegExp md5rx("([a-f0-9]+)>([0-9]+)</.+");

            foundResponse = true;
            if (ts.attributes().value("stat") == "ok")
                success = true;

            // The next XML is invalid as the element can start with a digit
            ts.readNext();

            if (md5rx.exactMatch(data.mid(ts.characterOffset()))) {
                QStringList qsl = md5rx.capturedTexts();
                if (qsl[1] == QString(m_md5sum.toHex())) {
                    m_photoId = qsl[2].toInt();
                    kDebug() << "m_photoId: " << m_photoId;

                    emit signalProgressInfo(i18n("Photo '%1' already exists.", m_title));

                    m_state = GE_GETINFO;
                    m_talker_buffer.resize(0);

                    QStringList qsl;
                    qsl.append("method=pwg.images.getInfo");
                    qsl.append("image_id=" + QString::number(m_photoId));
                    QString dataParameters = qsl.join("&");
                    QByteArray buffer;
                    buffer.append(dataParameters.toUtf8());

                    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
                    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
                    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

                    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
                        this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

                    connect(m_job, SIGNAL(result(KJob*)),
                        this, SLOT(slotResult(KJob*)));

                    return;
                }
            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
        return;
    }


    if (m_version >= PIWIGO_VER_2_4) {
        m_state = GE_ADDPHOTOCHUNK;
        m_talker_buffer.resize(0);
        m_chunkId = 0;
        addNextChunk();
    } else {
        m_state = GE_OLD_ADDPHOTOCHUNK;
        m_talker_buffer.resize(0);

        QFile imagefile(m_path);
        imagefile.open(QIODevice::ReadOnly);

        QStringList qsl;
        qsl.append("method=pwg.images.addChunk");
        qsl.append("original_sum=" + m_md5sum.toHex());
        qsl.append("position=1");
        qsl.append("type=file");
        qsl.append("data=" + imagefile.readAll().toBase64().toPercentEncoding());
        QString dataParameters = qsl.join("&");
        QByteArray buffer;
        buffer.append(dataParameters.toUtf8());

        imagefile.close();

        m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
        m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
        m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

        emit signalProgressInfo( i18n("Upload resized version of %1", KUrl(m_path).fileName()) );

        connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
                this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

        connect(m_job, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));
    }
}

void PiwigoTalker::parseResponseGetInfo(const QByteArray& data)
{
    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;
    QList<int> categories;

    kDebug() << "parseResponseGetInfo: " << QString(data);


    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp")
            {
                foundResponse = true;
                if (ts.attributes().value("stat") == "ok") success = true;
            }
            if (ts.name() == "category")
            {
                if (ts.attributes().hasAttribute("id")) {
                    QString id(ts.attributes().value("id").toString());
                    categories.append(id.toInt());
                }
            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (categories.contains(m_albumId)) {
        emit signalAddPhotoFailed(i18n("Photo '%1' already exists in this album.", m_title));
        return;
    } else {
        categories.append(m_albumId);
    }

    m_state = GE_SETINFO;
    m_talker_buffer.resize(0);

    QStringList qsl_cat;
    for (int i = 0; i < categories.size(); ++i) {
        qsl_cat.append(QString::number(categories.at(i)));
    }
    
    QStringList qsl;
    qsl.append("method=pwg.images.setInfo");
    qsl.append("image_id=" + QString::number(m_photoId));
    qsl.append("categories=" + qsl_cat.join(";"));
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    return;

}

void PiwigoTalker::parseResponseSetInfo(const QByteArray& data)
{
    QString str = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    kDebug() << "parseResponseSetInfo: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp")
            {
                foundResponse = true;
                if (ts.attributes().value("stat") == "ok") success = true;
                break;
            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
        return;
    }

    if (m_path.size())
        QFile(m_path).remove();

    if (m_thumbpath.size())
        QFile(m_thumbpath).remove();

    m_path      = "";
    m_thumbpath = "";

    emit signalAddPhotoSucceeded();
}

void PiwigoTalker::addNextChunk()
{
    QFile imagefile(!m_hqpath.isEmpty() ? m_hqpath : m_path);

    imagefile.open(QIODevice::ReadOnly);

    m_chunkId++; // We start with chunk 1

    imagefile.seek((m_chunkId - 1) * CHUNK_MAX_SIZE);

    m_talker_buffer.resize(0);
    QStringList qsl;
    qsl.append("method=pwg.images.addChunk");
    qsl.append("original_sum=" + m_md5sum.toHex());
    qsl.append("position=" + QString::number(m_chunkId));
    qsl.append("type=file");
    qsl.append("data=" + imagefile.read(CHUNK_MAX_SIZE).toBase64().toPercentEncoding());
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    imagefile.close();

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    emit signalProgressInfo( i18n("Upload the chunk %1 of %2", m_chunkId, KUrl(m_path).fileName()) );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}


void PiwigoTalker::parseResponseAddPhotoChunk(const QByteArray& data)
{
    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    kDebug() << "parseResponseAddPhotoChunk: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp")
            {
                foundResponse = true;
                if (ts.attributes().value("stat") == "ok") success = true;
                break;
            }
        }
    }

    if (!foundResponse || !success)
    {
        emit signalProgressInfo(i18n("Warning : The full size photo cannot be uploaded."));
    }

    // If the photo wasn't completely sent, send the next chunk
    QFileInfo fi(!m_hqpath.isEmpty() ? m_hqpath : m_path);
    if (m_chunkId * CHUNK_MAX_SIZE < fi.size())
    {
        addNextChunk();
    }
    else
    {
        addPhotoSummary();
    }
}

void PiwigoTalker::addPhotoSummary()
{
    m_state = GE_OLD_ADDPHOTOSUMMARY;
    m_talker_buffer.resize(0);

    QFile imagefile(m_thumbpath);
    imagefile.open(QIODevice::ReadOnly);

    QStringList qsl;
    qsl.append("method=pwg.images.add");
    qsl.append("original_sum=" + m_md5sum.toHex());
    qsl.append("name=" + m_title.toUtf8().toPercentEncoding());
    if (!m_author.isEmpty()) qsl.append("author=" + m_author.toUtf8().toPercentEncoding());
    if (!m_comment.isEmpty()) qsl.append("comment=" + m_comment.toUtf8().toPercentEncoding());
    qsl.append("categories=" + QString::number(m_albumId));
    qsl.append("file_sum=" + computeMD5Sum(m_path).toHex());
    qsl.append("date_creation=" + m_date.toString("yyyy-MM-dd").toUtf8().toPercentEncoding());
    //qsl.append("tag_ids="); // TODO Implement this function
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    imagefile.close();

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    emit signalProgressInfo( i18n("Upload the metadata of %1", KUrl(m_path).fileName()) );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

void PiwigoTalker::parseResponseAddPhotoSummary(const QByteArray& data)
{
    QString str = QString::fromUtf8(data);
    QXmlStreamReader ts(data.mid(data.indexOf("<?xml")));
    QString line;
    bool foundResponse = false;
    bool success       = false;

    kDebug() << "parseResponseAddPhotoSummary: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp")
            {
                foundResponse = true;
                if (ts.attributes().value("stat") == "ok") success = true;
                break;
            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
        return;
    }

    if (m_path.size())
        QFile(m_path).remove();

    if (m_thumbpath.size())
        QFile(m_thumbpath).remove();

    m_path      = "";
    m_thumbpath = "";

    emit signalAddPhotoSucceeded();
}

/**
 * Support for Web API < 2.4
 */

void PiwigoTalker::parseResponseOldAddPhoto(const QByteArray& data)
{
    QString str = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    kDebug() << "parseResponseAddPhoto: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp")
            {
                foundResponse = true;
                if (ts.attributes().value("stat") == "ok") success = true;
                break;
            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
        return;
    }

    m_state = GE_OLD_ADDTHUMB;
    m_talker_buffer.resize(0);

    QFile imagefile(m_thumbpath);
    imagefile.open(QIODevice::ReadOnly);

    QStringList qsl;
    qsl.append("method=pwg.images.addChunk");
    qsl.append("original_sum=" + m_md5sum.toHex());
    qsl.append("position=1");
    qsl.append("type=thumb");
    qsl.append("data=" + imagefile.readAll().toBase64().toPercentEncoding());
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    imagefile.close();

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    emit signalProgressInfo( i18n("Upload the thumbnail of %1", KUrl(m_path).fileName()) );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

void PiwigoTalker::addHQNextChunk()
{
    QFile imagefile(m_hqpath);
    imagefile.open(QIODevice::ReadOnly);

    m_chunkId++; // We start with chunk 1

    imagefile.seek((m_chunkId - 1) * CHUNK_MAX_SIZE);

    m_talker_buffer.resize(0);
    QStringList qsl;
    qsl.append("method=pwg.images.addChunk");
    qsl.append("original_sum=" + m_md5sum.toHex());
    qsl.append("position=" + QString::number(m_chunkId));
    qsl.append("type=high");
    qsl.append("data=" + imagefile.read(CHUNK_MAX_SIZE).toBase64().toPercentEncoding());
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    imagefile.close();

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    emit signalProgressInfo( i18n("Upload the chunk %1 of %2", m_chunkId, KUrl(m_path).fileName()) );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

void PiwigoTalker::parseResponseOldAddThumbnail(const QByteArray& data)
{
    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    kDebug() << "parseResponseAddThumbnail: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp")
            {
                foundResponse = true;
                if (ts.attributes().value("stat") == "ok") success = true;
                break;
            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
        return;
    }

    if (m_hqpath.isNull() || m_hqpath.isEmpty())
    {
        addOldPhotoSummary();
    }
    else
    {
        m_state = GE_OLD_ADDHQ;
        m_talker_buffer.resize(0);
        m_chunkId = 0;

        addHQNextChunk();
    }
}

void PiwigoTalker::parseResponseOldAddHQPhoto(const QByteArray& data)
{
    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    kDebug() << "parseResponseAddHQPhoto: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp")
            {
                foundResponse = true;
                if (ts.attributes().value("stat") == "ok") success = true;
                break;
            }
        }
    }

    if (!foundResponse || !success)
    {
        emit signalProgressInfo(i18n("Warning : The full size photo cannot be uploaded."));
    }

    // If the HQ photo wasn't completely sent, send the next chunk
    QFileInfo fi(m_hqpath);
    if (m_chunkId * CHUNK_MAX_SIZE < fi.size())
    {
        addHQNextChunk();
    }
    else
    {
        addOldPhotoSummary();
    }
}

void PiwigoTalker::addOldPhotoSummary()
{
    m_state = GE_OLD_ADDPHOTOSUMMARY;
    m_talker_buffer.resize(0);

    QFile imagefile(m_thumbpath);
    imagefile.open(QIODevice::ReadOnly);

    QStringList qsl;
    qsl.append("method=pwg.images.add");
    qsl.append("original_sum=" + m_md5sum.toHex());
    qsl.append("name=" + m_title.toUtf8().toPercentEncoding());
    qsl.append("categories=" + QString::number(m_albumId));
    if (!m_author.isEmpty()) qsl.append("author=" + m_author.toUtf8().toPercentEncoding());
    if (!m_comment.isEmpty()) qsl.append("comment=" + m_comment.toUtf8().toPercentEncoding());
    qsl.append("file_sum=" + computeMD5Sum(m_path).toHex());
    qsl.append("thumbnail_sum=" + computeMD5Sum(m_thumbpath).toHex());
    if (!m_hqpath.isNull() && !m_hqpath.isEmpty())
    {
        qsl.append("high_sum=" + m_md5sum.toHex());
    }
    qsl.append("date_creation=" + m_date.toString("yyyy-MM-dd").toUtf8().toPercentEncoding());
    //qsl.append("tag_ids="); // TODO Implement this function
    
    QString dataParameters = qsl.join("&");
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    imagefile.close();

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );
    m_job->addMetaData("customHTTPHeader", "Authorization: " + s_authToken );

    emit signalProgressInfo( i18n("Upload the metadata of %1", KUrl(m_path).fileName()) );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

void PiwigoTalker::parseResponseOldAddPhotoSummary(const QByteArray& data)
{
    QString str = QString::fromUtf8(data);
    QXmlStreamReader ts(data.mid(data.indexOf("<?xml")));
    QString line;
    bool foundResponse = false;
    bool success       = false;

    kDebug() << "parseResponseAddPhotoSummary: " << QString(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == "rsp")
            {
                foundResponse = true;
                if (ts.attributes().value("stat") == "ok") success = true;
                break;
            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
        return;
    }

    if (m_path.size())
        QFile(m_path).remove();

    if (m_thumbpath.size())
        QFile(m_thumbpath).remove();

    m_path      = "";
    m_thumbpath = "";

    emit signalAddPhotoSucceeded();
}

} // namespace KIPIPiwigoExportPlugin
