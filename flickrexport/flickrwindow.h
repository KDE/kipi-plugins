/* ============================================================
 * File  : flickrwindow.h
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2005-06-17
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

#ifndef FLICKRWINDOW_H
#define FLICKRWINDOW_H

#include <kdialogbase.h>
#include <qvaluelist.h>
#include <qpair.h>
#include <qintdict.h>

#include <libkipi/interface.h>
#include <libkipi/imagedialog.h>





class QListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QProgressDialog;
class KHTMLPart;
class KURL;
class QLineEdit;

namespace KIPI
{
class Interface;
}

namespace KWallet
{
class Wallet;
}

namespace KIPIFlickrExportPlugin
{
class FlickrWidget;
class FlickrTalker;
class GAlbum;
class GPhoto;
class FPhotoInfo;
class GAlbumViewItem;

class FlickrWindow : public KDialogBase
{
    Q_OBJECT

public:

    FlickrWindow(KIPI::Interface *interface, QWidget *parent);
    ~FlickrWindow();

private:

    QListView                *m_tagView;
    KHTMLPart                *m_photoView;
  //  QPushButton              *m_newAlbumBtn;
    QPushButton              *m_addPhotoBtn;
    QCheckBox                *m_resizeCheckBox;
    QCheckBox                *m_publicCheckBox;
    QCheckBox                *m_familyCheckBox;
    QCheckBox                *m_friendsCheckBox;
    QSpinBox                 *m_dimensionSpinBox;
    QLineEdit                *m_tagsLineEdit;
    FlickrWidget	*m_widget;
    FlickrTalker            *m_talker;
    QIntDict<GAlbumViewItem>  m_albumDict;
    QString                   m_token;
    QString                   m_lastSelectedAlbum;
    KIPI::Interface          *m_interface;
    //KWallet::Wallet          *m_wallet;

    QProgressDialog                      *m_progressDlg;
    QProgressDialog                      *m_authProgressDlg;
    unsigned int                          m_uploadCount;
    unsigned int                          m_uploadTotal;
    QValueList< QPair<QString,FPhotoInfo> >  m_uploadQueue;

private slots:
    void slotTokenObtained(const QString& token);
    void slotDoLogin();
    //void slotLoginFailed( const QString& msg );
    void slotBusy( bool val );
    void slotError( const QString& msg );
  //  void slotAlbums( const QValueList<GAlbum>& albumList );
  //  void slotPhotos( const QValueList<GPhoto>& photoList );
  //  void slotTagSelected();
    //void slotOpenPhoto( const KURL& url );
    //void slotNewAlbum();
    void slotAddPhotos();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed( const QString& msg );
    void slotAddPhotoCancel();
    void slotAuthCancel();
};

}

#endif /* FLICKRWINDOW_H */
