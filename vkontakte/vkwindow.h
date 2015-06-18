/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte web service
 *
 * Copyright (C) 2011, 2012 by Alexander Potashev <aspotashev at gmail dot com>
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

namespace Vkontakte
{
    class VkApi;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIVkontaktePlugin
{

class AlbumChooserWidget;
class AuthInfoWidget;

class VkontakteWindow : public KPToolDialog
{
    Q_OBJECT

public:

    VkontakteWindow(bool import, QWidget* const parent);
    ~VkontakteWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void startReactivation();

Q_SIGNALS:

    void signalUpdateBusyStatus(bool busy);

protected Q_SLOTS:

    // requesting photo information
    void slotPhotoUploadDone(KJob *kjob);

    void slotStartTransfer();

    void slotButtonClicked(int button);

    void slotFinished();

    void updateBusyStatus(bool busy);
    void updateBusyStatusReady();

    void authenticated();
    void authCleared();
    void updateHeaderLabel();

protected:

    void initAccountBox();

    void readSettings();
    void writeSettings();

    void reset();

    void handleVkError(KJob* kjob);

    void closeEvent(QCloseEvent* event);

protected:

    /// Plugin
    bool                           m_import;

    /// User interface
    QWidget*                       m_mainWidget;
    QWidget*                       m_settingsBox;
    QLabel*                        m_headerLabel;

    /// accounts
    AuthInfoWidget*                m_accountBox;

    // album selection
    AlbumChooserWidget*            m_albumsBox;

    /// options
//     QCheckBox*                  m_checkKeepOriginal;

    KPImagesList*                  m_imgList;
    UploadWidget*                  m_uploadWidget;

    KPProgressWidget*              m_progressBar;

    /** Pointers to running jobs */
    QList<KJob*>                   m_jobs;

    Vkontakte::VkApi*              m_vkapi;

    int                            m_albumToSelect;

    QString                        m_appId;
};

} // namespace KIPIVkontaktePlugin

#endif // VKWINDOW_H
