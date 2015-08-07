/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

//local includes

#include "authorize.h"
#include "gsitem.h"

namespace KIO
{
    class Job;
}

namespace KIPIGoogleServicesPlugin
{

class PicasawebTalker : public Authorize
{
    Q_OBJECT

public:

    enum State
    {
        FE_LOGOUT = -1,
        FE_LISTALBUMS = 0,
        FE_LISTPHOTOS,
        FE_ADDPHOTO,
        FE_UPDATEPHOTO,
        FE_GETPHOTO,
        FE_CREATEALBUM
    };

public:

    PicasawebTalker(QWidget* const parent);
    ~PicasawebTalker();

    void listAlbums();
    void listPhotos(const QString& albumId,
                    const QString& imgmax=QString());
    void createAlbum(const GSFolder& newAlbum);
    bool addPhoto(const QString& photoPath, GSPhoto& info, const QString& albumId,bool rescale,int maxDim,int imageQuality);
    bool updatePhoto(const QString& photoPath, GSPhoto& info,bool rescale,int maxDim,int imageQuality);
    void getPhoto(const QString& imgPath);

    QString getLoginName()   const;
    QString getUserName()    const;
    QString getUserEmailId() const;
    QString token()          const
    {
        return m_access_token;
    }

    void cancel();

Q_SIGNALS:

    void signalError(const QString& msg);
    void signalListAlbumsDone(int, const QString&, const QList <GSFolder>&);
    void signalListPhotosDone(int, const QString&, const QList <GSPhoto>&);
    void signalCreateAlbumDone(int, const QString&, const QString&);
    void signalAddPhotoDone(int, const QString&, const QString&);
    void signalGetPhotoDone(int errCode, const QString& errMsg,
                            const QByteArray& photoData);

private:

    void parseResponseListAlbums(const QByteArray& data);
    void parseResponseListPhotos(const QByteArray& data);
    void parseResponseCreateAlbum(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);

private Q_SLOTS:

    void slotError( const QString& msg );
    void dataReq(KIO::Job* job, QByteArray& data);
    void info(KIO::Job* job, const QString& str);
    void slotResult(KJob* job);

private:

    QString                     m_loginName;
    QString                     m_username;
    QString                     m_password;
    QString                     m_userEmailId;

    QMap<KIO::Job*, QByteArray> m_jobData;
    KIO::Job*                   m_job;

    State                       m_state;
};

} // namespace KIPIGoogleServicesPlugin

#endif /* PICASAWEBTALKER_H */
