/* ============================================================
 * File  : gallerywindow.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Copyright 2004 by Renchi Raju
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

#ifndef GALLERYWINDOW_H
#define GALLERYWINDOW_H

#include <kdialogbase.h>
#include <qintdict.h>

class QListView;
class QPushButton;
class KHTMLPart;
class KURL;
class GalleryTalker;
class GAlbum;
class GPhoto;
class GAlbumViewItem;
template <class T> class QValueList;

class GalleryWindow : public KDialogBase
{
    Q_OBJECT

public:

    GalleryWindow();
    ~GalleryWindow();

private:

    QListView                *m_albumView;
    KHTMLPart                *m_photoView;
    QPushButton              *m_newAlbumBtn;
    QPushButton              *m_addPhotoBtn;
    GalleryTalker            *m_talker;
    QIntDict<GAlbumViewItem>  m_albumDict;
    QString                   m_url;
    QString                   m_user;
    QString                   m_lastSelectedAlbum;

private slots:

    void slotDoLogin();
    void slotLoginFailed( const QString& msg );
    void slotBusy( bool val );
    void slotError( const QString& msg );
    void slotAlbums( const QValueList<GAlbum>& albumList );
    void slotPhotos( const QValueList<GPhoto>& photoList );
    void slotAlbumSelected();
    void slotOpenPhoto( const KURL& url );
    void slotNewAlbum();
    void slotAddPhotos();
};

#endif /* GALLERYWINDOW_H */
