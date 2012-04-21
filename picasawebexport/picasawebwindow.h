/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Jens Mueller <tschenser at gmx dot de>
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

#ifndef PICASAWEBWINDOW_H
#define PICASAWEBWINDOW_H

// Qt includes

#include <QList>
#include <QLinkedList>
#include <QPair>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kptooldialog.h"
#include "kpaboutdata.h"

class KUrl;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPicasawebExportPlugin
{

class PicasaWebAlbum;
class PicasaWebPhoto;
class PicasawebNewAlbum;
class PicasawebWidget;
class PicasawebTalker;
//class FPhotoInfo;

class PicasawebWindow : public KPToolDialog
{
    Q_OBJECT

public:

    PicasawebWindow(Interface* const interface, const QString& tmpFolder,
                    bool import, QWidget *parent);
    ~PicasawebWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

private Q_SLOTS:

    void slotBusy(bool val);
    void slotLoginProgress(int step, int maxStep, const QString& label);
    void slotLoginDone(int errCode, const QString& errMsg);
    void slotAddPhotoDone(int errCode, const QString& errMsg, const QString& photoID);
    void slotGetPhotoDone(int errCode, const QString& errMsg, const QByteArray& photoData);
    void slotCreateAlbumDone(int errCode, const QString& errMsg, const QString& newAlbumID);
    void slotListAlbumsDone(int errCode, const QString& errMsg, const QList <PicasaWebAlbum>& albumsList);
    void slotListPhotosDoneForUpload(int errCode, const QString& errMsg,
                                     const QList <PicasaWebPhoto>& photosList);
    void slotListPhotosDoneForDownload(int errCode, const QString& errMsg,
                                       const QList <PicasaWebPhoto>& photosList);
    void slotUserChangeRequest(bool anonymous);
    void slotReloadAlbumsRequest();
    void slotNewAlbumRequest();
    void slotTransferCancel();
    void slotStartTransfer();
    void slotImageListChanged();
    void slotButtonClicked(int button);
    void slotStopAndCloseProgressBar();

private:

    bool prepareImageForUpload(const QString& imgPath, bool isRAW);
    void uploadNextPhoto();
    void downloadNextPhoto();
    void cancelProcessing();

    void readSettings();
    void writeSettings();

    void authenticate(const QString& token = QString(),
                      const QString& username = QString(),
                      const QString& password = QString());

    void buttonStateChange(bool state);

private:

    bool                                      m_import;
    unsigned int                              m_imagesCount;
    unsigned int                              m_imagesTotal;
    QString                                   m_tmpDir;
    QString                                   m_tmpPath;

    QString                                   m_token;
    QString                                   m_username;
    QString                                   m_password;
    QString                                   m_currentAlbumID;

    int                                       m_renamingOpt;

    QLinkedList< QPair<KUrl,PicasaWebPhoto> > m_transferQueue;

    PicasawebTalker*                          m_talker;
    PicasawebWidget*                          m_widget;
    PicasawebNewAlbum*                        m_albumDlg;

    Interface*                                m_interface;
};

} // namespace KIPIPicasawebExportPlugin

#endif /* PICASAWEBWINDOW_H */
