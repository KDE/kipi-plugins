/* ============================================================
 * File  : gallerytalker.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
 *
 *
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
 * ============================================================ */

#ifndef GALLERYTALKER_H
#define GALLERYTALKER_H

// KDE includes
#include <KUrl>

// Qt includes
#include <QObject>
#include <QList>


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

    enum State {
        GE_LOGIN = 0,
        GE_LISTALBUMS,
        GE_LISTPHOTOS,
        GE_CREATEALBUM,
        GE_ADDPHOTO
    };

    GalleryTalker(QWidget* parent);
    ~GalleryTalker();

    static void setGallery2(bool usegallery2) {
        s_using_gallery2 = usegallery2;
    };
    static bool isGallery2() {
        return s_using_gallery2;
    };

    static QString getAuthToken() {
        return s_authToken;
    };

    bool loggedIn() const;

    void login(const KUrl& url, const QString& name,
               const QString& passwd);
    void listAlbums();
    void listPhotos(const QString& albumName);
    void createAlbum(const QString& parentAlbumName,
                     const QString& albumName,
                     const QString& albumTitle,
                     const QString& albumCaption);
    bool addPhoto(const QString& albumName,
                  const QString& photoPath,
                  const QString& caption = QString(),
                  bool  captionIsTitle = true, bool captionIsDescription = false,
                  bool rescale = false, int maxDim = 600);

    void cancel();

private:

    QWidget*   m_parent;
    State      m_state;
    QString    m_cookie;
    KUrl       m_url;
    KIO::Job*  m_job;
    bool       m_loggedIn;
    QByteArray m_buffer;

    static bool    s_using_gallery2;
    static QString s_authToken;

private:

    void parseResponseLogin(const QByteArray &data);
    void parseResponseListAlbums(const QByteArray &data);
    void parseResponseListPhotos(const QByteArray &data);
    void parseResponseCreateAlbum(const QByteArray &data);
    void parseResponseAddPhoto(const QByteArray &data);

signals:

    void signalError(const QString& msg);
    void signalLoginFailed(const QString& msg);
    void signalBusy(bool val);
    void signalAlbums(const QList<GAlbum>& albumList);
    void signalPhotos(const QList<GPhoto>& photoList);
    void signalAddPhotoSucceeded();
    void signalAddPhotoFailed(const QString& msg);

private slots:

    void data(KIO::Job *job, const QByteArray &data);
    void slotResult(KIO::Job *job);
};

}

#endif /* GALLERYTALKER_H */
