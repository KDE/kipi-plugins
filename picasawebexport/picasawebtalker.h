/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PICASAWEBTALKER_H
#define PICASAWEBTALKER_H

// Qt includes

#include <QLinkedList>
#include <QMap>
#include <QHash>
#include <QObject>

// KDE includes

#include <kurl.h>
#include <kio/jobclasses.h>

namespace KIO
{
    class Job;
}

namespace KIPIPicasawebExportPlugin
{
//class FPhotoInfo;
class PicasaWebAlbum;
class PicasaWebPhoto;

class PicasawebTalker : public QObject
{
    Q_OBJECT

public:

    enum State
    {
        FE_LOGIN = 0,
        FE_LISTALBUMS,
        FE_LISTPHOTOS,
        FE_ADDPHOTO,
        FE_UPDATEPHOTO,
        FE_GETPHOTO,
        FE_CHECKTOKEN,
        FE_GETTOKEN,
        FE_CREATEALBUM
    };

public:

    PicasawebTalker(QWidget* parent);
    ~PicasawebTalker();

    QString token() { return m_token;}
    void getToken(const QString& user, const QString& passwd) ;
    void checkToken(const QString& token) ;
    void authenticate(const QString& token=QString(), const QString& username=QString(), const QString& password=QString()) ;
    void listAlbums(const QString& username);
    void listPhotos(const QString& username, const QString& albumId,
                    const QString& imgmax=QString());
    void createAlbum(const PicasaWebAlbum& newAlbum);
    bool addPhoto(const QString& photoPath, PicasaWebPhoto& info, const QString& albumId);
    bool updatePhoto(const QString& photoPath, PicasaWebPhoto& info);
    void getPhoto(const QString& imgPath);
    QString getUserName();
    void cancel();

Q_SIGNALS:

    void signalError( const QString& msg );
    void signalBusy( bool val );
    void signalLoginProgress(int, int, const QString&);
    void signalLoginDone(int, const QString&);
    void signalListAlbumsDone(int, const QString&, const QList <PicasaWebAlbum>&);
    void signalListPhotosDone(int, const QString&, const QList <PicasaWebPhoto>&);
    void signalCreateAlbumDone(int, const QString&, const QString&);
    void signalAddPhotoDone(int, const QString&, const QString&);
    void signalGetPhotoDone(int errCode, const QString& errMsg,
                            const QByteArray& photoData);

private:

    void parseResponseListAlbums(const QByteArray &data);
    void parseResponseListPhotos(const QByteArray &data);
    void parseResponseCreateAlbum(const QByteArray &data);
    void parseResponseAddPhoto(const QByteArray &data);
    void parseResponseGetToken(const QByteArray &data);
    void parseResponseCheckToken(const QByteArray &data);

private Q_SLOTS:

    void slotError( const QString& msg );
    void data(KIO::Job *job, const QByteArray &data);
    void dataReq(KIO::Job* job, QByteArray &data);
    void info(KIO::Job *job, const QString& str);
    void slotResult (KJob *job);

private:

    QWidget*                    m_parent;
    QByteArray                  m_buffer;
    QString                     m_token;
    QString                     m_username;
    QString                     m_password;

    QMap<KIO::Job*, QByteArray> m_jobData;
    KIO::Job*                   m_job;

    State                       m_state;
};

} // namespace KIPIPicasawebExportPlugin

#endif /* PICASAWEBTALKER_H */
