/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to SmugMug web service
 *
 * Copyright (C) 2005-2009 by Vardhman Jain <vardhman at gmail dot com>
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

#ifndef SMUGWINDOW_H
#define SMUGWINDOW_H

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

namespace KIPISmugExportPlugin
{

class SmugTalker;
class SmugWidget;
class SmugNewAlbum;
class SmugAlbum;
class SmugCategory;

class SmugWindow : public KDialog
{
    Q_OBJECT

public:

    SmugWindow(KIPI::Interface *interface, const QString &tmpFolder, QWidget *parent);
    ~SmugWindow();

private slots:

    void slotBusy(bool val);
    void slotLoginDone(int errCode, const QString& errMsg);
    void slotAddPhotoDone(int errCode, const QString& errMsg);
    void slotCreateAlbumDone(int errCode, const QString& errMsg, 
                             int newAlbumID);
    void slotListAlbumsDone(int errCode, const QString& errMsg,
                            const QList <SmugAlbum>& albumsList);
    void slotListCategoriesDone(int errCode, const QString& errMsg,
                                const QList <SmugCategory>& categoriesList);
    void slotListSubCategoriesDone(int errCode, const QString& errMsg,
                                   const QList <SmugCategory>& categoriesList);

    void slotUserChangeRequest();
    void slotReloadAlbumsRequest();
    void slotNewAlbumRequest();
    void slotAddPhotoCancel();
    void slotLoginCancel();
    void slotHelp();
    void slotClose();
    void slotStartUpload();
    void slotImageListChanged(bool);
    void slotCategorySelectionChanged(int index);

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

    QString                     m_email;
    QString                     m_password;
    int                         m_currentAlbumID;
    int                         m_currentCategoryID;

    QProgressDialog            *m_progressDlg;
    QProgressDialog            *m_authProgressDlg;
    KPasswordDialog            *m_loginDlg;

    KUrl::List                  m_uploadQueue;

    SmugTalker                 *m_talker;
    SmugWidget                 *m_widget;
    SmugNewAlbum               *m_albumDlg;

    KIPI::Interface            *m_interface;

    KIPIPlugins::KPAboutData   *m_about;
};

} // namespace KIPISmugExportPlugin

#endif /* SMUGWINDOW_H */
