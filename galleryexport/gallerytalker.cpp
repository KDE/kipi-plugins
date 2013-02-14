/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2003-10-01
* Description : a plugin to export to a remote Gallery server.
*
* Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
* Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
* Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
* Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "gallerytalker.moc"

// C++ includes

#include <cstring>
#include <cstdio>

// Qt includes

#include <QByteArray>
#include <QImage>
#include <QRegExp>
#include <QTextStream>
#include <QFileInfo>

// KDE includes

#include <kdebug.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "galleryitem.h"
#include "gallerympform.h"
#include "kpversion.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIGalleryExportPlugin
{

bool GalleryTalker::s_using_gallery2 = true;
QString GalleryTalker::s_authToken   = QString();

class GalleryTalker::Private
{
public:

    Private(QWidget* const parent);

    QWidget*          widget;
    State             state;
    QString           cookie;
    KUrl              galleryUrl;
    KIO::TransferJob* job;
    bool              loggedIn;
    QByteArray        talker_buffer;
};

GalleryTalker::Private::Private(QWidget* const parent)
{
    job      = 0;
    loggedIn = false;
    widget   = new QWidget(parent);
    state    = GE_LOGOUT;
}

GalleryTalker::GalleryTalker(QWidget* const parent)
    : d(new Private(parent))
{
}

GalleryTalker::~GalleryTalker()
{
    if (d->job)
        d->job->kill();

    delete d;
}

bool GalleryTalker::loggedIn() const
{
    return d->loggedIn;
}

void GalleryTalker::login(const KUrl& url, const QString& name, const QString& passwd)
{
    d->job        = 0;
    d->galleryUrl = url;
    d->state      = GE_LOGIN;
    d->talker_buffer.resize(0);

    GalleryMPForm form;
    form.addPair("cmd", "login");
    form.addPair("protocol_version", "2.11");
    form.addPair("uname", name);
    form.addPair("password", passwd);
    form.finish();

    d->job = KIO::http_post(d->galleryUrl, form.formData(), KIO::HideProgressInfo);
    d->job->addMetaData("content-type", form.contentType());
    d->job->addMetaData("cookies", "manual");

    connect(d->job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(d->job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);
}

void GalleryTalker::listAlbums()
{
    d->job   = 0;
    d->state = GE_LISTALBUMS;
    d->talker_buffer.resize(0);

    GalleryMPForm form;

    if (s_using_gallery2)
        form.addPair("cmd", "fetch-albums-prune");
    else
        form.addPair("cmd", "fetch-albums");

    form.addPair("protocol_version", "2.11");
    form.finish();

    d->job = KIO::http_post(d->galleryUrl, form.formData(), KIO::HideProgressInfo);
    d->job->addMetaData("content-type", form.contentType());
    d->job->addMetaData("cookies", "manual");
    d->job->addMetaData("setcookies", d->cookie);

    connect(d->job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(d->job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);
}

void GalleryTalker::listPhotos(const QString& albumName)
{
    d->job   = 0;
    d->state = GE_LISTPHOTOS;
    d->talker_buffer.resize(0);

    GalleryMPForm form;
    form.addPair("cmd", "fetch-album-images");
    form.addPair("protocol_version", "2.11");
    form.addPair("set_albumName", albumName);
    form.finish();

    d->job = KIO::http_post(d->galleryUrl, form.formData(), KIO::HideProgressInfo);
    d->job->addMetaData("content-type", form.contentType());
    d->job->addMetaData("cookies", "manual");
    d->job->addMetaData("setcookies", d->cookie);

    connect(d->job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(d->job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);
}

void GalleryTalker::createAlbum(const QString& parentAlbumName,
                                const QString& albumName,
                                const QString& albumTitle,
                                const QString& albumCaption)
{
    d->job   = 0;
    d->state = GE_CREATEALBUM;
    d->talker_buffer.resize(0);

    GalleryMPForm form;
    form.addPair("cmd", "new-album");
    form.addPair("protocol_version", "2.11");
    form.addPair("set_albumName", parentAlbumName);

    if (!albumName.isEmpty())
        form.addPair("newAlbumName", albumName);

    if (!albumTitle.isEmpty())
        form.addPair("newAlbumTitle", albumTitle);

    if (!albumCaption.isEmpty())
        form.addPair("newAlbumDesc", albumCaption);

    form.finish();

    d->job = KIO::http_post(d->galleryUrl, form.formData(), KIO::HideProgressInfo);
    d->job->addMetaData("content-type", form.contentType());
    d->job->addMetaData("cookies", "manual");
    d->job->addMetaData("setcookies", d->cookie);

    connect(d->job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(d->job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);
}

bool GalleryTalker::addPhoto(const QString& albumName,
                             const QString& photoPath,
                             const QString& title,
                             const QString& description,
                             bool  rescale,
                             int   maxDim)
{
    d->job       = 0;
    QString path = photoPath;
    d->state     = GE_ADDPHOTO;
    d->talker_buffer.resize(0);

    GalleryMPForm form;
    form.addPair("cmd", "add-item");
    form.addPair("protocol_version", "2.11");
    form.addPair("set_albumName", albumName);

    QImage image;

    // Check if RAW file.
    if (KPMetadata::isRawFile(photoPath))
        KDcrawIface::KDcraw::loadRawPreview(image, photoPath);
    else
        image.load(photoPath);

    if (!image.isNull())
    {
        // image file - see if we need to rescale it
        if (rescale && (image.width() > maxDim || image.height() > maxDim))
        {
            image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        path = KStandardDirs::locateLocal("tmp", KUrl(photoPath).fileName());
        image.save(path);
        kDebug() << "Resizing and saving to temp file: " << path ;

        // Restore all metadata.
        KPMetadata meta;

        if (meta.load(photoPath))
        {
            meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            meta.setImageDimensions(image.size());
            meta.save(path);
        }
        else
        {
            kWarning() << "Image " << photoPath << " has no exif data";
        }
    }

    // The filename bit can perhaps be calculated in addFile()
    // but not sure of the temporary filename that could be
    // used for resizing... so I've added it explicitly for now.
    if (!form.addFile(path, title))
        return false;

    if (!title.isEmpty())
        form.addPair("caption", title);

    if (!description.isEmpty())
        form.addPair("extrafield.Description", description);

    form.finish();

    d->job = KIO::http_post(d->galleryUrl, form.formData(), KIO::HideProgressInfo);
    d->job->addMetaData("content-type", form.contentType());
    d->job->addMetaData("cookies", "manual");
    d->job->addMetaData("setcookies", d->cookie);

    connect(d->job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(slotTalkerData(KIO::Job*,QByteArray)));

    connect(d->job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));

    emit signalBusy(true);

    return true;
}

void GalleryTalker::cancel()
{
    if (d->job)
    {
        d->job->kill();
        d->job = 0;
    }
}

void GalleryTalker::slotTalkerData(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = d->talker_buffer.size();
    d->talker_buffer.resize(oldSize + data.size());
    memcpy(d->talker_buffer.data() + oldSize, data.data(), data.size());
}

void GalleryTalker::slotResult(KJob *job)
{
    KIO::Job* const tempjob = static_cast<KIO::Job*>(job);

    if (tempjob->error())
    {
        if (d->state == GE_LOGIN)
        {
            emit signalLoginFailed(tempjob->errorString());
        }
        else
        {
            if (d->state == GE_ADDPHOTO)
            {
                emit signalAddPhotoFailed(tempjob->errorString());
            }
            else
            {
                tempjob->ui()->setWindow(d->widget);
                tempjob->ui()->showErrorMessage();
            }
        }

        emit signalBusy(false);
        return;
    }

    switch (d->state)
    {
        case(GE_LOGIN):
            parseResponseLogin(d->talker_buffer);
            break;
        case(GE_LISTALBUMS):
            parseResponseListAlbums(d->talker_buffer);
            break;
        case(GE_LISTPHOTOS):
            parseResponseListPhotos(d->talker_buffer);
            break;
        case(GE_CREATEALBUM):
            parseResponseCreateAlbum(d->talker_buffer);
            break;
        case(GE_ADDPHOTO):
            parseResponseAddPhoto(d->talker_buffer);
            break;
        default:   // GE_LOGOUT
            break;
    }

    if (d->state == GE_LOGIN && d->loggedIn)
    {
        const QStringList cookielist = (tempjob->queryMetaData("setcookies")).split('\n');
        d->cookie                    = "Cookie:";

        if(!cookielist.isEmpty())
        {
            QRegExp rx("^GALLERYSID=.+");
            QString app;

            foreach(const QString &str, cookielist)
            {
                if(str.contains("Set-Cookie: "))
                {
                    const QStringList cl = str.split(' ');
                    int n                = cl.lastIndexOf(rx);

                    if(n!= -1)
                    {
                        app = cl.at(n);
                    }
                }
            }

            d->cookie += app;
        }

        tempjob->kill();
        listAlbums();
    }

    emit signalBusy(false);
}

void GalleryTalker::parseResponseLogin(const QByteArray &data)
{
    bool foundResponse = false;
    d->loggedIn        = false;
    QString str        = QString::fromUtf8(data);
    QTextStream ts(&str, QIODevice::ReadOnly);
    QString line;

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            foundResponse = line.startsWith(QLatin1String("#__GR2PROTO__"));
        }
        else
        {
            QStringList strlist = line.split('=');

            if (strlist.count() == 2)
            {
                if (("status" == strlist[0]) && ("0" == strlist[1]))
                {
                    d->loggedIn = true;
                }
                else
                {
                    if ("auth_token" == strlist[0])
                    {
                        s_authToken = strlist[1];
                    }
                }
            }
        }
    }

    if (!foundResponse)
    {
        emit signalLoginFailed(i18n("Gallery URL probably incorrect"));
        return;
    }

    if (!d->loggedIn)
    {
        emit signalLoginFailed(i18n("Incorrect username or password specified"));
    }
}

void GalleryTalker::parseResponseListAlbums(const QByteArray& data)
{
    bool foundResponse = false;
    bool success       = false;
    QString str        = QString::fromUtf8(data);
    QTextStream ts(&str, QIODevice::ReadOnly);
    QString line;

    typedef QList<GAlbum> GAlbumList;
    GAlbumList albumList;
    GAlbumList::iterator iter = albumList.begin();

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            foundResponse = line.startsWith(QLatin1String("#__GR2PROTO__"));
        }
        else
        {
            QStringList strlist = line.split('=');

            if (strlist.count() == 2)
            {
                QString key   = strlist[0];
                QString value = strlist[1];

                if (key == "status")
                {
                    success = (value == "0");
                }
                else if (key.startsWith(QLatin1String("album.name")))
                {
                    GAlbum album;
                    album.name = value;

                    if (s_using_gallery2)
                        album.ref_num = value.toInt();
                    else
                        album.ref_num = key.section('.', 2, 2).toInt();

                    iter = albumList.insert(iter, album);
                }
                else if (key.startsWith(QLatin1String("album.title")))
                {
                        (*iter).title = value;
                }
                else if (key.startsWith(QLatin1String((QLatin1String(("album.summary"))))))
                {
                        (*iter).summary = value;
                }
                else if (key.startsWith(QLatin1String(("album.parent"))))
                {
                        (*iter).parent_ref_num = value.toInt();
                }
                else if (key.startsWith(QLatin1String(("album.perms.add"))))
                {
                        (*iter).add = (value == "true");
                }
                else if (key.startsWith(QLatin1String(("album.perms.write"))))
                {
                        (*iter).write = (value == "true");
                }
                else if (key.startsWith(QLatin1String(("album.perms.del_item"))))
                {
                        (*iter).del_item = (value == "true");
                }
                else if (key.startsWith(QLatin1String(("album.perms.del_alb"))))
                {
                        (*iter).del_alb = (value == "true");
                }
                else if (key.startsWith(QLatin1String(("album.perms.create_sub"))))
                {
                        (*iter).create_sub = (value == "true");
                }
                else if (key == "auth_token")
                {
                    s_authToken = value;
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
    qSort(albumList);

    emit signalAlbums(albumList);
}

void GalleryTalker::parseResponseListPhotos(const QByteArray &data)
{
    bool foundResponse = false;
    bool success       = false;
    QString str        = QString::fromUtf8(data);
    QTextStream ts(&str, QIODevice::ReadOnly);
    QString line;

    typedef QList<GPhoto> GPhotoList;
    GPhotoList photoList;
    GPhotoList::iterator iter = photoList.begin();

    /// @todo Unused variable albumURL - Michael Hansen
//     QString albumURL;

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            foundResponse = line.startsWith(QLatin1String(("#__GR2PROTO__")));
        }
        else
        {
            QStringList strlist = line.split('=');

            if (strlist.count() == 2)
            {
                QString key   = strlist[0];
                QString value = strlist[1];

                if (key == "status")
                {
                    success = (value == "0");
                }
                else if (key.startsWith(QLatin1String(("image.name"))))
                {
                        GPhoto photo;
                        photo.name    = value;
                        photo.ref_num = key.section('.', 2, 2).toInt();
                        iter = photoList.insert(iter, photo);
                }
                else if (key.startsWith(QLatin1String(("image.caption"))))
                {
                        (*iter).caption = value;
                }
                else if (key.startsWith(QLatin1String(("image.thumbName"))))
                {
                        (*iter).thumbName = value;
                }
                /// @todo Unused variable albumURL - Michael Hansen
//                 else if (key.startsWith(QLatin1String(("baseurl"))))
//                 {
//                         /// @todo I would recommend to use QString::remove('\\') instead - Michael Hansen
//                         albumURL = value.replace("\\", "");     // doesn't compile fixing EBN Krazy issue!!
//                 }
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

    emit signalPhotos(photoList);
}

void GalleryTalker::parseResponseCreateAlbum(const QByteArray& data)
{
    bool foundResponse = false;
    bool success       = false;
    QString str        = QString::fromUtf8(data);
    QTextStream ts(&str, QIODevice::ReadOnly);
    QString line;

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            foundResponse = line.startsWith(QLatin1String(("#__GR2PROTO__")));
        }
        else
        {
            QStringList strlist = line.split('=');

            if (strlist.count() == 2)
            {
                QString key   = strlist[0];
                QString value = strlist[1];

                if (key == "status")      // key == "status" NOT FOUND!!!
                {
                    success = (value == "0");
                    kWarning() << "Create Album. success: " << success ;
                }
                else if (key.startsWith(QLatin1String(("status_text"))))
                {
                    kDebug() << "STATUS: Create Album: " << value ;
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

void GalleryTalker::parseResponseAddPhoto(const QByteArray& data)
{
    bool foundResponse = false;
    bool success       = false;
    QString str        = QString::fromUtf8(data);
    QTextStream ts(&str, QIODevice::ReadOnly);
    QString line;

    while (!ts.atEnd())
    {
        line = ts.readLine();

        if (!foundResponse)
        {
            // Gallery1 sends resizing debug code sometimes so we
            // have to detect things slightly differently
            foundResponse = (line.startsWith(QLatin1String(("#__GR2PROTO__")))
                             || (line.startsWith(QLatin1String(("<br>- Resizing")))
                                 && line.endsWith(QLatin1String(("#__GR2PROTO__")))));
        }
        else
        {
            QStringList strlist = line.split('=');

            if (strlist.count() == 2)
            {
                QString key   = strlist[0];
                QString value = strlist[1];

                if (key == "status")
                {
                    success = (value == "0");
                    kWarning() << "Add photo. success: " << success ;
                }
                else if (key.startsWith(QLatin1String(("status_text"))))
                {
                    kDebug() << "STATUS: Add Photo: " << value ;
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

} // namespace KIPIGalleryExportPlugin
