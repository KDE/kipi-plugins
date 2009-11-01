/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009 by Timothée Groleau <kde at timotheegroleau dot com>
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

#ifndef SWWINDOW_H
#define SWWINDOW_H

// Qt includes

#include <QList>

// KDE includes

#include <KDialog>

// LibKIPI includes

#include <libkipi/interface.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

class QCloseEvent;

class KProgressDialog;
class KUrl;

namespace KIPI
{
class Interface;
}

namespace KIPIPlugins
{
class KPAboutData;
}

namespace KIPIShwupPlugin
{

class SwConnector;
class SwWidget;
class SwNewAlbum;
class SwLogin;
class SwAlbum;
class SwPhoto;

class SwWindow : public KDialog
{
    Q_OBJECT

public:

    SwWindow(KIPI::Interface *interface, const QString& tmpFolder, QWidget *parent);
    ~SwWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

private Q_SLOTS:

    void slotBusy(bool val);
    void slotShwupKipiBlackListed();
    void slotShwupInvalidCredentials();

    void slotRequestRestURLDone(int errCode, const QString& errMsg);
    void slotListAlbumsDone(int errCode, const QString& errMsg, const QList <SwAlbum>& albumsList);
    void slotCreateAlbumDone(int errCode, const QString& errMsg, const SwAlbum& album);
    void slotAddPhotoDone(int errCode, const QString& errMsg);

    void slotUserChangeRequest();
    void slotReloadAlbumsRequest();
    void slotNewAlbumRequest();
    void slotTransferCancel();
    void slotLoginInvalid();
    void slotHelp();
    void slotClose();
    void slotStartTransfer();
    void slotImageListChanged();

private:

    QString getImageCaption(const KExiv2Iface::KExiv2& ev);
    bool    prepareImageForUpload(const QString& imgPath, bool isRAW, QString& caption);
    void    uploadNextPhoto();

    void    readSettings();
    void    writeSettings();

    void    authenticate();

    void    buttonStateChange(bool state);

    void    closeEvent(QCloseEvent*);

private:

    unsigned int              m_imagesCount;
    unsigned int              m_imagesTotal;

    QString                   m_tmpDir;
    QString                   m_tmpPath;

    long long                 m_currentAlbumID;

    QList<SwAlbum>            m_albumsList;
    QString                   m_userEmail;
    QString                   m_userPassword;

    KProgressDialog          *m_progressDlg;
    SwLogin                  *m_loginDlg;

    KUrl::List                m_transferQueue;

    SwConnector              *m_connector;
    SwWidget                 *m_widget;
    SwNewAlbum               *m_albumDlg;

    KIPI::Interface          *m_interface;

    KIPIPlugins::KPAboutData *m_about;
};

} // namespace KIPIShwupPlugin

#endif /* SWWINDOW_H */
