/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2014-09-30
* Description : a plugin to export to a remote Piwigo server.
*
* Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
* Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
* Copyright (C) 2006-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
* Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2010-2014 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#include "piwigotalker.h"

// Qt includes

#include <QByteArray>
#include <QImage>
#include <QRegExp>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QUuid>

// KDE includes

#include <kjobwidgets.h>
#include <kio/jobuidelegate.h>
#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"
#include "piwigoitem.h"
#include "kpversion.h"
#include "kpimageinfo.h"
#include "kputil.h"

using namespace KIPIPlugins;

namespace KIPIPiwigoExportPlugin
{

QString PiwigoTalker::s_authToken = QStringLiteral("");

PiwigoTalker::PiwigoTalker(QWidget* const parent)
    : m_parent(parent),
      m_state(GE_LOGOUT),
      m_job(0),
      m_loggedIn(false),
      m_chunkId(0),
      m_nbOfChunks(0),
      m_version(-1),
      m_albumId(0),
      m_photoId(0),
      m_iface(0)
{
    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
    }
}

PiwigoTalker::~PiwigoTalker()
{
    cancel();
}

void PiwigoTalker::cancel()
{
    deleteTemporaryFile();

    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
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

void PiwigoTalker::login(const QUrl& url, const QString& name, const QString& passwd)
{
    m_job   = 0;
    m_url   = url;
    m_state = GE_LOGIN;
    m_talker_buffer.resize(0);

    // Add the page to the URL
    if (!m_url.url().endsWith(QLatin1String(".php")))
    {
        m_url.setPath(m_url.path() + QLatin1Char('/') + QLatin1String("ws.php"));
    }

    s_authToken = QString::fromLatin1(QUuid::createUuid().toByteArray().toBase64());

    QStringList qsl;
    qsl.append(QLatin1String("password=") + QString::fromUtf8(passwd.toUtf8().toPercentEncoding()));
    qsl.append(QLatin1String("method=pwg.session.login"));
    qsl.append(QLatin1String("username=") + QString::fromUtf8(name.toUtf8().toPercentEncoding()));
    QString dataParameters = qsl.join(QLatin1String("&"));
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());
    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData(QStringLiteral("content-type"),
                       QStringLiteral("Content-Type: application/x-www-form-urlencoded"));
    m_job->addMetaData(QStringLiteral("customHTTPHeader"),
                       QStringLiteral("Authorization: ") + s_authToken);

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
    qsl.append(QStringLiteral("method=pwg.categories.getList"));
    qsl.append(QStringLiteral("recursive=true"));
    QString dataParameters = qsl.join(QStringLiteral("&"));
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData(QStringLiteral("content-type"),
                       QStringLiteral("Content-Type: application/x-www-form-urlencoded"));
    m_job->addMetaData(QStringLiteral("customHTTPHeader"),
                       QStringLiteral("Authorization: ") + s_authToken);

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);
}

