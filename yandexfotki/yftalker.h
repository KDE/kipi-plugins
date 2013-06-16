/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-14
 * Description : Yandex.Fotki web service backend
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
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

#ifndef YF_TALKER_H
#define YF_TALKER_H

// Qt includes

#include <QObject>
#include <QPointer>

// KDE includes

#include <kio/jobclasses.h>

// Local includes

#include "yfphoto.h"
#include "yfalbum.h"

class QDomElement;

namespace KIO
{
class Job;
}

namespace KIPIYandexFotkiPlugin
{

class YandexFotkiTalker : public QObject
{
    Q_OBJECT

public:

    YandexFotkiTalker(QObject* const parent = 0);
    ~YandexFotkiTalker();

    /*
     * We use FSM for async I/O
     */

    enum State
    {
        // FSM states
        STATE_UNAUTHENTICATED        = 0x0,
        STATE_AUTHENTICATED          = 0x80,
        STATE_ERROR                  = 0x40,

        STATE_GETSERVICE             = STATE_UNAUTHENTICATED | 0x1,
        STATE_GETSERVICE_ERROR       = STATE_UNAUTHENTICATED | STATE_ERROR | 0x2,
        STATE_GETSERVICE_DONE        = STATE_UNAUTHENTICATED | 0x3,
        /*
        // for future use
        STATE_CHECKTOKEN             = STATE_UNAUTHENTICATED | 0x4,
        STATE_CHECKTOKEN_INVALID     = STATE_UNAUTHENTICATED | 0x5,
        STATE_CHECKTOKEN_DONE        = STATE_UNAUTHENTICATED | 0x6,
        */
        STATE_GETSESSION             = STATE_UNAUTHENTICATED | 0x7,
        STATE_GETSESSION_ERROR       = STATE_UNAUTHENTICATED | STATE_ERROR | 0x8,
        STATE_GETSESSION_DONE        = STATE_UNAUTHENTICATED | 0x9,

        STATE_GETTOKEN               = STATE_UNAUTHENTICATED | 0xa,
        STATE_INVALID_CREDENTIALS    = STATE_UNAUTHENTICATED | STATE_ERROR | 0xb,
        STATE_GETTOKEN_ERROR         = STATE_UNAUTHENTICATED | STATE_ERROR | 0xc,
        STATE_GETTOKEN_DONE          = STATE_AUTHENTICATED, // simple alias

        STATE_LISTALBUMS             = STATE_AUTHENTICATED | 0x1,
        STATE_LISTALBUMS_ERROR       = STATE_AUTHENTICATED | STATE_ERROR | 0x2,
        STATE_LISTALBUMS_DONE        = STATE_AUTHENTICATED,

        STATE_LISTPHOTOS             = STATE_AUTHENTICATED | 0x4,
        STATE_LISTPHOTOS_ERROR       = STATE_AUTHENTICATED | STATE_ERROR | 0x5,
        STATE_LISTPHOTOS_DONE        = STATE_AUTHENTICATED,

        STATE_UPDATEPHOTO_FILE       = STATE_AUTHENTICATED | 0x7,
        STATE_UPDATEPHOTO_FILE_ERROR = STATE_AUTHENTICATED | STATE_ERROR | 0x8,
        STATE_UPDATEPHOTO_INFO       = STATE_AUTHENTICATED | 0x9,
        STATE_UPDATEPHOTO_INFO_ERROR = STATE_AUTHENTICATED | STATE_ERROR | 0xa,
        STATE_UPDATEPHOTO_DONE       = STATE_AUTHENTICATED,

        STATE_UPDATEALBUM            = STATE_AUTHENTICATED | 0xb,
        STATE_UPDATEALBUM_ERROR      = STATE_AUTHENTICATED | STATE_ERROR | 0xc,
        STATE_UPDATEALBUM_DONE       = STATE_AUTHENTICATED
    };

    /*
     * Fields
     */
    State state() const
    {
        return m_state;
    }

    const QString& sessionKey() const
    {
        return m_sessionKey;
    }

    const QString& sessionId() const
    {
        return m_sessionId;
    }

    const QString& token() const
    {
        return m_token;
    }

    const QString& login() const
    {
        return m_login;
    }

