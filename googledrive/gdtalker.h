/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#ifndef GD_TALKER_H
#define GD_TALKER_H

//Qt includes

#include <QList>
#include <QString>
#include <QObject>
#include <QStringList>

//KDE includes

#include <kio/job.h>

//local includes

#include "gditem.h"

namespace KIPIGoogleDrivePlugin
{

class GDTalker : public QObject
{
    Q_OBJECT

public:

    GDTalker(QWidget* const parent);
    ~GDTalker();

Q_SIGNALS:

    void signalBusy(bool val);
    void signalAccessTokenFailed(int errCode,const QString& errMsg);
    void signalAccessTokenObtained();
    void signalListAlbumsFailed(const QString& msg);
    void signalListAlbumsDone(const QList<QPair<QString,QString> >& list);
    void signalCreateFolderFailed(const QString& msg);
    void signalCreateFolderSucceeded();
    void signalAddPhotoFailed(const QString& msg);
    void signalAddPhotoSucceeded();
    void signalSetUserName(const QString& msg);
    void signalTextBoxEmpty();
    void signalRefreshTokenObtained(const QString& msg);

private Q_SLOTS:

    void data(KIO::Job* job,const QByteArray& data);
    void slotResult(KJob* job);

public:

    void doOAuth();
    void getAccessToken();
    void getAccessTokenFromRefreshToken(const QString& msg);
    void getUserName();
    void listFolders();
    void createFolder(const QString& title,const QString& id);
    bool addPhoto(const QString& imgPath,const GDPhoto& info,const QString& id,bool rescale,int maxDim,int imageQuality);
    bool authenticated();
    void cancel();

    QString getValue(const QString &,const QString &);
    QStringList getParams(const QString &,const QStringList &,const QString &);
    QString getToken(const QString &,const QString &,const QString &);
    int getTokenEnd(const QString &,int);

private:

    void parseResponseAccessToken(const QByteArray& data);
    void parseResponseListFolders(const QByteArray& data);
    void parseResponseCreateFolder(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);
    void parseResponseUserName(const QByteArray& data);
    void parseResponseRefreshToken(const QByteArray& data);

private:

    enum State
    {
        GD_LISTFOLDERS=0,
        GD_CREATEFOLDER,
        GD_ADDPHOTO,
        GD_ACCESSTOKEN,
        GD_USERNAME,
        GD_REFRESHTOKEN
    };

private:

    QWidget*     m_parent;

    QString      m_scope;
    QString      m_redirect_uri;
    QString      m_response_type;
    QString      m_client_id;
    QString      m_client_secret;
    QString      m_access_token;
    QString      m_refresh_token;
    QString      m_code;

    QString      m_token_uri;

    QString      m_bearer_access_token;
    QByteArray   m_buffer;

    KIO::Job*    m_job;

    State        m_state;
    int          continuePos;

    QString      m_rootid;
    QString      m_rootfoldername;

    QString      m_username;
};

} // namespace KIPIGoogleDrivePlugin

#endif /* GD_TALKER_H */
