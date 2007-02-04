/* ============================================================
 * File  : gallerysink.h
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Based On Work By: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2007-01-27
 *
 * Copyright 2007 Colin Guthrie
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * ============================================================ */

#ifndef GALLERYSINK_H
#define GALLERYSINK_H

#include <qobject.h>
#include <kurl.h>

#include "../../sink.h"
#include "galleryform.h"

namespace KIO
{
    class Job;
}

class KURL;
template <class T> class QValueList;

namespace KIPISyncPlugin
{

class GalleryCollection;
class GalleryItem;

class GallerySink : Sink
{
  Q_OBJECT
  
public:
  enum State {
      GE_LOGIN = 0,
      GE_LISTALBUMS,
      GE_LISTPHOTOS,
      GE_CREATEALBUM,
      GE_ADDPHOTO
  };

  GallerySink(unsigned int sinkId, KConfig* pConfig, KWallet::Wallet* pWallet, GalleryVersion version);
  ~GallerySink();

  const KIPI2::CollectionList* getCollections();
  
  void Save(KConfig* pConfig, KWallet::Wallet* pWallet);

  /*
  bool loggedIn() const;

  void login( const KURL& url, const QString& name,
              const QString& passwd );
  void listAlbums();
  void listPhotos( const QString& albumName );
  void createAlbum( const QString& parentAlbumName,
                    const QString& albumName,
                    const QString& albumTitle,
                    const QString& albumCaption );
  bool addPhoto( const QString& albumName,
                  const QString& photoPath,
                  const QString& caption=QString(),
                  bool rescale=false, int maxDim=600);

  void cancel();
  */
private:
  GalleryVersion mVersion;
  QString        mAuthToken;
  
  QString        mName;
  QString        mURL;
  QString        mUsername;
  QString        mPassword;
  
  State      m_state;
  QString    m_cookie;
  KURL       m_url;
  KIO::Job*  m_job;
  bool       m_loggedIn;
  QByteArray m_buffer;
  /*
  void parseResponseLogin(const QByteArray &data);
  void parseResponseListAlbums(const QByteArray &data);
  void parseResponseListPhotos(const QByteArray &data);
  void parseResponseCreateAlbum(const QByteArray &data);
  void parseResponseAddPhoto(const QByteArray &data);

signals:
  void signalError( const QString& msg );
  void signalLoginFailed( const QString& msg );
  void signalBusy( bool val );
  void signalAlbums( const QValueList<GAlbum>& albumList );
  void signalPhotos( const QValueList<GPhoto>& photoList );
  void signalAddPhotoSucceeded( );
  void signalAddPhotoFailed( const QString& msg );

private slots:
  void data(KIO::Job *job, const QByteArray &data);
  void slotResult (KIO::Job *job);
  */
};

}

#endif /* GALLERYTALKER_H */