    void setLogin(const QString& login)
    {
        m_login = login;
    }

    const QString& password() const
    {
        return m_password;
    }

    void setPassword(const QString& password)
    {
        m_password = password;
    }

    bool isAuthenticated() const
    {
        return (m_state & STATE_AUTHENTICATED) != 0;
    }

    bool isErrorState() const
    {
        return (m_state & STATE_ERROR) != 0;
    }

    const QList<YandexFotkiAlbum>& albums() const
    {
        return m_albums;
    }

    const QList<YandexFotkiPhoto>& photos() const
    {
        return m_photos;
    }

    /*
     * Actions
     */

    void getService();
    //void checkToken();
    void getSession();
    void getToken();

    void listAlbums();
    void listPhotos(const YandexFotkiAlbum& album);
    void updatePhoto(YandexFotkiPhoto& photo, const YandexFotkiAlbum& album);
    void updateAlbum(YandexFotkiAlbum& album);

    void reset();
    void cancel();

    /*
     * API-related public constants
     */
    static const QString USERPAGE_URL;
    static const QString USERPAGE_DEFAULT_URL;

Q_SIGNALS:

    // emits than  than we are in a bad state
    void signalError();
    void signalGetSessionDone();
    void signalGetTokenDone();
    void signalGetServiceDone();

    void signalListAlbumsDone(const QList<YandexFotkiAlbum>&);
    void signalListPhotosDone(const QList<YandexFotkiPhoto>&);
    void signalUpdatePhotoDone(YandexFotkiPhoto& );
    void signalUpdateAlbumDone();

protected Q_SLOTS:

    void handleJobData(KIO::Job* job, const QByteArray& data);
    // special method for ugly KIO::put API
    void handleJobReq(KIO::Job* job, QByteArray& data);

    void parseResponseGetSession(KJob* job);
    //void parseResponseCheckToken(KJob *job);
    void parseResponseGetToken(KJob* job);
    void parseResponseGetService(KJob* job);
    void parseResponseListAlbums(KJob* job);

    void parseResponseListPhotos(KJob* job);
    bool parsePhotoXml(const QDomElement& entryElem,
                       YandexFotkiPhoto& photo);

    void parseResponseUpdatePhotoFile(KJob* job);
    void parseResponseUpdatePhotoInfo(KJob* job);
    void parseResponseUpdateAlbum(KJob* job);

protected:

    /*
     * API-related
     */

    // fields
    QString              m_sessionKey;
    QString              m_sessionId;
    QString              m_token;
    QString              m_login;
    QString              m_password;
    QString              m_apiAlbumsUrl;
    QString              m_apiPhotosUrl;
    QString              m_apiTagsUrl;

    // constants
    static const QString SESSION_URL; // use QString insted of KUrl, we need .arg
    static const QString TOKEN_URL;
    static const QString SERVICE_URL;
    static const QString AUTH_REALM;
    static const QString ACCESS_STRINGS[];

protected:

    /*
     * Utils
     */
    // for parseXXXX slots
    bool prepareJobResult(KJob* job, State error);

    // for updatePhoto
    void updatePhotoFile(YandexFotkiPhoto& photo);
    void updatePhotoInfo(YandexFotkiPhoto& photo);

    // for updateAlbum
    void updateAlbumCreate(YandexFotkiAlbum& album);

    // return result from any place
    void setErrorState(State state);

    void listAlbumsNext(); // see listPhotos();

    // for photos pagination in listPhotos()
    void listPhotosNext(); // see listPhotos();

protected:

    /*
     * FSM data
     */
    State                   m_state;
    // temporary data
    YandexFotkiPhoto*       m_lastPhoto;
    QString                 m_lastPhotosUrl;

    // for albums pagination in listAlbums()
    QList<YandexFotkiAlbum> m_albums;

    QString                 m_albumsNextUrl;

    QList<YandexFotkiPhoto> m_photos;
    QString                 m_photosNextUrl;

    // KIO job
    QPointer<KIO::Job>      m_job;
    // KIO buffer
    QByteArray              m_buffer;
};

} // namespace KIPIYandexFotkiPlugin

#endif /* YF_TALKER_H */
