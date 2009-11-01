/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to import/export images to/from
 *               SmugMug web service
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

#ifndef SMUGWINDOW_H
#define SMUGWINDOW_H

// Qt includes

#include <QList>

// KDE includes

#include <KDialog>

// LibKIPI includes

#include <libkipi/interface.h>

// local includes

#include "smugitem.h"

class QCloseEvent;

class KUrl;
class KProgressDialog;
class KPasswordDialog;

namespace KIPI
{
class Interface;
}

namespace KIPIPlugins
{
class KPAboutData;
}

namespace KIPISmugPlugin
{

class SmugTalker;
class SmugWidget;
class SmugNewAlbum;

class SmugWindow : public KDialog
{
    Q_OBJECT

public:

    SmugWindow(KIPI::Interface* interface, const QString& tmpFolder,
               bool import, QWidget *parent);
    ~SmugWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

protected:

    void closeEvent(QCloseEvent*);

private Q_SLOTS:

    void slotBusy(bool val);
    void slotLoginProgress(int step, int maxStep, const QString& label);
    void slotLoginDone(int errCode, const QString& errMsg);
    void slotAddPhotoDone(int errCode, const QString& errMsg);
    void slotGetPhotoDone(int errCode, const QString& errMsg, const QByteArray& photoData);
    void slotCreateAlbumDone(int errCode, const QString& errMsg, int newAlbumID);
    void slotListAlbumsDone(int errCode, const QString& errMsg, const QList <SmugAlbum>& albumsList);
    void slotListPhotosDone(int errCode, const QString& errMsg, const QList <SmugPhoto>& photosList);
    void slotListAlbumTmplDone(int errCode, const QString& errMsg, const QList <SmugAlbumTmpl>& albumTList);
    void slotListCategoriesDone(int errCode, const QString& errMsg, const QList <SmugCategory>& categoriesList);
    void slotListSubCategoriesDone(int errCode, const QString& errMsg, const QList <SmugCategory>& categoriesList);

    void slotUserChangeRequest(bool anonymous);
    void slotReloadAlbumsRequest();
    void slotNewAlbumRequest();
    void slotTransferCancel();
    void slotLoginCancel();
    void slotHelp();
    void slotClose();
    void slotStartTransfer();
    void slotImageListChanged();
    void slotTemplateSelectionChanged(int index);
    void slotCategorySelectionChanged(int index);

private:

    bool prepareImageForUpload(const QString& imgPath, bool isRAW);
    void uploadNextPhoto();
    void downloadNextPhoto();

    void readSettings();
    void writeSettings();

    void authenticate(const QString& email = "", const QString& password = "");

    void buttonStateChange(bool state);

private:

    bool                      m_import;
    unsigned int              m_imagesCount;
    unsigned int              m_imagesTotal;
    QString                   m_tmpDir;
    QString                   m_tmpPath;

    bool                      m_anonymousImport;
    QString                   m_anonymousNick;
    QString                   m_email;
    QString                   m_password;
    int                       m_currentAlbumID;
    int                       m_currentTmplID;
    int                       m_currentCategoryID;

    KProgressDialog          *m_progressDlg;
    KProgressDialog          *m_authProgressDlg;
    KPasswordDialog          *m_loginDlg;

    KUrl::List                m_transferQueue;

    SmugTalker               *m_talker;
    SmugWidget               *m_widget;
    SmugNewAlbum             *m_albumDlg;

    KIPI::Interface          *m_interface;

    KIPIPlugins::KPAboutData *m_about;
};

} // namespace KIPISmugPlugin

#endif /* SMUGWINDOW_H */
