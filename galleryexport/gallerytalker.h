/* ============================================================
 * File  : gallerytalker.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Copyright 2004 by Renchi Raju
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

#include <qobject.h>
#include <kurl.h>

namespace KIO
{
    class Job;
}

class KURL;
class GAlbum;
class GPhoto;
template <class T> class QValueList;

class GalleryTalker : public QObject
{
    Q_OBJECT

public:

    enum State {
        GE_LOGIN = 0,
        GE_LISTALBUMS,
        GE_LISTPHOTOS,
        GE_GETTHUMB
    };

    GalleryTalker(QWidget* parent);
    ~GalleryTalker();

    bool loggedIn() const;

    void login( const KURL& url, const QString& name,
                const QString& passwd );
    void listPhotos( const QString& albumName );

private:

    QWidget*   m_parent;
    State      m_state;
    QString    m_cookie;
    KURL       m_url;
    KIO::Job*  m_job;
    bool       m_loggedIn;
    QByteArray m_buffer;

private:

    void parseResponseLogin(const QByteArray &data);
    void parseResponseListAlbums(const QByteArray &data);
    void parseResponseListPhotos(const QByteArray &data);

signals:

    void signalError( const QString& msg );
    void signalLoginFailed( const QString& msg );
    void signalBusy( bool val );
    void signalAlbums( const QValueList<GAlbum>& albumList );
    void signalPhotos( const QValueList<GPhoto>& photoList );

private slots:

    void data(KIO::Job *job, const QByteArray &data);
    void slotResult (KIO::Job *job);
};



#endif /* GALLERYTALKER_H */
