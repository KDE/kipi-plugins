/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QHash>
#include <QList>
#include <QPair>
#include <QLabel>

// KDE includes

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kptooldialog.h"
#include "comboboxintermediate.h"

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
}

namespace KWallet
{
    class Wallet;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIFlickrExportPlugin
{

class FlickrWidget;
class FlickrTalker;
class FlickrList;
class FPhotoInfo;
class GPhoto;
class GAlbum;
//class GAlbumViewItem;

using namespace KIPI;
using namespace KIPIPlugins;

class FlickrWindow : public KPToolDialog
{
    Q_OBJECT

public:

    FlickrWindow(const QString& tmpFolder, QWidget* const parent, const QString& serviceName);
    ~FlickrWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

private Q_SLOTS:

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
    void slotPopulatePhotoSetComboBox();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed(const QString& msg);
    void slotAddPhotoSetSucceeded();
    void slotListPhotoSetsFailed(const QString& msg);
    void slotAddPhotoCancelAndClose();
    void slotAuthCancel();
    void slotClose();
    void slotUser1();
    void slotImageListChanged();

private:

    void closeEvent(QCloseEvent*);
    void readSettings();
    void writeSettings();

private:

    unsigned int                           m_uploadCount;
    unsigned int                           m_uploadTotal;

    QString                                m_serviceName;

    QPushButton*                           m_newAlbumBtn;
    QPushButton*                           m_changeUserButton;

    KComboBox*                             m_albumsListComboBox;
    QCheckBox*                             m_publicCheckBox;
    QCheckBox*                             m_familyCheckBox;
    QCheckBox*                             m_friendsCheckBox;
    QCheckBox*                             m_exportHostTagsCheckBox;
    QCheckBox*                             m_stripSpaceTagsCheckBox;
    QCheckBox*                             m_addExtraTagsCheckBox;
    QCheckBox*                             m_resizeCheckBox;
    QCheckBox*                             m_sendOriginalCheckBox;

    QSpinBox*                              m_dimensionSpinBox;
    QSpinBox*                              m_imageQualitySpinBox;

    QPushButton*                           m_extendedPublicationButton;
    QPushButton*                           m_extendedTagsButton;
    ComboBoxIntermediate*                  m_contentTypeComboBox;
    ComboBoxIntermediate*                  m_safetyLevelComboBox;

//  QHash<int, GAlbumViewItem>             m_albumDict;

    QString                                m_token;
    QString                                m_username;
    QString                                m_userId;
    QString                                m_lastSelectedAlbum;
    QString                                m_tmp;

    QLabel*                                m_userNameDisplayLabel;

    QProgressDialog*                       m_authProgressDlg;

    QList< QPair<KUrl, FPhotoInfo> >       m_uploadQueue;

//  KWallet::Wallet*                       m_wallet;
    KHTMLPart*                             m_photoView;

    KLineEdit*                             m_tagsLineEdit;

    FlickrWidget*                          m_widget;
    FlickrTalker*                          m_talker;

    FlickrList*                            m_imglst;
};

} // namespace KIPIFlickrExportPlugin

#endif /* FLICKRWINDOW_H */
