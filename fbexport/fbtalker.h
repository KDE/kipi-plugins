/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to export images to Facebook web service
 *
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

#ifndef FBTALKER_H
#define FBTALKER_H

// Qt includes.
#include <QList>
#include <QString>
#include <QTime>
#include <QObject>

// KDE includes.
#include <kio/job.h>

class QProgressDialog;
class QDomElement;

namespace KIPIFbExportPlugin
{

class FbAlbum;

class FbTalker : public QObject
{
    Q_OBJECT

public:
    FbTalker(QWidget* parent);
    ~FbTalker();

    QString         getSessionKey() const;
    unsigned int    getSessionExpires() const;

    QString getDisplayName() const;
    QString getProfileURL() const;

    bool    loggedIn();
    void    cancel();
    void    authenticate(const QString& sessionKey, unsigned int sessionExpires);
    void    logout(); //rename

    void    listAlbums();

    void    createAlbum(const FbAlbum& album);

    bool    addPhoto(const QString& imgPath, long long albumID);

public:
    QProgressDialog *m_authProgressDlg; // TODO: move to method?

signals:
    void signalBusy(bool val);
    void signalLoginDone(int errCode, const QString& errMsg);
    void signalAddPhotoDone(int errCode, const QString& errMsg);
    void signalCreateAlbumDone(int errCode, const QString& errMsg,
                               long long newAlbumID);
    void signalListAlbumsDone(int errCode, const QString& errMsg,
                              const QList <FbAlbum>& albumsList);

private:
    enum State
    {
        FB_CREATETOKEN = 0,
        FB_GETSESSION,
        FB_GETLOGGEDINUSER,
        FB_GETUSERINFO,
        FB_LOGOUT,
        FB_LISTALBUMS,
        FB_CREATEALBUM,
        FB_ADDPHOTO
    };

    QString getApiSig(const QMap<QString, QString>& args);
    QString getCallString(const QMap<QString, QString>& args);
    void    createToken();
    void    getSession();
    void    getLoggedInUser();
    void    getUserInfo();

    QString errorToText(int errCode, const QString& errMsg);
    int parseErrorResponse(const QDomElement& e, QString& errMsg);
    void parseResponseCreateToken(const QByteArray& data);
    void parseResponseGetSession(const QByteArray& data);
    void parseResponseGetLoggedInUser(const QByteArray& data);
    void parseResponseGetUserInfo(const QByteArray& data);
    void parseResponseLogout(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);
    void parseResponseCreateAlbum(const QByteArray& data);
    void parseResponseListAlbums(const QByteArray& data);

private slots:
    void data(KIO::Job *job, const QByteArray& data);
    void slotResult(KJob *job);

private:
    QWidget*   m_parent;

    QByteArray m_buffer;

    QString    m_userAgent;
    QString    m_apiURL;
    QString    m_apiVersion;
    QString    m_apiKey;
    QString    m_secretKey;
    QString    m_authToken;
    QString    m_sessionKey;
    long       m_sessionExpires;
    long       m_uid;
    QTime      m_callID;

    QString    m_userName;
    QString    m_userURL;

    KIO::Job*  m_job;

    State      m_state;
};

} // namespace KIPIFbExportPlugin

#endif /* FBTALKER_H */
