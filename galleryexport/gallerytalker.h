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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* ============================================================ */

#ifndef GALLERYTALKER_H
#define GALLERYTALKER_H

// Qt includes

#include <QObject>
#include <QList>

// KDE includes

#include <kurl.h>
#include <kio/job.h>

namespace KIO
{
    class Job;
}

class KUrl;
template <class T> class QList;

namespace KIPIGalleryExportPlugin
{

class GAlbum;
class GPhoto;

class GalleryTalker : public QObject
{
    Q_OBJECT

public:

    enum State
    {
        GE_LOGOUT = -1,
        GE_LOGIN  = 0,
        GE_LISTALBUMS,
        GE_LISTPHOTOS,
        GE_CREATEALBUM,
        GE_ADDPHOTO
    };

public:

    explicit GalleryTalker(QWidget* const parent);
    ~GalleryTalker();

    static void setGallery2(bool usegallery2)
    {
        s_using_gallery2 = usegallery2;
    };

    static bool isGallery2()
    {
        return s_using_gallery2;
    };

    static QString getAuthToken()
    {
        return s_authToken;
    };

    bool loggedIn() const;

    void login(const KUrl& url, const QString& name, const QString& passwd);
    void listAlbums();
    void listPhotos(const QString& albumName);

    void createAlbum(const QString& parentAlbumName,
                     const QString& albumName,
                     const QString& albumTitle,
                     const QString& albumCaption);

    bool addPhoto(const QString& albumName,
                  const QString& photoPath,
                  const QString& title,
                  const QString& description,
                  bool rescale = false, int maxDim = 600);

    void cancel();

Q_SIGNALS:

    void signalError(const QString& msg);
    void signalLoginFailed(const QString& msg);
    void signalBusy(bool val);
    void signalAlbums(const QList<GAlbum>& albumList);
    void signalPhotos(const QList<GPhoto>& photoList);
    void signalAddPhotoSucceeded();
    void signalAddPhotoFailed(const QString& msg);

private:

    void parseResponseLogin(const QByteArray& data);
    void parseResponseListAlbums(const QByteArray& data);
    void parseResponseListPhotos(const QByteArray& data);
    void parseResponseCreateAlbum(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);

private Q_SLOTS:

    void slotTalkerData(KIO::Job *job, const QByteArray& data);
    void slotResult(KJob *job);

private:

    class Private;
    Private* const d;

    static bool    s_using_gallery2;
    static QString s_authToken;
};

} // namespace KIPIGalleryExportPlugin

#endif /* GALLERYTALKER_H */
