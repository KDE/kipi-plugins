/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

// Qt includes.
#include <QList>

// KDE includes.
#include <kdialog.h>

// LibKIPI includes.
#include <libkipi/interface.h>

class QProgressDialog;
class KPasswordDialog;

class KUrl;

namespace KIPI
{
class Interface;
}

namespace KIPIPlugins
{
class KPAboutData;
}

namespace KIPIFbExportPlugin
{

class FbTalker;
class FbWidget;
class FbNewAlbum;
class FbAlbum;

class FbWindow : public KDialog
{
    Q_OBJECT

public:

    FbWindow(KIPI::Interface *interface, const QString &tmpFolder, QWidget *parent);
    ~FbWindow();

private slots:

    void slotBusy(bool val);
    void slotLoginDone(int errCode, const QString& errMsg);
    void slotAddPhotoDone(int errCode, const QString& errMsg);
    void slotCreateAlbumDone(int errCode, const QString& errMsg, 
                             long long newAlbumID);
    void slotListAlbumsDone(int errCode, const QString& errMsg,
                            const QList <FbAlbum>& albumsList);

    void slotUserChangeRequest();
    void slotReloadAlbumsRequest();
    void slotNewAlbumRequest();
    void slotAddPhotoCancel();
    void slotLoginCancel();
    void slotHelp();
    void slotClose();
    void slotStartUpload();
    void slotImageListChanged(bool);

private:
    bool prepareImageForUpload(const QString& imgPath, bool isRAW);
    void uploadNextPhoto();

    void readSettings();
    void writeSettings();
    void buttonStateChange(bool state);

private:

    unsigned int                m_uploadCount;
    unsigned int                m_uploadTotal;
    QString                     m_tmpDir;
    QString                     m_tmpPath;

    long long                   m_currentAlbumID;

    QProgressDialog            *m_progressDlg;
    QProgressDialog            *m_authProgressDlg;

    KUrl::List                  m_uploadQueue;

    FbTalker                   *m_talker;
    FbWidget                   *m_widget;
    FbNewAlbum                 *m_albumDlg;

    KIPI::Interface            *m_interface;

    KIPIPlugins::KPAboutData   *m_about;
};

} // namespace KIPIFbExportPlugin

#endif /* FBWINDOW_H */
