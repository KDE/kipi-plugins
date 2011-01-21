/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.kipi-plugins.org
*
* Date        : 2010-02-15
* Description : a plugin to export to a remote Piwigo server.
*
* Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
* Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
* Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
* Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2010 by Frederic Coiffier <frederic dot coiffier at free dot com>
*
* This program is free software; you can redistribute it
* and/or modify it under the terms of the GNU General
* Public License as published by the Free Software Foundation;
* either version 2, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* ============================================================ */

#ifndef PIWIGOWINDOW_H
#define PIWIGOWINDOW_H

// KDE includes

#include <KDialog>

// Qt includes

#include <QList>
#include <QProgressDialog>

// Debug
#include <QTextStream>
#include <QFile>

namespace KIPI
{
class Interface;
}

namespace KIPIPlugins
{
class KPAboutData;
}

namespace KIPIPiwigoExportPlugin
{
class Piwigo;
class PiwigoTalker;
class GAlbum;
class GPhoto;

class PiwigoWindow : public KDialog
{
    Q_OBJECT

public:

    PiwigoWindow(KIPI::Interface *interface, QWidget *parent, Piwigo* pPiwigo);
    ~PiwigoWindow();

private:

    void connectSignals();
    void readSettings();
    QString cleanName(const QString&);

private Q_SLOTS:

    void slotDoLogin();
    void slotLoginFailed(const QString& msg);
    void slotBusy(bool val);
    void slotProgressInfo(const QString& msg);
    void slotError(const QString& msg);
    void slotAlbums(const QList<GAlbum>& albumList);
    void slotAlbumSelected();
    void slotAddPhoto();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed(const QString& msg);
    void slotAddPhotoCancel();
    void slotHelp();
    void slotEnableSpinBox(int n);
    void slotSettings();
    void slotProcessUrl(const QString&);

private:

    KIPI::Interface*          m_interface;
    KIPIPlugins::KPAboutData* m_about;
    PiwigoTalker*            m_talker;
    Piwigo*                  mpPiwigo;

    QProgressDialog*          m_progressDlg;
    unsigned int              m_uploadCount;
    unsigned int              m_uploadTotal;
    QStringList*              mpUploadList;
    QString                   firstAlbumName;

    class Private;
    Private* const d;
};

} // namespace KIPIPiwigoExportPlugin

#endif /* PIWIGOWINDOW_H */
