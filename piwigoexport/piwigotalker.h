/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.kipi-plugins.org
*
* Date        : 2010-02-15
* Description : a plugin to export to a remote Piwigo server.
*
* Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
* Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
* Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
* Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2010 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#ifndef PIWIGOTALKER_H
#define PIWIGOTALKER_H

// Qt includes

#include <QObject>
#include <QList>
#include <QDateTime>
// Debug
#include <QTextStream>
#include <QFile>

// KDE includes

#include <KUrl>
#include <kio/job.h>

namespace KIO
{
class Job;
}

class KUrl;
template <class T> class QList;

namespace KIPIPiwigoExportPlugin
{

class GAlbum;
class GPhoto;

class PiwigoTalker : public QObject
{
    Q_OBJECT

public:

    enum State {
        GE_LOGIN = 0,
        GE_LISTALBUMS,
        GE_CHECKPHOTOEXIST,
        GE_ADDPHOTO,
        GE_ADDTHUMB,
        GE_ADDHQ,
        GE_ADDPHOTOSUMMARY
    };

    enum {
        CHUNK_MAX_SIZE = 500000
    };
    
public:

    PiwigoTalker(QWidget* parent);
    ~PiwigoTalker();

    static QString getAuthToken() {
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

    bool addPhoto(int albumId,
                  const QString& photoPath,
                  const QString& caption = QString(),
                  bool  captionIsTitle = true, bool captionIsDescription = false,
                  bool rescale = false, bool downloadHQ = false, int maxDim = 600, int thumbDim = 128);

    void cancel();

Q_SIGNALS:

    void signalProgressInfo(const QString& msg);
    void signalError(const QString& msg);
    void signalLoginFailed(const QString& msg);
    void signalBusy(bool val);
    void signalAlbums(const QList<GAlbum>& albumList);
    void signalAddPhotoSucceeded();
    void signalAddPhotoFailed(const QString& msg);

private:

    void parseResponseLogin(const QByteArray& data);
    void parseResponseListAlbums(const QByteArray& data);
    void parseResponseDoesPhotoExist(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);
    void parseResponseAddThumbnail(const QByteArray& data);
    void addHQNextChunk();
    void parseResponseAddHQPhoto(const QByteArray& data);
    void addPhotoSummary();
    void parseResponseAddPhotoSummary(const QByteArray& data);

    QByteArray computeMD5Sum(const QString &filepath);

private Q_SLOTS:

    void slotTalkerData(KIO::Job *job, const QByteArray& data);
    void slotResult(KJob *job);

private:

    QWidget*          m_parent;
    State             m_state;
    QString           m_cookie;
    KUrl              m_url;
    KIO::TransferJob* m_job;
    bool              m_loggedIn;
    QByteArray        m_talker_buffer;
    uint              m_chunkId;

    QByteArray        m_md5sum;
    QString           m_path;
    int               m_albumId;
    QString           m_thumbpath;
    QString           m_hqpath;
    QString           m_comment;
    QString           m_name;
    QDateTime         m_date;

    static QString    s_authToken;
};

} // namespace KIPIPiwigoExportPlugin

#endif /* PIWIGOTALKER_H */
