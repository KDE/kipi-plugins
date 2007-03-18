/* ============================================================
 * File  : gallerysink.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Description :
 *
 * Copyright 2004 by Renchi Raju

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

// LibKExiv2 includes. 

#include <libkexiv2/libkexiv2.h>

#include "gallerysink.h"
#include "gallerycollection.h"
#include "galleryitem.h"

namespace KIPISyncPlugin
{

GallerySink::GallerySink(unsigned int sinkId, QString name, KConfig* pConfig, KWallet::Wallet* pWallet, GalleryVersion version)
  : Sink(sinkId, name, pConfig, pWallet),
    mVersion(version), 
    mAuthToken(""), 
    mpJob(0),
    m_loggedIn(false)
{
  QString tmp = pConfig->readEntry(QString("URL%1").arg(sinkId));
  mURL = KURL(tmp);
  mUsername = pConfig->readEntry(QString("Username%1").arg(sinkId));
  if (pWallet)
    pWallet->readPassword(QString("Password%1").arg(sinkId), mPassword);
}

GallerySink::~GallerySink()
{
  if (mpJob)
    mpJob->kill();
}


const KIPI2::CollectionList* GallerySink::getCollections()
{
  return NULL;
}
  
void GallerySink::Save(KConfig* pConfig, KWallet::Wallet* pWallet)
{
  pConfig->writeEntry(QString("Name%1").arg(mSinkId), mName);
  pConfig->writeEntry(QString("Type%1").arg(mSinkId), Type());
  pConfig->writeEntry(QString("URL%1").arg(mSinkId), QString(mURL.url()));
  pConfig->writeEntry(QString("Username%1").arg(mSinkId), mUsername);
  if (pWallet)
    pWallet->writePassword(QString("Password%1").arg(mSinkId), mPassword);
}


/*
bool GallerySink::loggedIn() const
{
    return m_loggedIn;
}
*/

