/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#ifndef FLICKRWINDOW_H
#define FLICKRWINDOW_H

// Qt includes.

#include <QHash>
#include <QList>
#include <QPair>
#include <QLabel>

// KDE includes.

#include <kdialog.h>

// LibKIPI includes.

#include <libkipi/interface.h>

class QProgressDialog;
class QPushButton;
class QSpinBox;
class QCheckBox;

class KLineEdit;
class KHTMLPart;
class KUrl;
class KComboBox;

namespace KIPI
{
class Interface;
}

namespace KIPIPlugins
{
class KPAboutData;
class ImagesList;
}

namespace KWallet
{
class Wallet;
}

namespace KIPIFlickrExportPlugin
{

class FlickrWidget;
class FlickrTalker;
class FPhotoInfo;
class FPhotoSet;
class GPhoto;
class GAlbum;
class GAlbumViewItem;
class ImagesList;

class FlickrWindow : public KDialog
{
    Q_OBJECT

public:

    FlickrWindow(KIPI::Interface *interface, const QString &tmpFolder, 
                 QWidget *parent, const QString& serviceName);
    ~FlickrWindow();

private slots:

    void slotTokenObtained(const QString& token);
    void slotDoLogin();
    void slotBusy(bool val);
    void slotError(const QString& msg);

//  void slotLoginFailed( const QString& msg );
//  void slotAlbums( const QValueList<GAlbum>& albumList );
//  void slotPhotos( const QValueList<GPhoto>& photoList );
//  void slotTagSelected();
//  void slotOpenPhoto( const KUrl& url );

    void slotCreateNewPhotoSet();
    void slotUserChangeRequest();
    void slotListPhotoSetsSucceeded();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed(const QString& msg);
    void slotListPhotoSetsFailed(const QString& msg);
    void slotAddPhotoCancel();
    void slotAuthCancel();
    void slotHelp();
    void slotClose();
    void slotUser1();
    void slotImageListChanged(bool);

private:

    void readSettings();
    void writeSettings();

private:

    unsigned int                           m_uploadCount;
    unsigned int                           m_uploadTotal;

    QString                                m_serviceName;

    QPushButton                           *m_newAlbumBtn;
    QPushButton                           *m_changeUserButton;

    KComboBox                             *m_albumsListComboBox;
    QCheckBox                             *m_resizeCheckBox;
    QCheckBox                             *m_publicCheckBox;
    QCheckBox                             *m_familyCheckBox;
    QCheckBox                             *m_friendsCheckBox;
    QCheckBox                             *m_exportHostTagsCheckBox;
    QCheckBox                             *m_stripSpaceTagsCheckBox;

    QSpinBox                              *m_dimensionSpinBox;
    QSpinBox                              *m_imageQualitySpinBox;

    QHash<int, GAlbumViewItem>             m_albumDict;

    QString                                m_token;
    QString                                m_username;
    QString                                m_userId;
    QString                                m_lastSelectedAlbum;
    QString                                m_tmp;

    QLabel                                *m_userNameDisplayLabel;

    QProgressDialog                       *m_progressDlg;
    QProgressDialog                       *m_authProgressDlg;

    QList< QPair<KUrl, FPhotoInfo> >       m_uploadQueue;

//  KWallet::Wallet                       *m_wallet;
    KHTMLPart                             *m_photoView;

    KLineEdit                             *m_tagsLineEdit;

    FlickrWidget                          *m_widget;
    FlickrTalker                          *m_talker;

    KIPI::Interface                       *m_interface;

    KIPIPlugins::KPAboutData              *m_about;
    KIPIPlugins::ImagesList               *m_imglst;
};

} // namespace KIPIFlickrExportPlugin

#endif /* FLICKRWINDOW_H */
