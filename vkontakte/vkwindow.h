/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte web service
 *
 * Copyright (C) 2011 by Alexander Potashev <aspotashev at gmail dot com>
 *
 * GUI based on Yandex.Fotki KIPI Plugin
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
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

#ifndef VKONTAKTE_WINDOW_H
#define VKONTAKTE_WINDOW_H

// KDE includes

#include <kdialog.h>

// libkvkontakte includes

#include <libkvkontakte/albuminfo.h>


class QLabel;
class QSpinBox;
class QCheckBox;
class QGroupBox;
class QButtonGroup;
class QCloseEvent;
class QProgressBar;
class QToolButton;

class KUrl;
class KProgressDialog;
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
class ImagesList;
}

namespace KIPIVkontaktePlugin
{

class VkontakteWindow : public KDialog
{
    Q_OBJECT

public:

    VkontakteWindow(KIPI::Interface *interface,
                    bool import, QWidget *parent);
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
    void slotCloseEvent(QCloseEvent *event);
    void slotChangeUserClicked();

    // authentication
    void slotApplicationPermissionCheckDone(KJob *);
    void slotAuthenticationDialogDone(const QString &accessToken);
    void slotAuthenticationDialogCanceled();

    // requesting album information
    void startAlbumsUpdate();
    void startGetFullName();
    void startGetUserId();
    void startAlbumCreation(AlbumInfoPtr album);
    void startAlbumEditing(AlbumInfoPtr album);
    void slotAlbumsUpdateDone(KJob *);
    void slotGetFullNameDone(KJob *);
    void slotGetUserIdDone(KJob *);
    void slotAlbumCreationDone(KJob *);
    void slotAlbumEditingDone(KJob *);

    // requesting photo information
    void slotPhotoUploadDone(KJob *);

    void updateLabels();

    void slotNewAlbumRequest();
    void slotEditAlbumRequest();
    void slotReloadAlbumsRequest();

    void slotStartTransfer();

    void slotHelp();
    void slotButtonClicked(int button);

protected:

//     void readSettings();
//     void writeSettings();

    void reset();
    void updateControls(bool val);

    void handleVkError(KJob *job);

    // authentication
    bool isAuthenticated();
    void startAuthentication(bool forceAuthWindow);

protected:

    // Plugin
    bool m_import;
    KIPI::Interface *m_interface;

    // User interface
    QWidget *m_mainWidget;
    // accounts
    QGroupBox *m_accountBox;
    QLabel *m_loginLabel;
    QLabel *m_headerLabel;
    KPushButton *m_changeUserButton;

    // albums
    QGroupBox *m_albumsBox;
    KPushButton *m_newAlbumButton;
    KPushButton *m_reloadAlbumsButton;
    KComboBox *m_albumsCombo;
    QToolButton *m_editAlbumButton;

    KIPIPlugins::ImagesList *m_imgList;
    KIPI::UploadWidget *m_uploadWidget;

    QProgressBar *m_progressBar;


    QList<KJob *> m_jobs; /** Pointers to running jobs */
    bool m_authenticated;
    QString m_accessToken;
    QList<AlbumInfoPtr> m_albums;

    QString m_userFullName;
    int m_userId;
};

} // namespace KIPIVkontaktePlugin

#endif // VKONTAKTE_WINDOW_H
