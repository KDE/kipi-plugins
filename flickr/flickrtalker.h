/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2009 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2009-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef FLICKRTALKER_H
#define FLICKRTALKER_H

// Qt includes

#include <QUrl>
#include <QList>
#include <QPair>
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QNetworkAccessManager>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "flickritem.h"

class QProgressDialog;

using namespace KIPI;

namespace KIPIFlickrPlugin
{

class GAlbum;
class GPhoto;
class FPhotoInfo;
class FPhotoSet;

class FlickrTalker : public QObject
{
    Q_OBJECT

public:

    enum State
    {
        FE_LOGOUT = -1,
        FE_LOGIN  = 0,
        FE_LISTPHOTOSETS,
        FE_LISTPHOTOS,
        FE_GETPHOTOPROPERTY,
        FE_ADDPHOTO,
        FE_GETFROB,
        FE_CHECKTOKEN,
        FE_GETTOKEN,
        FE_GETAUTHORIZED,
        FE_CREATEPHOTOSET,
        FE_ADDPHOTOTOPHOTOSET,
        FE_GETMAXSIZE
    };

public:

    FlickrTalker(QWidget* const parent, const QString& serviceName);
    ~FlickrTalker();

    QString getUserName() const;
    QString getUserId() const;
    void    maxAllowedFileSize();
    QString getMaxAllowedFileSize();
    void    getFrob();
    void    getToken();
    void    checkToken(const QString& token);
    void    getPhotoProperty(const QString& method, const QStringList& argList);
    void    cancel();

    void    listPhotoSets();
    void    listPhotos(const QString& albumName);
    void    createPhotoSet(const QString& name,
                           const QString& title,
                           const QString& desc,
                           const QString& primaryPhotoId);

    void    addPhotoToPhotoSet(const QString& photoId, const QString& photoSetId);
    bool    addPhoto(const QString& photoPath, const FPhotoInfo& info,
                     bool rescale=false, int maxDim=600, int imageQuality=85);

public:

    QProgressDialog*         m_authProgressDlg;
    QLinkedList <FPhotoSet>* m_photoSetsList;
    FPhotoSet                m_selectedPhotoSet;

Q_SIGNALS:

    void signalError(const QString& msg);
    //  void signalLoginFailed( const QString& msg );
    void signalBusy(bool val);
    void signalAlbums(const QList<GAlbum>& albumList);
    void signalPhotos(const QList<GPhoto>& photoList);
    void signalAddPhotoSucceeded();
    void signalAddPhotoSetSucceeded();
    void signalListPhotoSetsSucceeded();
    void signalListPhotoSetsFailed(QString& msg);
    void signalAddPhotoFailed(const QString& msg);
    void signalListPhotoSetsFailed(const QString& msg);
    void signalAuthenticate();
    void signalTokenObtained(const QString& token);

private:

    //  void parseResponseLogin(const QByteArray& data);
    void parseResponseMaxSize(const QByteArray& data);
    void parseResponseListPhotoSets(const QByteArray& data);
    void parseResponseListPhotos(const QByteArray& data);
    void parseResponseCreateAlbum(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);
    void parseResponseGetFrob(const QByteArray& data);
    void parseResponseGetToken(const QByteArray& data);
    void parseResponseCheckToken(const QByteArray& data);
    void parseResponsePhotoProperty(const QByteArray& data);
    void parseResponseCreatePhotoSet(const QByteArray& data);
    void parseResponseAddPhotoToPhotoSet(const QByteArray& data);

    QString getApiSig(const QString& secret, const QUrl& url);

private Q_SLOTS:

    void slotError(const QString& msg);
    void slotAuthenticate();
    void slotFinished(QNetworkReply* reply);

private:

    QWidget*               m_parent;
//  QString                m_cookie;
    QByteArray             m_buffer;

    QString                m_serviceName;
    QString                m_apiUrl;
    QString                m_authUrl;
    QString                m_uploadUrl;
    QString                m_apikey;
    QString                m_secret;
    QString                m_frob;
    QString                m_maxSize;
    QString                m_token;
    QString                m_username;
    QString                m_userId;
    QString                m_lastTmpFile;

    QNetworkAccessManager* m_netMngr;
    QNetworkReply*         m_reply;

    State                  m_state;

    Interface*             m_iface;
};

} // namespace KIPIFlickrPlugin

#endif /* FLICKRTALKER_H */