bool GallerySink::Connect()
{
    GalleryForm form(mVersion, mAuthToken);

    form.addPair("cmd",              "login");
    form.addPair("protocol_version", "2.3");
    form.addPair("uname",            mUsername);
    form.addPair("password",         mPassword);

    KIO::TransferJob* job = KIO::http_post(mURL, form.formData(), false);
    job->addMetaData("content-type", form.contentType() );
    job->addMetaData("cookies", "manual");
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    mState = GE_LOGIN;
    mpJob   = job;
    mBuffer.resize(0);
    //emit signalBusy( true );
}
/*
void GallerySink::listAlbums()
{
    GalleryForm form(mVersion, mAuthToken);

    QString task = "fetch-albums";
    if (Gallery2 == mVersion)
      task = "fetch-albums-prune";

    form.addPair("cmd",              task);
    form.addPair("protocol_version", "2.3");

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType() );
    job->addMetaData("cookies", "manual");
    job->addMetaData("setcookies", m_cookie);
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    mState = GE_LISTALBUMS;
    mpJob   = job;
    mBuffer.resize(0);
    emit signalBusy( true );
}

void GallerySink::listPhotos( const QString& albumName )
{
    if (mpJob)
    {
        mpJob->kill();
        mpJob = 0;
    }

    GalleryForm form(mVersion, mAuthToken);

    form.addPair("cmd",              "fetch-album-images");
    form.addPair("protocol_version", "2.3");
    form.addPair("set_albumName",    albumName);

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType() );
    job->addMetaData("cookies", "manual");
    job->addMetaData("setcookies", m_cookie);
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    mState = GE_LISTPHOTOS;
    mpJob   = job;
    mBuffer.resize(0);
    emit signalBusy( true );
}

void GallerySink::createAlbum( const QString& parentAlbumName,
                                 const QString& albumName,
                                 const QString& albumTitle,
                                 const QString& albumCaption )
{
    if (mpJob)
    {
        mpJob->kill();
        mpJob = 0;
    }

    GalleryForm form(mVersion, mAuthToken);

    form.addPair("cmd", "new-album");
    form.addPair("protocol_version", "2.3");
    form.addPair("set_albumName", parentAlbumName);
    if (!albumName.isEmpty())
        form.addPair("newAlbumName", albumName);
    if (!albumTitle.isEmpty())
        form.addPair("newAlbumTitle", albumTitle);
    if (!albumCaption.isEmpty())
        form.addPair("newAlbumDesc", albumCaption);

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType() );
    job->addMetaData("cookies", "manual");
    job->addMetaData("setcookies", m_cookie);
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    mState = GE_CREATEALBUM;
    mpJob   = job;
    mBuffer.resize(0);
    emit signalBusy( true );
}

bool GallerySink::addPhoto( const QString& albumName,
                              const QString& photoPath,
                              const QString& caption,
                              bool  rescale, int maxDim )
{
    if (mpJob)
    {
        mpJob->kill();
        mpJob = 0;
    }

    QString path = photoPath;
    QString display_filename = QFile::encodeName(KURL(path).filename());

    GalleryForm form(mVersion, mAuthToken);

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
              KExiv2Library::LibKExiv2 exiv2;
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

    KIO::TransferJob* job = KIO::http_post(m_url, form.formData(), false);
    job->addMetaData("content-type", form.contentType());
    job->addMetaData("cookies", "manual");
    job->addMetaData("setcookies", m_cookie);
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KIO::Job *)),
            SLOT(slotResult(KIO::Job *)));

    mState = GE_ADDPHOTO;
    mpJob   = job;
    mBuffer.resize(0);
    emit signalBusy( true );

    return true;
}

void GallerySink::cancel()
{
    if (mpJob)
    {
        mpJob->kill();
        mpJob = 0;
    }
}

void GallerySink::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = mBuffer.size();
    mBuffer.resize(mBuffer.size() + data.size());
    memcpy(mBuffer.data()+oldSize, data.data(), data.size());
}

void GallerySink::slotResult(KIO::Job *job)
{
    mpJob = 0;
    emit signalBusy( false );

    if ( job->error() )
    {
        if ( mState == GE_LOGIN )
            emit signalLoginFailed( job->errorString() );
        else if ( mState == GE_ADDPHOTO )
            emit signalAddPhotoFailed( job->errorString() );
        else
            job->showErrorDialog( ); //m_parent
        return;
    }

    switch(mState)
    {
    case(GE_LOGIN):
        parseResponseLogin(mBuffer);
        break;
    case(GE_LISTALBUMS):
        parseResponseListAlbums(mBuffer);
        break;
    case(GE_LISTPHOTOS):
        parseResponseListPhotos(mBuffer);
        break;
    case(GE_CREATEALBUM):
        parseResponseCreateAlbum(mBuffer);
        break;
    case(GE_ADDPHOTO):
        parseResponseAddPhoto(mBuffer);
        break;
    }

    if (mState == GE_LOGIN && m_loggedIn)
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

void GallerySink::parseResponseLogin(const QByteArray &data)
{
    QTextStream ts(data, IO_ReadOnly );
    ts.setEncoding(QTextStream::UnicodeUTF8);
    QString     line;
    bool foundResponse = false;

    m_loggedIn = false;
    
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
                if (("status" == strlist[0]) && ("0" == strlist[1]))
                {
                  m_loggedIn = true;
                }
                else if ("auth_token" == strlist[0])
                {
                  mAuthToken = strlist[1];
                }
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

void GallerySink::parseResponseListAlbums(const QByteArray &data)
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
                    if (Gallery2 == mVersion)
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

void GallerySink::parseResponseListPhotos(const QByteArray &data)
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

void GallerySink::parseResponseCreateAlbum(const QByteArray &data)
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

void GallerySink::parseResponseAddPhoto(const QByteArray &data)
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
*/
}


//#include "gallerysink.moc"