bool PiwigoTalker::addPhoto(int   albumId,
                            const QString& mediaPath,
                            bool  rescale,
                            int   maxWidth,
                            int   maxHeight,
                            int   quality)
{
    m_job         = 0;
    m_state       = GE_CHECKPHOTOEXIST;
    m_talker_buffer.resize(0);

    m_path    = mediaPath; // By default, m_path contains the original file
    m_tmpPath = QStringLiteral(""); // By default, no temporary file (except with rescaling)
    m_albumId = albumId;

    m_md5sum  = computeMD5Sum(mediaPath);

    qCDebug(KIPIPLUGINS_LOG) << mediaPath << " " << m_md5sum.toHex();

    if (mediaPath.endsWith(QStringLiteral(".mp4"))  || mediaPath.endsWith(QStringLiteral(".MP4")) ||
        mediaPath.endsWith(QStringLiteral(".ogg"))  || mediaPath.endsWith(QStringLiteral(".OGG")) ||
        mediaPath.endsWith(QStringLiteral(".webm")) || mediaPath.endsWith(QStringLiteral(".WEBM")))
    {
        // Video management
        // Nothing to do
    }
    else
    {
        // Image management
        QImage image;

        if (m_iface)
        {
            QPointer<RawProcessor> rawdec = m_iface->createRawProcessor();

            // check if its a RAW file.
            if (rawdec && rawdec->isRawFile(QUrl::fromLocalFile(mediaPath)))
            {
                rawdec->loadRawPreview(QUrl::fromLocalFile(mediaPath), image);
            }
        }

        if (image.isNull())
        {
            image.load(mediaPath);
        }

        if (image.isNull())
        {
            // Invalid image
            return false;
        }

        if (!rescale)
        {
            qCDebug(KIPIPLUGINS_LOG) << "Upload the original version: " << m_path;
        }
        else
        {
            // Rescale the image
            if (image.width() > maxWidth || image.height() > maxHeight)
            {
                image = image.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            m_path = m_tmpPath = makeTemporaryDir("kipi-piwigo").filePath(QUrl::fromLocalFile(mediaPath).fileName());
            image.save(m_path, "JPEG", quality);

            qCDebug(KIPIPLUGINS_LOG) << "Upload a resized version: " << m_path ;

            // Restore all metadata with EXIF
            // in the resized version
            if (m_iface)
            {
                QPointer<MetadataProcessor> meta = m_iface->createMetadataProcessor();
            
                if (meta && meta->load(QUrl::fromLocalFile(mediaPath)))
                {
                    meta->setImageProgramId(QStringLiteral("Kipi-plugins"), kipipluginsVersion());
                    meta->setImageDimensions(image.size());
                    meta->save(QUrl::fromLocalFile(m_path));
                }
                else
                {
                    qCDebug(KIPIPLUGINS_LOG) << "Image " << mediaPath << " has no exif data";
                }
            }
        }
    }

    // Metadata management

    // Complete name and comment for summary sending
    QFileInfo fi(mediaPath);
    m_title   = fi.completeBaseName();
    m_comment = QStringLiteral("");
    m_author  = QStringLiteral("");
    m_date    = fi.created();

    // Look in the Kipi host database
    KPImageInfo info(QUrl::fromLocalFile(mediaPath));

    if (info.hasTitle() && !info.title().isEmpty())
        m_title = info.title();

    if (info.hasDescription() && !info.description().isEmpty())
        m_comment = info.description();

    if (info.hasCreators() && !info.creators().isEmpty())
        m_author = info.creators().join(QStringLiteral(" / "));

    if (info.hasDate())
        m_date = info.date();

    qCDebug(KIPIPLUGINS_LOG) << "Title: " << m_title;
    qCDebug(KIPIPLUGINS_LOG) << "Comment: " << m_comment;
    qCDebug(KIPIPLUGINS_LOG) << "Author: " << m_author;
    qCDebug(KIPIPLUGINS_LOG) << "Date: " << m_date;

    QStringList qsl;
    qsl.append(QLatin1String("method=pwg.images.exist"));
    qsl.append(QLatin1String("md5sum_list=") + QString::fromLatin1(m_md5sum.toHex()));
    QString dataParameters = qsl.join(QLatin1String("&"));
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData(QStringLiteral("content-type"),
                       QStringLiteral("Content-Type: application/x-www-form-urlencoded"));
    m_job->addMetaData(QStringLiteral("customHTTPHeader"),
                       QStringLiteral("Authorization: ") + s_authToken);

    emit signalProgressInfo( i18n("Check if %1 already exists", QUrl(mediaPath).fileName()) );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);

    return true;
}

void PiwigoTalker::slotTalkerData(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

	m_talker_buffer.append(data);
}

