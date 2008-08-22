/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-24
 * Description : 
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 *
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

#ifndef GALLERYWINDOW_H
#define GALLERYWINDOW_H

// KDE includes
#include <KDialog>

// Qt includes
#include <QList>
#include <QPair>
#include <QHash>
// #include <KProgressDialog>

namespace KIPI
{
class Interface;
}

namespace KIPIPlugins
{
class KPAboutData;
}

namespace KIPIGalleryExportPlugin
{

class Gallery;
class GalleryTalker;
class GAlbum;
class GPhoto;
class GAlbumViewItem;

class GalleryWindow : public KDialog
{
    Q_OBJECT

public:

    GalleryWindow(KIPI::Interface *interface, QWidget *parent, Gallery* pGallery);
    ~GalleryWindow();

private:

    class Private;
    Private* const d;

    KIPI::Interface* m_interface;
    KIPIPlugins::KPAboutData *m_about;
    GalleryTalker* m_talker;
    Gallery* mpGallery;
//     KProgressDialog* m_progressDlg;

    void connectSignals();
    void readSettings();

private slots:

    void slotDoLogin();
    void slotLoginFailed(const QString& msg);
    void slotBusy(bool val); // not in flickrexport
    void slotError(const QString& msg);
    void slotAlbums(const QList<GAlbum>& albumList); //not in flickrexport
    void slotPhotos(const QList<GPhoto>& photoList); //not in flickrexport
    void slotAlbumSelected();
    void slotOpenPhoto(const KUrl& url); //not in flickrexport
    void slotNewAlbum();
    void slotAddPhotos();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed(const QString& msg);
    void slotAddPhotoCancel();
    void slotHelp();

//     void slotUploadFinished(int id, bool error);
//     void slotUploadProgress(int done, int total);
};

}

#endif /* GALLERYWINDOW_H */
