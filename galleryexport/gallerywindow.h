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
#include <qvaluelist.h>
#include <qpair.h>
#include <qintdict.h>

class QListView;
class QPushButton;
class QProgressDialog;
class KHTMLPart;
class KURL;

namespace KIPI
{
class Interface;
}

namespace KWallet
{
class Wallet;
}

namespace KIPIGalleryExportPlugin
{

class GalleryTalker;
class GAlbum;
class GPhoto;
class GAlbumViewItem;

class GalleryWindow : public KDialogBase
{
    Q_OBJECT

public:

    GalleryWindow(KIPI::Interface *interface);
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
    KIPI::Interface          *m_interface;
    KWallet::Wallet          *m_wallet;

    QProgressDialog                      *m_progressDlg;
    unsigned int                          m_uploadCount;
    unsigned int                          m_uploadTotal;
    QValueList< QPair<QString,QString> >  m_uploadQueue;

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
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed( const QString& msg );
    void slotAddPhotoCancel();
};

}

#endif /* GALLERYWINDOW_H */
