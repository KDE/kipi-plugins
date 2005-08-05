/* ============================================================
 * File  : flickrtalker.h
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2005-07-07
 * Copyright 2005 by Vardhman Jain
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

#ifndef FLICKRTALKER_H
#define FLICKRTALKER_H

#include <qobject.h>
#include <kurl.h>
#include <qprogressdialog.h>
namespace KIO
{
    class Job;
}

class KURL;
template <class T> class QValueList;

namespace KIPIFlickrExportPlugin
{

class GAlbum;
class GPhoto;
class FPhotoInfo;
class FlickrTalker : public QObject
{
    Q_OBJECT

public:

    enum State {
        FE_LOGIN = 0,
        FE_LISTALBUMS,
        FE_LISTPHOTOS,
        FE_GETPHOTOPROPERTY,
        FE_ADDPHOTO,
        FE_GETFROB,
        FE_CHECKTOKEN,
        FE_GETTOKEN,
        FE_GETAUTHORIZED
    };

    FlickrTalker(QWidget* parent);
    ~FlickrTalker();

    QString getApiSig(QString,QStringList) ;
    void getFrob() ;	    
    void getToken() ;	   
    void checkToken(const QString& token) ;	   
    void getPhotoProperty(const QString& method,const QString& argList) ;	   
  
    void listAlbums();
    void listPhotos( const QString& albumName );
    void createAlbum( const QString& parentAlbumName,
                      const QString& albumName,
                      const QString& albumTitle,
                      const QString& albumCaption );
    bool addPhoto( const QString& photoPath,
                   FPhotoInfo& info,
			bool rescale=false, int maxDim=600 );

    void cancel();
    
    QProgressDialog *authProgressDlg;
private:

    QWidget*   m_parent;
    State      m_state;
    //QString    m_cookie;
    //KURL       m_url;
    KIO::Job*  m_job;
    QByteArray m_buffer;
    QString    m_apikey;
    QString    m_secret;
    QString    m_frob;
    QString    m_token;

private:

 //   void parseResponseLogin(const QByteArray &data);
    void parseResponseListAlbums(const QByteArray &data);
    void parseResponseListPhotos(const QByteArray &data);
    void parseResponseCreateAlbum(const QByteArray &data);
    void parseResponseAddPhoto(const QByteArray &data);
    void parseResponseGetFrob(const QByteArray &data);
    void parseResponseGetToken(const QByteArray &data);
    void parseResponseCheckToken(const QByteArray &data);
    void parseResponsePhotoProperty(const QByteArray &data);
   	
signals:

    void signalError( const QString& msg );
    //void signalLoginFailed( const QString& msg );
    void signalBusy( bool val );
    void signalAlbums( const QValueList<GAlbum>& albumList );
    void signalPhotos( const QValueList<GPhoto>& photoList );
    void signalAddPhotoSucceeded( );
    void signalAddPhotoFailed( const QString& msg );
    void signalAuthenticate() ;	    
    void signalTokenObtained(const QString& token);

private slots:

    void slotError( const QString& msg );
    void slotAuthenticate() ;	    
    void data(KIO::Job *job, const QByteArray &data);
    void slotResult (KIO::Job *job);
};

}

#endif /* FLICKRTALKER_H */
