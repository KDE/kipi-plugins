/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.kipi-plugins.org
*
* Date        : 2010-02-15
* Description : a plugin to export to a remote Piwigo server.
*
* Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
* Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
* Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
* Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2010 by Frederic Coiffier <frederic dot coiffier at free dot com>
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
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "piwigoitem.h"
#include "pluginsversion.h"

namespace KIPIPiwigoExportPlugin
{

QString PiwigoTalker::s_authToken   = "";

PiwigoTalker::PiwigoTalker(QWidget* parent)
    : m_parent(parent),  m_job(0),  m_loggedIn(false)
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
                            const QString& caption,
                            bool  captionIsTitle,
                            bool  captionIsDescription,
                            bool  rescale,
                            bool  downloadHQ, 
                            int   maxDim,
                            int   thumbDim)
{
    m_job        = 0;
    m_state      = GE_CHECKPHOTOEXIST;
    m_talker_buffer.resize(0);

    m_path    = photoPath;
    m_albumId = albumId;
    m_md5sum  = computeMD5Sum(photoPath);

    if (downloadHQ)
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
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
    QFileInfo fi(photoPath);

    if (rawFilesExt.toUpper().contains( fi.suffix().toUpper() ))
        KDcrawIface::KDcraw::loadDcrawPreview(image, photoPath);
    else
        image.load(photoPath);

    if (!image.isNull())
    {
        QImage thumbnail = image.scaled(thumbDim, thumbDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_thumbpath      = KStandardDirs::locateLocal("tmp", "thumb-" + KUrl(photoPath).fileName());
        thumbnail.save(m_thumbpath, "JPEG", 95);
        kDebug() << "Thumbnail to temp file: " << m_thumbpath ;


        // image file - see if we need to rescale it
        if (rescale && (image.width() > maxDim || image.height() > maxDim))
        {
            image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        m_path = KStandardDirs::locateLocal("tmp", KUrl(photoPath).fileName());
        image.save(m_path, "JPEG", 95);
        kDebug() << "Resizing and saving to temp file: " << m_path ;

        // Complete name and comment for summary sending
        m_comment = m_name = caption;
        m_date    = fi.created();

        // Restore all metadata.
        KExiv2Iface::KExiv2 exiv2Iface;

        if (exiv2Iface.load(photoPath))
        {
            exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            exiv2Iface.setImageDimensions(image.size());
            exiv2Iface.save(m_path);
            kDebug() << "Comment : " << exiv2Iface.getExifComment();
            if (exiv2Iface.getExifComment().length())
            {
                if (captionIsTitle) m_name = exiv2Iface.getExifComment();
                if (captionIsDescription) m_comment = exiv2Iface.getExifComment();
            }

            if (!exiv2Iface.getImageDateTime().isNull())
            {
                m_date = exiv2Iface.getImageDateTime();
            }
        }
        else
        {
            kWarning() << "Image " << photoPath << " has no exif data";
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

    if (tempjob->error())
    {
        if (m_state == GE_LOGIN)
        {
            emit signalLoginFailed(tempjob->errorString());
            kDebug() << tempjob->errorString();
        }
        else
        {
            if (m_state == GE_CHECKPHOTOEXIST || m_state == GE_ADDPHOTO || m_state == GE_ADDTHUMB || 
                m_state == GE_ADDHQ           || m_state == GE_ADDPHOTOSUMMARY)
            {
                emit signalAddPhotoFailed(tempjob->errorString());
            }
            else
            {
                tempjob->ui()->setWindow(m_parent);
                tempjob->ui()->showErrorMessage();
            }
        }
        emit signalBusy(false);
        return;
    }

    switch (m_state)
    {
    case(GE_LOGIN):
        parseResponseLogin(m_talker_buffer);
        break;
    case(GE_LISTALBUMS):
        parseResponseListAlbums(m_talker_buffer);
        break;
    case(GE_CHECKPHOTOEXIST):
        parseResponseDoesPhotoExist(m_talker_buffer);
        break;
    case(GE_ADDPHOTO):
        parseResponseAddPhoto(m_talker_buffer);
        break;
    case(GE_ADDTHUMB):
        parseResponseAddThumbnail(m_talker_buffer);
        break;
    case(GE_ADDHQ):
        parseResponseAddHQPhoto(m_talker_buffer);
        break;
    case(GE_ADDPHOTOSUMMARY):
        parseResponseAddPhotoSummary(m_talker_buffer);
        break;
    }

    tempjob->kill();
    m_job = 0;

    if (m_state == GE_LOGIN && m_loggedIn)
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

    kDebug() << "parseResponseListAlbums: " << QString(data);

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
                QString uppercats = ts.readElementText();
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

    m_state = GE_ADDPHOTO;

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

void PiwigoTalker::parseResponseAddPhoto(const QByteArray& data)
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

    m_state = GE_ADDTHUMB;

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

void PiwigoTalker::parseResponseAddThumbnail(const QByteArray& data)
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
        addPhotoSummary();
    }
    else
    {
        m_state = GE_ADDHQ;
        m_chunkId = 0;

        addHQNextChunk();
    }
}

void PiwigoTalker::parseResponseAddHQPhoto(const QByteArray& data)
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
        addPhotoSummary();
    }
}

void PiwigoTalker::addPhotoSummary()
{
    m_state = GE_ADDPHOTOSUMMARY;

    QFile imagefile(m_thumbpath);
    imagefile.open(QIODevice::ReadOnly);

    QStringList qsl;
    qsl.append("method=pwg.images.add");
    qsl.append("original_sum=" + m_md5sum.toHex());
    qsl.append("name=" + m_name.toUtf8().toPercentEncoding());
    qsl.append("author="); // TODO Retrieve author name from EXIF/IPTC tags
    qsl.append("categories=" + QString::number(m_albumId));
    qsl.append("file_sum=" + computeMD5Sum(m_path).toHex());
    qsl.append("thumbnail_sum=" + computeMD5Sum(m_thumbpath).toHex());
    if (!m_hqpath.isNull() && !m_hqpath.isEmpty())
    {
        qsl.append("high_sum=" + m_md5sum.toHex());
    }
    qsl.append("date_creation=" + m_date.toString("yyyy-MM-dd").toUtf8().toPercentEncoding());
    qsl.append("tag_ids=");
    qsl.append("comment=" + m_comment.toUtf8().toPercentEncoding());
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
    QXmlStreamReader ts(data);
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

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (m_path.size())
        QFile(m_path).remove();
    if (m_thumbpath.size())
        QFile(m_thumbpath).remove();

    m_path      = "";
    m_thumbpath = "";

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }
}

} // namespace KIPIPiwigoExportPlugin
