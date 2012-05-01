/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte web service
 *
 * Copyright (C) 2011 by Alexander Potashev <aspotashev at gmail dot com>
 *
 * GUI based on Yandex.Fotki KIPI Plugin
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010      by Roman Tsisyk <roman at tsisyk dot com>
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

#ifndef VKWINDOW_H
#define VKWINDOW_H

// libkvkontakte includes

#include <libkvkontakte/albuminfo.h>

// Local includes

#include "kptooldialog.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QGroupBox;
class QButtonGroup;
class QCloseEvent;
class QToolButton;

class KUrl;
class KPasswordDialog;
class KComboBox;
class KPushButton;
class KLineEdit;
class KJob;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins
{
    class KPImagesList;
    class KPProgressWidget;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIVkontaktePlugin
{

class VkontakteWindow : public KPToolDialog
{
    Q_OBJECT

public:

    VkontakteWindow(Interface* const interface,
                    bool import, QWidget* const parent);
    ~VkontakteWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void startReactivation();

    QString getDestinationPath() const;

Q_SIGNALS:

    void signalAuthenticationDone();

protected Q_SLOTS:

    // ui slots
    void slotChangeUserClicked();

    // authentication
    void slotApplicationPermissionCheckDone(KJob *kjob);
    void slotAuthenticationDialogDone(const QString &accessToken);
    void slotAuthenticationDialogCanceled();

    // requesting album information
    void startAlbumsUpdate();
    void startGetFullName();
    void startGetUserId();
    void startAlbumCreation(Vkontakte::AlbumInfoPtr album);
    void startAlbumEditing(Vkontakte::AlbumInfoPtr album);
    void startAlbumDeletion(Vkontakte::AlbumInfoPtr album);
    void slotAlbumsUpdateDone(KJob *kjob);
    void slotGetFullNameDone(KJob *kjob);
    void slotGetUserIdDone(KJob *kjob);
    void slotAlbumCreationDone(KJob *kjob);
    void slotAlbumEditingDone(KJob *kjob);
    void slotAlbumDeletionDone(KJob *kjob);

    // requesting photo information
    void slotPhotoUploadDone(KJob *kjob);

    void slotNewAlbumRequest();
    void slotEditAlbumRequest();
    void slotDeleteAlbumRequest();
    void slotReloadAlbumsRequest();

    void slotStartTransfer();

    void slotButtonClicked(int button);

    void slotFinished();

protected:

    void readSettings();
    void writeSettings();

    void reset();
    void updateControls(bool val);
    void updateLabels();

    void handleVkError(KJob *kjob);

    // authentication
    bool isAuthenticated();
    void startAuthentication(bool forceLogout);

    Vkontakte::AlbumInfoPtr currentAlbum();
    void selectAlbum(int aid);

protected:

    /// Plugin
    bool                           m_import;
    Interface*                     m_interface;

    /// User interface
    QWidget*                       m_mainWidget;

    /// accounts
    QGroupBox*                     m_accountBox;
    QLabel*                        m_loginLabel;
    QLabel*                        m_headerLabel;
    KPushButton*                   m_changeUserButton;

    /// albums
    QGroupBox*                     m_albumsBox;
    KPushButton*                   m_newAlbumButton;
    KPushButton*                   m_reloadAlbumsButton;
    KComboBox*                     m_albumsCombo;
    QToolButton*                   m_editAlbumButton;
    QToolButton*                   m_deleteAlbumButton;

    /// options
//     QCheckBox*                  m_checkKeepOriginal;

    KPImagesList*                  m_imgList;
    UploadWidget*                  m_uploadWidget;

    KPProgressWidget*              m_progressBar;

    /** Pointers to running jobs */
    QList<KJob*>                   m_jobs;

    bool                           m_authenticated;
    QString                        m_accessToken;
    QList<Vkontakte::AlbumInfoPtr> m_albums;

    QString                        m_userFullName;
    int                            m_userId;

    /** album with this "aid" will be selected in slotAlbumsUpdateDone() */
    int                            m_albumToSelect;

    QString                        m_appId;
};

} // namespace KIPIVkontaktePlugin

#endif // VKWINDOW_H
