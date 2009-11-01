/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#ifndef FBWINDOW_H
#define FBWINDOW_H

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

namespace KIPIFacebookPlugin
{

class FbTalker;
class FbWidget;
class FbNewAlbum;
class FbUser;
class FbAlbum;
class FbPhoto;

class FbWindow : public KDialog
{
    Q_OBJECT

public:

    FbWindow(KIPI::Interface *interface, const QString& tmpFolder,
             bool import, QWidget *parent);
    ~FbWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

private Q_SLOTS:

    void slotBusy(bool val);
    void slotLoginProgress(int step, int maxStep, const QString& label);
    void slotLoginDone(int errCode, const QString& errMsg);
    void slotChangePermDone(int errCode, const QString& errMsg);
    void slotAddPhotoDone(int errCode, const QString& errMsg);
    void slotGetPhotoDone(int errCode, const QString& errMsg,
                          const QByteArray& photoData);
    void slotCreateAlbumDone(int errCode, const QString& errMsg,
                             long long newAlbumID);
    void slotListAlbumsDone(int errCode, const QString& errMsg,
                            const QList <FbAlbum>& albumsList);
    void slotListPhotosDone(int errCode, const QString& errMsg,
                            const QList <FbPhoto>& photosList);
    void slotListFriendsDone(int errCode, const QString& errMsg,
                             const QList <FbUser>& friendsList);

    void slotUserChangeRequest();
    void slotPermChangeRequest();
    void slotReloadAlbumsRequest(long long userID);
    void slotNewAlbumRequest();
    void slotTransferCancel();
    void slotLoginCancel();
    void slotHelp();
    void slotClose();
    void slotStartTransfer();
    void slotImageListChanged();

private:

    void    setProfileAID(long long userID);
    QString getImageCaption(const KExiv2Iface::KExiv2& ev);
    bool    prepareImageForUpload(const QString& imgPath, bool isRAW, QString& caption);

    void    uploadNextPhoto();
    void    downloadNextPhoto();

    void    readSettings();
    void    writeSettings();

    void    authenticate();

    void    buttonStateChange(bool state);

    void    closeEvent(QCloseEvent*);

private:

    bool                      m_import;
    unsigned int              m_imagesCount;
    unsigned int              m_imagesTotal;
    QString                   m_tmpDir;
    QString                   m_tmpPath;

    long long                 m_profileAID;
    long long                 m_currentAlbumID;
    QString                   m_sessionKey;
    QString                   m_sessionSecret;
    unsigned int              m_sessionExpires;

    KProgressDialog          *m_progressDlg;
    KProgressDialog          *m_authProgressDlg;

    KUrl::List                m_transferQueue;

    FbTalker                 *m_talker;
    FbWidget                 *m_widget;
    FbNewAlbum               *m_albumDlg;

    KIPI::Interface          *m_interface;

    KIPIPlugins::KPAboutData *m_about;
};

} // namespace KIPIFacebookPlugin

#endif /* FBWINDOW_H */
