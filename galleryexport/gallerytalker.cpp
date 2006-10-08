/* ============================================================
 * File  : gallerytalker.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Description :
 *
 * Copyright 2004 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <qcstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qimage.h>
#include <qregexp.h>

#include <klocale.h>
#include <kio/job.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kstandarddirs.h>

#include <cstring>
#include <cstdio>

#include "galleryitem.h"
#include "gallerympform.h"
#include "gallerytalker.h"
#include "exiv2iface.h"

namespace KIPIGalleryExportPlugin
{

GalleryTalker::GalleryTalker( QWidget* parent )
    : m_parent( parent ),  m_job( 0 ),  m_loggedIn( false )
{
}

GalleryTalker::~GalleryTalker()
{
    if (m_job)
        m_job->kill();
}

bool GalleryTalker::s_using_gallery2 = true;

bool GalleryTalker::loggedIn() const
{
    return m_loggedIn;
}

void GalleryTalker::login( const KURL& url, const QString& name,
                           const QString& passwd )
{
    m_url = url;

    GalleryMPForm form;

    form.addPair("cmd",              "login");
    form.addPair("protocol_version", "2.3");
    form.addPair("uname",            name);
    form.addPair("password",         passwd);
    form.finish();

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType() );
    job->addMetaData("cookies", "manual");
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    m_state = GE_LOGIN;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void GalleryTalker::listAlbums()
{
    GalleryMPForm form;

    QString task = "fetch-albums";
    if (s_using_gallery2)
      task = "fetch-albums-prune";

    form.addPair("cmd",              task);
    form.addPair("protocol_version", "2.3");
    form.finish();

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType() );
    job->addMetaData("cookies", "manual");
    job->addMetaData("setcookies", m_cookie);
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    m_state = GE_LISTALBUMS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void GalleryTalker::listPhotos( const QString& albumName )
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    GalleryMPForm form;

    form.addPair("cmd",              "fetch-album-images");
    form.addPair("protocol_version", "2.3");
    form.addPair("set_albumName",    albumName);
    form.finish();

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType() );
    job->addMetaData("cookies", "manual");
    job->addMetaData("setcookies", m_cookie);
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    m_state = GE_LISTPHOTOS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

void GalleryTalker::createAlbum( const QString& parentAlbumName,
                                 const QString& albumName,
                                 const QString& albumTitle,
                                 const QString& albumCaption )
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    GalleryMPForm form;

    form.addPair("cmd", "new-album");
    form.addPair("protocol_version", "2.3");
    form.addPair("set_albumName", parentAlbumName);
    if (!albumName.isEmpty())
        form.addPair("newAlbumName", albumName);
    if (!albumTitle.isEmpty())
        form.addPair("newAlbumTitle", albumTitle);
    if (!albumCaption.isEmpty())
        form.addPair("newAlbumDesc", albumCaption);
    form.finish();

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType() );
    job->addMetaData("cookies", "manual");
    job->addMetaData("setcookies", m_cookie);
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    m_state = GE_CREATEALBUM;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );
}

bool GalleryTalker::addPhoto( const QString& albumName,
                              const QString& photoPath,
                              const QString& caption,
                              bool  rescale, int maxDim )
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    QString path = photoPath;
    QString display_filename = QFile::encodeName(KURL(path).filename());

    GalleryMPForm form;

    form.addPair("cmd", "add-item");
    form.addPair("protocol_version", "2.3");
    form.addPair("set_albumName", albumName);

    if (!caption.isEmpty())
      form.addPair("caption", caption);
    QImage image(photoPath);

    if (!image.isNull())
    {
        // image file - see if we need to rescale it
        if (rescale && (image.width() > maxDim || image.height() > maxDim))
        {
            image = image.smoothScale(maxDim, maxDim, QImage::ScaleMin);
            path = locateLocal("tmp", KURL(photoPath).filename());
            image.save(path, QImageIO::imageFormat(photoPath));

            if ("JPEG" == QString(QImageIO::imageFormat(photoPath)).upper())
            {
              KIPIPlugins::Exiv2Iface exiv2;
              if (exiv2.load(photoPath))
              {
                exiv2.save(path);
              }
            }
            kdDebug() << "Resizing and saving to temp file: "
                      << path << endl;
        }
    }

    // The filename bit can perhaps be calculated in addFile()
    // but not sure of the temporary filename that could be
    // used for resizing... so I've added it explicitly for now.
    if (!form.addFile(path, display_filename))
        return false;

    form.finish();

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("cookies", "manual");
    job->addMetaData("setcookies", m_cookie);
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    m_state = GE_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy( true );

    return true;
}

void GalleryTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
}

void GalleryTalker::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldSize, data.data(), data.size());
}

void GalleryTalker::slotResult(KIO::Job *job)
{
    m_job = 0;
    emit signalBusy( false );

    if ( job->error() )
    {
        if ( m_state == GE_LOGIN )
            emit signalLoginFailed( job->errorString() );
        else if ( m_state == GE_ADDPHOTO )
            emit signalAddPhotoFailed( job->errorString() );
        else
            job->showErrorDialog( m_parent );
        return;
    }

    switch(m_state)
    {
    case(GE_LOGIN):
        parseResponseLogin(m_buffer);
        break;
    case(GE_LISTALBUMS):
        parseResponseListAlbums(m_buffer);
        break;
    case(GE_LISTPHOTOS):
        parseResponseListPhotos(m_buffer);
        break;
    case(GE_CREATEALBUM):
        parseResponseCreateAlbum(m_buffer);
        break;
    case(GE_ADDPHOTO):
        parseResponseAddPhoto(m_buffer);
        break;
    }

    if (m_state == GE_LOGIN && m_loggedIn)
    {
        QStringList cookielist = QStringList::split("\n", job->queryMetaData("setcookies"));
        m_cookie = "Cookie:";
        for (QStringList::Iterator it = cookielist.begin(); it != cookielist.end(); ++it)
        {
            QRegExp rx("^Set-Cookie: ([^;]+)");
            if (rx.search(*it) > -1)
                m_cookie += " " + rx.cap(1) + ";";
        }
        listAlbums();
    }
}

void GalleryTalker::parseResponseLogin(const QByteArray &data)
{
    QTextStream ts(data, IO_ReadOnly );
    ts.setEncoding(QTextStream::UnicodeUTF8);
    QString     line;
    bool foundResponse = false;

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            foundResponse = line.startsWith("#__GR2PROTO__");
        }
        else
        {
            QStringList strlist = QStringList::split("=", line);
            if (strlist.count() == 2)
            {
                m_loggedIn = (strlist[0] == "status") &&
                             (strlist[1] == "0");
                if (m_loggedIn)
                    break;
            }
        }
    }

    if (!foundResponse)
    {
        emit signalLoginFailed( i18n("Gallery URL probably incorrect"));
        return;
    }

    if (!m_loggedIn)
    {
        emit signalLoginFailed(i18n("Incorrect username or password specified"));
    }
}

void GalleryTalker::parseResponseListAlbums(const QByteArray &data)
{
    QTextStream ts(data, IO_ReadOnly );
    ts.setEncoding(QTextStream::UnicodeUTF8);
    QString     line;
    bool foundResponse = false;
    bool success       = false;

    typedef QValueList<GAlbum> GAlbumList;
    GAlbumList albumList;
    GAlbumList::iterator iter = albumList.begin();

    while (!ts.atEnd())
    {
        line = ts.readLine();
        if (!foundResponse)
        {
            foundResponse = line.startsWith("#__GR2PROTO__");
        }
        else
        {
            QStringList strlist = QStringList::split("=", line);
            if (strlist.count() == 2)
            {
                QString key   = strlist[0];
                QString value = strlist[1];

                if (key == "status")
                {
                    success = (value == "0");
                }
                else if (key.startsWith("album.name"))
                {
                    GAlbum album;
                    album.name    = value;
                    if (s_using_gallery2)
                        album.ref_num = value.toInt();
                    else
                        album.ref_num = key.section(".", 2, 2).toInt();
                    iter = albumList.append(album);
                }
                else if (key.startsWith("album.title"))
                {
                    if (iter != albumList.end())
                        (*iter).title = value;
                }
                else if (key.startsWith("album.summary"))
                {
                    if (iter != albumList.end())
                        (*iter).summary = value;
                }
                else if (key.startsWith("album.parent"))
                {
                    if (iter != albumList.end())
                        (*iter).parent_ref_num = value.toInt();
                }
                else if (key.startsWith("album.perms.add"))
                {
                    if (iter != albumList.end())
                        (*iter).add = (value == "true");
                }
                else if (key.startsWith("album.perms.write"))
                {
                    if (iter != albumList.end())
                        (*iter).write = (value == "true");
                }
                else if (key.startsWith("album.perms.del_item"))
                {
                    if (iter != albumList.end())
                        (*iter).del_item = (value == "true");
                }
                else if (key.startsWith("album.perms.del_alb"))
                {
                    if (iter != albumList.end())
                        (*iter).del_alb = (value == "true");
                }
                else if (key.startsWith("album.perms.create_sub"))
                {
                    if (iter != albumList.end())
                        (*iter).create_sub = (value == "true");
                }
            }
        }
    }

    if (!foundResponse)
    {
        emit signalError(i18n("Invalid response received from remote Gallery"));
        return;
    }

    if (!success)
    {
        emit signalError(i18n("Failed to list albums"));
        return;
    }

    // We need parent albums to come first for rest of the code to work
    qHeapSort(albumList);

    emit signalAlbums( albumList );
}

void GalleryTalker::parseResponseListPhotos(const QByteArray &data)
{
    QTextStream ts(data, IO_ReadOnly );
    ts.setEncoding(QTextStream::UnicodeUTF8);
    QString     line;
    bool foundResponse = false;
    bool success       = false;

    typedef QValueList<GPhoto> GPhotoList;
    GPhotoList photoList;
    GPhotoList::iterator iter = photoList.begin();

    QString albumURL;

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            foundResponse = line.startsWith("#__GR2PROTO__");
        }
        else
        {
        	// Boris the Gallery default URL contains "=" char. So we will split the string only from the first "=" char
            QStringList strlist = QStringList();
            strlist << line.left(line.find('=')) << line.mid(line.find('=')+1);
            if (strlist.count() >= 2)
            {
                QString key   = strlist[0];
                QString value = strlist[1];

                if (key == "status")
                {
                    success = (value == "0");
                }
                else if (key.startsWith("image.name"))
                {
                    GPhoto photo;
                    photo.name    = value;
                    photo.ref_num = key.section(".", 2, 2).toInt();
                    iter = photoList.append(photo);
                }
                else if (key.startsWith("image.caption"))
                {
                    if (iter != photoList.end())
                        (*iter).caption = value;
                }
                else if (key.startsWith("image.thumbName"))
                {
                    if (iter != photoList.end())
                        (*iter).thumbName = value;
                }
                else if (key.startsWith("baseurl"))
                {
                    albumURL = value.replace("\\","");
                }
            }
        }
    }

    if (!foundResponse)
    {
        emit signalError(i18n("Invalid response received from remote Gallery"));
        return;
    }

    if (!success)
    {
        emit signalError(i18n("Failed to list photos"));
        return;
    }

    for ( iter = photoList.begin(); iter != photoList.end(); ++iter )
    {
        (*iter).albumURL = albumURL;
    }

    emit signalPhotos( photoList );
}

void GalleryTalker::parseResponseCreateAlbum(const QByteArray &data)
{
    QTextStream ts(data, IO_ReadOnly );
    ts.setEncoding(QTextStream::UnicodeUTF8);
    QString     line;
    bool foundResponse = false;
    bool success       = false;

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            foundResponse = line.startsWith("#__GR2PROTO__");
        }
        else
        {
            QStringList strlist = QStringList::split("=", line);
            if (strlist.count() == 2)
            {
                QString key   = strlist[0];
                QString value = strlist[1];

                if (key == "status")
                {
                    success = (value == "0");
                }
                else if (key.startsWith("status_text"))
                {
                    kdDebug() << "STATUS: Create Album: " << value << endl;
                }

            }
        }
    }

    if (!foundResponse)
    {
        emit signalError(i18n("Invalid response received from remote Gallery"));
        return;
    }

    if (!success)
    {
        emit signalError(i18n("Failed to create new album"));
        return;
    }

    listAlbums();
}

void GalleryTalker::parseResponseAddPhoto(const QByteArray &data)
{
    QTextStream ts(data, IO_ReadOnly );
    ts.setEncoding(QTextStream::UnicodeUTF8);
    QString     line;
    bool foundResponse = false;
    bool success       = false;

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            // Gallery1 sends resizing debug code sometimes so we
            // have to detect things slightly differently
            foundResponse = (line.startsWith("#__GR2PROTO__")
                             || (line.startsWith("<br>- Resizing")
                                 && line.endsWith("#__GR2PROTO__")));
        }
        else
        {
            QStringList strlist = QStringList::split("=", line);
            if (strlist.count() == 2)
            {
                QString key   = strlist[0];
                QString value = strlist[1];

                if (key == "status")
                {
                    success = (value == "0");
                }
                else if (key.startsWith("status_text"))
                {
                    kdDebug() << "STATUS: Add Photo: " << value << endl;
                }

            }
        }
    }

    if (!foundResponse)
    {
        emit signalAddPhotoFailed(i18n("Invalid response received from remote Gallery"));
        return;
    }

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }
}

}

#include "gallerytalker.moc"