void PiwigoTalker::slotResult(KJob* job)
{
    KIO::Job* const tempjob = static_cast<KIO::Job*>(job);
    State state             = m_state; // Can change in the treatment itself
                                       // so we cache it

    if (tempjob->error())
    {
        if (state == GE_LOGIN)
        {
            emit signalLoginFailed(tempjob->errorString());
            qCDebug(KIPIPLUGINS_LOG) << tempjob->errorString();
        }
        else if (state == GE_GETVERSION)
        {
            qCDebug(KIPIPLUGINS_LOG) << tempjob->errorString();
            // Version isn't mandatory and errors can be ignored
            // As login succeeded, albums can be listed
            listAlbums();
        }
        else if (state == GE_CHECKPHOTOEXIST || state == GE_GETINFO           ||
                 state == GE_SETINFO         || state == GE_ADDPHOTOCHUNK     ||
                 state == GE_ADDPHOTOSUMMARY)
        {
            deleteTemporaryFile();
            emit signalAddPhotoFailed(tempjob->errorString());
        }
        else
        {
            KJobWidgets::setWindow(tempjob, m_parent);
            tempjob->ui()->showErrorMessage();
        }

        emit signalBusy(false);
        m_job = 0;
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
        case(GE_ADDPHOTOCHUNK):
            // Support for Web API >= 2.4
            parseResponseAddPhotoChunk(m_talker_buffer);
            break;
        case(GE_ADDPHOTOSUMMARY):
            parseResponseAddPhotoSummary(m_talker_buffer);
            break;
        default:   // GE_LOGOUT
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
    m_job   = 0;

    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    m_loggedIn         = false;

    qCDebug(KIPIPLUGINS_LOG) << "parseResponseLogin: " << QString::fromUtf8(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            foundResponse = true;

            if (ts.name() == QStringLiteral("rsp") && ts.attributes().value(QStringLiteral("stat")) == QStringLiteral("ok"))
            {
                m_loggedIn = true;

                /** Request Version */
                m_state   = GE_GETVERSION;
                m_talker_buffer.resize(0);
                m_version = -1;

                QByteArray buffer = "method=pwg.getVersion";
                m_job             = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
                m_job->addMetaData(QStringLiteral("content-type"),
                                   QStringLiteral("Content-Type: application/x-www-form-urlencoded"));
                m_job->addMetaData(QStringLiteral("customHTTPHeader"),
                                   QStringLiteral("Authorization: ") + s_authToken);

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
    QRegExp verrx(QStringLiteral(".?(\\d)\\.(\\d).*"));

    bool foundResponse = false;

    qCDebug(KIPIPLUGINS_LOG) << "parseResponseGetVersion: " << QString::fromUtf8(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            foundResponse = true;

            if (ts.name() == QStringLiteral("rsp") && ts.attributes().value(QStringLiteral("stat")) == QStringLiteral("ok"))
            {
                QString v = ts.readElementText();

                if (verrx.exactMatch(v))
                {
                    QStringList qsl = verrx.capturedTexts();
                    m_version       = qsl[1].toInt() * 10 + qsl[2].toInt();
                    qCDebug(KIPIPLUGINS_LOG) << "Version: " << m_version;
                    break;
                }
            }
        }
    }

    qCDebug(KIPIPLUGINS_LOG) << "foundResponse : " << foundResponse;

    if (m_version < PIWIGO_VER_2_4)
    {
        m_loggedIn = false;
        emit signalLoginFailed(i18n("Upload to Piwigo version < 2.4 is no longer supported"));
        return;
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

    qCDebug(KIPIPLUGINS_LOG) << "parseResponseListAlbums";

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isEndElement() && ts.name() == QStringLiteral("categories"))
            break;

        if (ts.isStartElement())
        {
            if (ts.name() == QStringLiteral("rsp") && ts.attributes().value(QStringLiteral("stat")) == QStringLiteral("ok"))
            {
                foundResponse = true;
            }

            if (ts.name() == QStringLiteral("categories"))
            {
                success = true;
            }

            if (ts.name() == QStringLiteral("category"))
            {
                GAlbum album;
                album.ref_num = ts.attributes().value(QStringLiteral("id")).toString().toInt();
                album.parent_ref_num = -1;

                qCDebug(KIPIPLUGINS_LOG) << album.ref_num << "\n";

                iter = albumList.insert(iter, album);
            }

            if (ts.name() == QStringLiteral("name"))
            {
                (*iter).name = ts.readElementText();
                qCDebug(KIPIPLUGINS_LOG) << (*iter).name << "\n";
            }

            if (ts.name() == QStringLiteral("uppercats"))
            {
                QString uppercats   = ts.readElementText();
                QStringList catlist = uppercats.split(QLatin1Char(','));

                if (catlist.size() > 1 && catlist.at(catlist.size() - 2).toInt() != (*iter).ref_num)
                {
                    (*iter).parent_ref_num = catlist.at(catlist.size() - 2).toInt();
                    qCDebug(KIPIPLUGINS_LOG) << (*iter).parent_ref_num << "\n";
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
    m_job   = 0;

    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    qCDebug(KIPIPLUGINS_LOG) << "parseResponseDoesPhotoExist: " << QString::fromUtf8(data);

    while (!ts.atEnd())
    {

        ts.readNext();

        if (ts.name() == QStringLiteral("rsp"))
        {
            foundResponse = true;

            if (ts.attributes().value(QStringLiteral("stat")) == QStringLiteral("ok"))
                success = true;

            // Originally, first versions of Piwigo 2.4.x returned an invalid XML as the element started with a digit
            // New versions are corrected (starting with _) : This code works with both versions
            QRegExp md5rx(QStringLiteral("_?([a-f0-9]+)>([0-9]+)</.+"));

            ts.readNext();

            if (md5rx.exactMatch(QString::fromUtf8(data.mid(ts.characterOffset()))))
            {
                QStringList qsl = md5rx.capturedTexts();

                if (qsl[1] == QString::fromLatin1(m_md5sum.toHex()))
                {
                    m_photoId = qsl[2].toInt();
                    qCDebug(KIPIPLUGINS_LOG) << "m_photoId: " << m_photoId;

                    emit signalProgressInfo(i18n("Photo '%1' already exists.", m_title));

                    m_state = GE_GETINFO;
                    m_talker_buffer.resize(0);

                    QStringList qsl;
                    qsl.append(QStringLiteral("method=pwg.images.getInfo"));
                    qsl.append(QStringLiteral("image_id=") + QString::number(m_photoId));
                    QString dataParameters = qsl.join(QStringLiteral("&"));
                    QByteArray buffer;
                    buffer.append(dataParameters.toUtf8());

                    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
                    m_job->addMetaData(QStringLiteral("content-type"),
                                       QStringLiteral("Content-Type: application/x-www-form-urlencoded"));
                    m_job->addMetaData(QStringLiteral("customHTTPHeader"),
                                       QStringLiteral("Authorization: ") + s_authToken);

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

    if (m_version >= PIWIGO_VER_2_4)
    {
        QFileInfo fi(m_path);

        m_state   = GE_ADDPHOTOCHUNK;
        m_talker_buffer.resize(0);
        // Compute the number of chunks for the image
        m_nbOfChunks = (fi.size() / CHUNK_MAX_SIZE) + 1;
        m_chunkId = 0;

        addNextChunk();
    }
    else
    {
        emit signalAddPhotoFailed(i18n("Upload to Piwigo version < 2.4 is no longer supported"));
        return;
    }
}

void PiwigoTalker::parseResponseGetInfo(const QByteArray& data)
{
    m_job   = 0;

    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;
    QList<int> categories;

    qCDebug(KIPIPLUGINS_LOG) << "parseResponseGetInfo: " << QString::fromUtf8(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == QStringLiteral("rsp"))
            {
                foundResponse = true;
                if (ts.attributes().value(QStringLiteral("stat")) == QStringLiteral("ok")) success = true;
            }

            if (ts.name() == QStringLiteral("category"))
            {
                if (ts.attributes().hasAttribute(QStringLiteral("id")))
                {
                    QString id(ts.attributes().value(QStringLiteral("id")).toString());
                    categories.append(id.toInt());
                }
            }
        }
    }

    qCDebug(KIPIPLUGINS_LOG) << "success : " << success;

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo"));
        return;
    }

    if (categories.contains(m_albumId))
    {
        emit signalAddPhotoFailed(i18n("Photo '%1' already exists in this album.", m_title));
        return;
    }
    else
    {
        categories.append(m_albumId);
    }

    m_state = GE_SETINFO;
    m_talker_buffer.resize(0);

    QStringList qsl_cat;

    for (int i = 0; i < categories.size(); ++i)
    {
        qsl_cat.append(QString::number(categories.at(i)));
    }

    QStringList qsl;
    qsl.append(QLatin1String("method=pwg.images.setInfo"));
    qsl.append(QLatin1String("image_id=") + QString::number(m_photoId));
    qsl.append(QLatin1String("categories=") + QString::fromUtf8(qsl_cat.join(QLatin1String(";")).toUtf8().toPercentEncoding()));
    QString dataParameters = qsl.join(QLatin1String("&"));
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData(QStringLiteral("content-type"),
                       QStringLiteral("Content-Type: application/x-www-form-urlencoded"));
    m_job->addMetaData(QStringLiteral("customHTTPHeader"),
                       QStringLiteral("Authorization: ") + s_authToken);

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    return;
}

void PiwigoTalker::parseResponseSetInfo(const QByteArray& data)
{
    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data);
    QString line;
    bool foundResponse = false;
    bool success       = false;

    qCDebug(KIPIPLUGINS_LOG) << "parseResponseSetInfo: " << QString::fromUtf8(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == QStringLiteral("rsp"))
            {
                foundResponse = true;
                if (ts.attributes().value(QStringLiteral("stat")) == QStringLiteral("ok")) success = true;
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

    deleteTemporaryFile();

    emit signalAddPhotoSucceeded();
}

void PiwigoTalker::addNextChunk()
{
    m_job   = 0;

    QFile imagefile(m_path);

    imagefile.open(QIODevice::ReadOnly);

    m_chunkId++; // We start with chunk 1

    imagefile.seek((m_chunkId - 1) * CHUNK_MAX_SIZE);

    m_talker_buffer.resize(0);
    QStringList qsl;
    qsl.append(QLatin1String("method=pwg.images.addChunk"));
    qsl.append(QLatin1String("original_sum=") + QString::fromLatin1(m_md5sum.toHex()));
    qsl.append(QLatin1String("position=") + QString::number(m_chunkId));
    qsl.append(QLatin1String("type=file"));
    qsl.append(QLatin1String("data=") + QString::fromUtf8(imagefile.read(CHUNK_MAX_SIZE).toBase64().toPercentEncoding()));
    QString dataParameters = qsl.join(QLatin1String("&"));
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    imagefile.close();

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData(QStringLiteral("content-type"),
                       QStringLiteral("Content-Type: application/x-www-form-urlencoded"));
    m_job->addMetaData(QStringLiteral("customHTTPHeader"),
                       QStringLiteral("Authorization: ") + s_authToken);

    emit signalProgressInfo( i18n("Upload the chunk %1/%2 of %3", m_chunkId, m_nbOfChunks, QUrl(m_path).fileName()) );

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

    qCDebug(KIPIPLUGINS_LOG) << "parseResponseAddPhotoChunk: " << QString::fromUtf8(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == QStringLiteral("rsp"))
            {
                foundResponse = true;
                if (ts.attributes().value(QStringLiteral("stat")) == QStringLiteral("ok")) success = true;
                break;
            }
        }
    }

    if (!foundResponse || !success)
    {
        emit signalProgressInfo(i18n("Warning : The full size photo cannot be uploaded."));
    }

    if (m_chunkId < m_nbOfChunks)
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
    m_job   = 0;
    m_state = GE_ADDPHOTOSUMMARY;
    m_talker_buffer.resize(0);

    QStringList qsl;
    qsl.append(QLatin1String("method=pwg.images.add"));
    qsl.append(QLatin1String("original_sum=") + QString::fromLatin1(m_md5sum.toHex()));
    qsl.append(QLatin1String("original_filename=") + QString::fromUtf8(QUrl(m_path).fileName().toUtf8().toPercentEncoding()));
    qsl.append(QLatin1String("name=") + QString::fromUtf8(m_title.toUtf8().toPercentEncoding()));
    if (!m_author.isEmpty()) qsl.append(QLatin1String("author=") + QString::fromUtf8(m_author.toUtf8().toPercentEncoding()));
    if (!m_comment.isEmpty()) qsl.append(QLatin1String("comment=") + QString::fromUtf8(m_comment.toUtf8().toPercentEncoding()));
    qsl.append(QLatin1String("categories=") + QString::number(m_albumId));
    qsl.append(QLatin1String("file_sum=") + QString::fromLatin1(computeMD5Sum(m_path).toHex()));
    qsl.append(QLatin1String("date_creation=") +
               QString::fromUtf8(m_date.toString(QLatin1String("yyyy-MM-dd hh:mm:ss")).toUtf8().toPercentEncoding()));
    //qsl.append("tag_ids="); // TODO Implement this function
    QString dataParameters = qsl.join(QLatin1String("&"));
    QByteArray buffer;
    buffer.append(dataParameters.toUtf8());

    m_job = KIO::http_post(m_url, buffer, KIO::HideProgressInfo);
    m_job->addMetaData(QStringLiteral("content-type"),
                       QStringLiteral("Content-Type: application/x-www-form-urlencoded"));
    m_job->addMetaData(QStringLiteral("customHTTPHeader"),
                       QStringLiteral("Authorization: ") + s_authToken);

    emit signalProgressInfo( i18n("Upload the metadata of %1", QUrl(m_path).fileName()) );

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

void PiwigoTalker::parseResponseAddPhotoSummary(const QByteArray& data)
{
    QString str        = QString::fromUtf8(data);
    QXmlStreamReader ts(data.mid(data.indexOf("<?xml")));
    QString line;
    bool foundResponse = false;
    bool success       = false;

    qCDebug(KIPIPLUGINS_LOG) << "parseResponseAddPhotoSummary: " << QString::fromUtf8(data);

    while (!ts.atEnd())
    {
        ts.readNext();

        if (ts.isStartElement())
        {
            if (ts.name() == QStringLiteral("rsp"))
            {
                foundResponse = true;
                if (ts.attributes().value(QStringLiteral("stat")) == QStringLiteral("ok")) success = true;
                break;
            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Piwigo (%1)", QString::fromUtf8(data)));
        return;
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
        return;
    }

    deleteTemporaryFile();

    emit signalAddPhotoSucceeded();
}

void PiwigoTalker::deleteTemporaryFile()
{
    if (m_tmpPath.size()) {
        QFile(m_tmpPath).remove();
        m_tmpPath = QStringLiteral("");
    }
}

} // namespace KIPIPiwigoExportPlugin
