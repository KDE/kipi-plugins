/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to import/export images to Dropbox web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2,  or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DBTALKER_H
#define DBTALKER_H

// Qt includes

#include <QString>
#include <QObject>
#include <QList>
#include <QPair>
#include <QQueue>

// KDE includes

#include <kio/job.h>

// Local includes

#include "dbitem.h"

namespace KIPIDropboxPlugin
{

class DBTalker : public QObject
{
    Q_OBJECT

public:

    DBTalker(QWidget* const parent);
    ~DBTalker();

public:

    void obtain_req_token();
    void doOAuth();
    bool authenticated();
    void getAccessToken();
    void continueWithAccessToken(const QString& msg1, const QString& msg2, const QString& msg3);
    void getUserName();
    void cancel();
    void listFolders(const QString& path);
    bool addPhoto(const QString& imgPath, const QString& uploadFolder, bool rescale, int maxDim, int imageQuality);
    void createFolder(const QString& path);
    QString generateNonce(qint32 length);

Q_SIGNALS:

    void signalBusy(bool val);
    void signalAccessTokenObtained(const QString& msg1, const QString& msg2, const QString& msg3);
    void signalAccessTokenFailed();
    void signalRequestTokenFailed(int errCode, const QString& errMsg);
    void signalSetUserName(const QString& msg);
    void signalListAlbumsFailed(const QString& msg);
    void signalListAlbumsDone(const QList<QPair<QString, QString> >& list);
    void signalCreateFolderFailed(const QString& msg);
    void signalCreateFolderSucceeded();
    void signalAddPhotoFailed(const QString& msg);
    void signalAddPhotoSucceeded();
    void signalTextBoxEmpty();

private Q_SLOTS:

    void data(KIO::Job* job, const QByteArray& data);
    void slotResult(KJob* job);

private:

    void parseResponseAccessToken(const QByteArray& data);
    void parseResponseRequestToken(const QByteArray& data);
    void parseResponseUserName(const QByteArray& data);
    void parseResponseListFolders(const QByteArray& data);
    void parseResponseCreateFolder(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);

private:

    enum State
    {
        DB_REQ_TOKEN = 0, 
        DB_ACCESSTOKEN, 
        DB_USERNAME, 
        DB_LISTFOLDERS, 
        DB_CREATEFOLDER, 
        DB_ADDPHOTO
    };

private:

    bool                            auth;
    long                            timestamp;
    QString                         nonce;
    QString                         m_oauth_consumer_key;
    QString                         m_oauth_signature;
    QString                         m_oauth_signature_method;
    QString                         m_access_oauth_signature;
    QString                         m_oauth_version;
    QString                         m_oauthToken;
    QString                         m_oauthTokenSecret;
    QString                         m_root;

    QWidget*                        m_parent;

    State                           m_state;

    KIO::Job*                       m_job;
    QByteArray                      m_buffer;
    QQueue<QString>                 queue;
};

} // namespace KIPIDropboxPlugin

#endif /*DBTALKER_H*/
