/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QProgressDialog>
#include <QLinkedList>
#include <QMap>
#include <QObject>

// KDE includes

#include <kurl.h>
#include <kio/jobclasses.h>

namespace KIO
{
    class Job;
}

class KUrl;

namespace KIPIPicasawebExportPlugin
{

class GAlbum;
// class GPhoto;
class FPhotoInfo;
class PicasaWebAlbum;

class PicasawebTalker : public QObject
{
    Q_OBJECT

public:

    enum State
    {
        FE_LOGIN = 0,
        FE_LISTALBUMS,
        FE_ADDTAG,
        FE_LISTPHOTOS,
        FE_GETPHOTOPROPERTY,
        FE_ADDPHOTO,
        FE_GETFROB,
        FE_CHECKTOKEN,
        FE_GETTOKEN,
        FE_CREATEALBUM,
        FE_GETAUTHORIZED
    };

public:

    PicasawebTalker(QWidget* parent);
    ~PicasawebTalker();

    QLinkedList <PicasaWebAlbum> * m_albumsList;
    QString token() { return m_token;}
    QString getApiSig(QString,QStringList) ;
    void addPhotoTag(const QString& photoURI, const QString& tag);
    void getToken(const QString& user, const QString& passwd) ;
    void checkToken(const QString& token) ;
    void authenticate(const QString& token=NULL, const QString& username=NULL, const QString& password=NULL) ;
    void getPhotoProperty(const QString& method, const QString& argList) ;
    void getHTMLResponseCode(const QString& str);
    void listAllAlbums();
    void listPhotos( const QString& albumName );
    void createAlbum( const QString& albumTitle, const QString& albumDesc, const QString& location,
                      long long timestamp, const QString& access, const QString& media_keywords, bool isCommentEnabled=true);
    bool addPhoto( const QString& photoPath,
                   FPhotoInfo& info, const QString& albumname,
                   bool rescale=false, int maxDim=600 , int imageQuality=85 );
	           QString getUserName();
	           QString getUserId();
    void cancel();

public:

    QProgressDialog *authProgressDlg;

Q_SIGNALS:

    void signalError( const QString& msg );
//  void signalLoginFailed( const QString& msg );
    void signalBusy( bool val );
    void signalAlbums( const QLinkedList<GAlbum>& albumList );
    // void signalPhotos( const QLinkedList<GPhoto>& photoList );
    void signalAddPhotoSucceeded( );
    void signalGetAlbumsListSucceeded();
    void signalGetAlbumsListFailed( const QString& msg );
    void signalAddPhotoFailed( const QString& msg );
    void signalAuthenticate() ;
    void signalTokenObtained(const QString& token);
    void signalCheckTokenSuccessful();

private:

//  void parseResponseLogin(const QByteArray &data);
    void parseResponseListAlbums(const QByteArray &data);
    void parseResponseListPhotos(const QByteArray &data);
    void parseResponseCreateAlbum(const QByteArray &data);
    void parseResponseAddPhoto(const QByteArray &data);
    void parseResponseAddTag(const QByteArray &data);
    void parseResponseGetToken(const QByteArray &data);
    void parseResponseCheckToken(const QByteArray &data);
    void parseResponsePhotoProperty(const QByteArray &data);

private Q_SLOTS:

    void slotCheckTokenSuccessful();
    void slotError( const QString& msg );
//  void slotAuthenticate() ;
    void data(KIO::Job *job, const QByteArray &data);
    void info(KIO::Job *job, const QString& str);
    void slotResult (KJob *job);

private:

    int        remaining_tags_count;

    QWidget*   m_parent;

    QByteArray m_buffer;

//  QString    m_cookie;
    QString    m_apikey;
    QString    m_secret;
    QString    m_frob;
    QString    m_token;
    QString    m_username;
    QString    m_password;
    QString    m_userId;

    QMap<QString, QStringList > tags_map;

//  KUrl       m_url;
    KIO::Job*  m_job;

    State      m_state;
};

} // namespace KIPIPicasawebExportPlugin

#endif /* PICASAWEBTALKER_H */
