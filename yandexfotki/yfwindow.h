/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
 *
 * GUI based on PicasaWeb KIPI Plugin
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
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

#ifndef YF_WINDOW_H
#define YF_WINDOW_H

// Qt includes

#include <QStack>

// Local includes

#include "kptooldialog.h"
#include "logindialog.h"
#include "yftalker.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QGroupBox;
class QButtonGroup;
class QCloseEvent;
class QProgressBar;

class KUrl;
class KProgressDialog;
class KPasswordDialog;
class KComboBox;
class KPushButton;
class KLineEdit;

namespace KIPI
{
    class UploadWidget;
}

namespace KIPIPlugins
{
    class KPImagesList;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIYandexFotkiPlugin
{

class YandexFotkiWindow : public KPToolDialog
{
    Q_OBJECT

public:

    YandexFotkiWindow(bool import, QWidget* const parent);
    ~YandexFotkiWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

    QString getDestinationPath() const;

protected Q_SLOTS:

    // ui slots
    void slotChangeUserClicked();
    void slotResizeChecked();

    void slotError();
    void slotGetSessionDone();
    void slotGetTokenDone();
    void slotGetServiceDone();

    void slotListAlbumsDone(const QList<YandexFotkiAlbum>& albumsList);
    void slotListPhotosDone(const QList <YandexFotkiPhoto>& photosList);
    void slotListPhotosDoneForUpload(const QList <YandexFotkiPhoto>& photosList);
    void slotListPhotosDoneForDownload(const QList <YandexFotkiPhoto>& photosList);
    void slotUpdatePhotoDone(YandexFotkiPhoto& );
    void slotUpdateAlbumDone();

    void slotNewAlbumRequest();
    void slotReloadAlbumsRequest();

    void slotStartTransfer();

    void slotButtonClicked(int button);

protected:

    void closeEvent(QCloseEvent* event);

    void readSettings();
    void writeSettings();

    void reset();
    void updateControls(bool val);
    void updateLabels();

    void authenticate(bool forceAuthWindow);
    void updateNextPhoto();

protected:

    enum UpdatePolicy
    {
        POLICY_UPDATE_MERGE = 0,
        POLICY_UPDATE_KEEP, // is not used in GUI
        POLICY_SKIP,
        POLICY_ADDNEW
    };

    /*
     * Plugin
     */

    bool                     m_import;

    /*
     * User interface
     */

    QWidget*                 m_mainWidget;
    // accounts
    QGroupBox*               m_accountBox;
    QLabel*                  m_loginLabel;
    QLabel*                  m_headerLabel;
    KPushButton*             m_changeUserButton;

    // albums
    QGroupBox*               m_albumsBox;
    KPushButton*             m_newAlbumButton;
    KPushButton*             m_reloadAlbumsButton;
    KComboBox*               m_albumsCombo;

    // upload settings
    KComboBox*               m_accessCombo;
    QCheckBox*               m_hideOriginalCheck;
    QCheckBox*               m_disableCommentsCheck;
    QCheckBox*               m_adultCheck;
    QCheckBox*               m_resizeCheck;
    QSpinBox*                m_dimensionSpin;
    QSpinBox*                m_imageQualitySpin;
    QButtonGroup*            m_policyGroup;

    KPImagesList*            m_imgList;
    UploadWidget*            m_uploadWidget;

    QProgressBar*            m_progressBar;

    /*
     * Backend
     */
    QString                  m_tmpDir;
    YandexFotkiTalker        m_talker;

    QStack<YandexFotkiPhoto> m_transferQueue;

    // XMP id const for images
    static const char*       XMP_SERVICE_ID;
};

} // namespace KIPIYandexFotkiPlugin

#endif /* YF_WINDOW_H */
