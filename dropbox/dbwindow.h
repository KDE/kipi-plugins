/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Dropbox web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#ifndef DBWINDOW_H
#define DBWINDOW_H

// Qt includes

#include <QList>
#include <QPair>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "kptooldialog.h"

class QCloseEvent;

class QUrl;

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

namespace KIPIDropboxPlugin
{
class DropboxWidget;
class DBTalker;
class DBPhoto;
class DBFolder;
class DBNewAlbum;

class DBWindow : public KPToolDialog
{
    Q_OBJECT

public:

    DBWindow(const QString& tmpFolder, QWidget* const parent);
    ~DBWindow();

    void reactivate();

private:

    void readSettings();
    void writeSettings();

    void uploadNextPhoto();

    void buttonStateChange(bool state);
    void closeEvent(QCloseEvent*) Q_DECL_OVERRIDE;

private Q_SLOTS:

    void slotImageListChanged();
    void slotUserChangeRequest();
    void slotNewAlbumRequest();
    void slotReloadAlbumsRequest();
    void slotStartTransfer();

    void slotBusy(bool);
    void slotSignalLinkingFailed();
    void slotSignalLinkingSucceeded();
    void slotSetUserName(const QString& msg);
    void slotListAlbumsFailed(const QString& msg);
    void slotListAlbumsDone(const QList<QPair<QString, QString> >& list);
    void slotCreateFolderFailed(const QString& msg);
    void slotCreateFolderSucceeded();
    void slotAddPhotoFailed(const QString& msg);
    void slotAddPhotoSucceeded();
    void slotTransferCancel();

    void slotFinished();

private:

    unsigned int         m_imagesCount;
    unsigned int         m_imagesTotal;

    QString              m_tmp;

    DropboxWidget*       m_widget;
    DBNewAlbum*          m_albumDlg;
    DBTalker*            m_talker;

    QString              m_currentAlbumName;

    QList<QUrl>          m_transferQueue;
};

} // namespace KIPIDropboxPlugin

#endif /* GDWINDOW_H */
