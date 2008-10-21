/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PICASAWEBWINDOW_H
#define PICASAWEBWINDOW_H

// Qt includes.

#include <q3valuelist.h>
#include <q3intdict.h>
#include <qpair.h>
#include <QLabel>

// KDE includes.

#include <kdialog.h>

// Libkipi includes.

#include <libkipi/interface.h>

// Local includes

#include "kpaboutdata.h"

class Q3ListView;
class Q3ProgressDialog;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QComboBox;

class KHTMLPart;
class KUrl;

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

class PicasawebWindow : public KDialog
{
    Q_OBJECT

public:

    PicasawebWindow(KIPI::Interface *interface, const QString &tmpFolder,QWidget *parent);
    ~PicasawebWindow();

    void getToken(QString& username, QString& password);

private:

    unsigned int                             m_uploadCount;
    unsigned int                             m_uploadTotal;

    Q3ListView                               *m_tagView;

    QSpinBox                                *m_dimensionSpinBox;
    QSpinBox                                *m_imageQualitySpinBox;

    QPushButton                             *m_newAlbumButton;
    QPushButton                             *m_addPhotoButton;
    QPushButton                             *m_startUploadButton;
    QPushButton                             *m_reloadAlbumsListButton;
    QPushButton                             *m_changeUserButton;

    QString                                  m_tmp;
    QString                                  m_token;
    QString                                  m_username;
    QString                                  m_userId;
    QString                                  m_lastSelectedAlbum;

    QCheckBox                               *m_resizeCheckBox;

    QLineEdit                               *m_tagsLineEdit;

    QRadioButton                            *m_exportApplicationTags;

    Q3ProgressDialog                         *m_progressDlg;
    Q3ProgressDialog                         *m_authProgressDlg;

    QComboBox                               *m_albumsListComboBox;

    QLabel                                  *m_userNameDisplayLabel;

//  KWallet::Wallet                         *m_wallet;

    KHTMLPart                               *m_photoView;
    KUrl::List                              *m_urls;

    PicasawebWidget                         *m_widget;
    PicasawebTalker                         *m_talker;

    Q3IntDict<GAlbumViewItem>                m_albumDict;

    Q3ValueList< QPair<QString,FPhotoInfo> > m_uploadQueue;

    KIPI::Interface                         *m_interface;
    KIPIPlugins::KPAboutData                *m_about;

private slots:

    void slotTokenObtained(const QString& token);
    void slotDoLogin();
//  void slotLoginFailed( const QString& msg );
    void slotBusy( bool val );
    void slotError( const QString& msg );
//  void slotAlbums( const QValueList<GAlbum>& albumList );
//  void slotPhotos( const QValueList<GPhoto>& photoList );
//  void slotTagSelected();
//  void slotOpenPhoto( const KUrl& url );
    void slotUpdateAlbumsList();
    void slotUserChangeRequest();
    void slotListPhotoSetsResponse(const Q3ValueList <FPhotoSet>& photoSetList);
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
//  void slotHandleLogin();

protected:

    void slotClose();
};

} // namespace KIPIPicasawebExportPlugin

#endif /* PICASAWEBWINDOW_H */
