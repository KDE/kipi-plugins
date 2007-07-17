/* ============================================================
 * File  : picasawebwindow.h
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2007-16-07
 * Copyright 2007 by Vardhman Jain <vardhman @ gmail.com>
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

#ifndef PICASAWEBWINDOW_H
#define PICASAWEBWINDOW_H

#include <kdialogbase.h>
#include <qvaluelist.h>
#include <qpair.h>
#include <qintdict.h>

#include <libkipi/interface.h>
#include <libkipi/imagedialog.h>

// Local includes

#include "kpaboutdata.h"


class QListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QProgressDialog;
class KHTMLPart;
class KURL;
class QLineEdit;
class QComboBox;

namespace KIPI
{
class Interface;
}

namespace KWallet
{
class Wallet;
}

namespace KIPIPicasawebExportPlugin
{
class PicasawebWidget;
class PicasawebTalker;
class GAlbum;
class GPhoto;
class FPhotoInfo;
class FPhotoSet;
class GAlbumViewItem;

class PicasawebWindow : public KDialogBase
{
    Q_OBJECT

public:

    PicasawebWindow(KIPI::Interface *interface, const QString &tmpFolder,QWidget *parent);
    void getToken(QString& username, QString& password);
    ~PicasawebWindow();

private:

    QListView                *m_tagView;
    KHTMLPart                *m_photoView;
  
    QPushButton              *m_newAlbumButton;
    QSpinBox                 *m_dimensionSpinBox;
    QSpinBox                 *m_imageQualitySpinBox;
    QPushButton              *m_addPhotoButton;
    QPushButton              *m_helpButton;
    
    QCheckBox                *m_resizeCheckBox;
    QLineEdit                *m_tagsLineEdit;
	QCheckBox			     *m_exportApplicationTags;
	QPushButton              *m_startUploadButton;
    QPushButton              *m_reloadAlbumsListButton;
    PicasawebWidget	         *m_widget;
    PicasawebTalker          *m_talker;
    QIntDict<GAlbumViewItem>  m_albumDict;
    QComboBox                  *m_albumsListComboBox;
    QString                   m_token;
	QString					  m_username;
	QString 				  m_userId;
    QString                   m_lastSelectedAlbum;
    KIPI::Interface          *m_interface;
	QString 				  m_tmp;
	QLabel 					  *m_userNameDisplayLabel;
    //KWallet::Wallet          *m_wallet;
	QPushButton				 *m_changeUserButton;
    KURL::List               *m_urls;
    QProgressDialog                      *m_progressDlg;
    QProgressDialog                      *m_authProgressDlg;
    unsigned int                          m_uploadCount;
    unsigned int                          m_uploadTotal;
    QValueList< QPair<QString,FPhotoInfo> >  m_uploadQueue;

    KIPIPlugins::KPAboutData    *m_about; 

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
    void slotUpdateAlbumsList();
    void slotUserChangeRequest();
    void slotListPhotoSetsResponse(const QValueList <FPhotoSet>& photoSetList);
    void slotAddPhotos();
	void slotUploadImages();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed( const QString& msg );
    void slotAddPhotoCancel();
    void slotAuthCancel();
    void slotHelp();
    void slotCreateNewAlbum();
    void slotGetAlbumsListSucceeded();
    void slotGetAlbumsListFailed(const QString& msg);
    void slotRefreshSizeButtons(bool);
    //void slotHandleLogin();
protected:
    void slotClose();
};

}

#endif /* PICASAWEBWINDOW_H */
