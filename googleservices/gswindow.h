/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#ifndef GSWINDOW_H
#define GSWINDOW_H

// Qt includes

#include <QList>
#include <QPair>

// Libkipi includes

#include <libkipi/interface.h>

// Local includes

#include "kptooldialog.h"

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

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIGoogleServicesPlugin
{
class GoogleServicesWidget;
class GDTalker;
class GSPhoto;
class GSFolder;
class NewAlbumDlg;
class PicasawebTalker;

class GSWindow : public KPToolDialog
{
    Q_OBJECT

public:

    GSWindow(const QString& tmpFolder, QWidget* const parent, const QString& serviceName);
    ~GSWindow();

    void reactivate();

private:

    void readSettings();
    void writeSettings();

    void uploadNextPhoto();
    void downloadNextPhoto();

    void buttonStateChange(bool state);
    void closeEvent(QCloseEvent*);
    void picasaTransferHandler();

private Q_SLOTS:

    void slotImageListChanged();
    void slotUserChangeRequest();
    void slotNewAlbumRequest();
    void slotReloadAlbumsRequest();
    void slotStartTransfer();
    void slotCloseClicked();
    //void slotChangeProgressBar();

    void slotBusy(bool);
    void slotTextBoxEmpty();
    void slotAccessTokenFailed(int errCode,const QString& errMsg);
    void slotAccessTokenObtained();
    void slotRefreshTokenObtained(const QString& msg);
    void slotSetUserName(const QString& msg);
    void slotListAlbumsDone(int,const QString&,const QList <GSFolder>&);
    void slotListPhotosDoneForDownload(int errCode, const QString& errMsg, const QList <GSPhoto>& photosList);
    void slotListPhotosDoneForUpload(int errCode, const QString& errMsg, const QList <GSPhoto>& photosList);
    void slotCreateFolderDone(int,const QString& msg, const QString& = QString("-1"));
    void slotAddPhotoDone(int,const QString& msg, const QString&);
    void slotGetPhotoDone(int errCode, const QString& errMsg, const QByteArray& photoData);
    void slotTransferCancel();

private:

    unsigned int                  m_imagesCount;
    unsigned int                  m_imagesTotal;
    int                           m_renamingOpt;

    QString                       m_serviceName;
    QString                       m_pluginName;
    bool                          m_picasaExport;
    bool                          m_picasaImport;
    bool                          m_gdrive;
    QString                       m_tmp;
    QString                       refresh_token;

    GoogleServicesWidget*         m_widget;
    NewAlbumDlg*                  m_albumDlg;
    NewAlbumDlg*                  m_picasa_albumdlg;
    
    GDTalker*                     m_talker;
    PicasawebTalker*              m_picsasa_talker;

    QString                       m_currentAlbumId;

    QList< QPair<KUrl, GSPhoto> > m_transferQueue;
};

} // namespace KIPIGoogleServicesPlugin

#endif /* GSWINDOW_H */
