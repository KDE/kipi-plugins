/* ============================================================
 * File  : gallerywindow.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * ============================================================ */

#ifndef GALLERYWINDOW_H
#define GALLERYWINDOW_H

#include <KDialog>

#include <q3valuelist.h>
#include <qpair.h>
#include <q3intdict.h>

class Q3ListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class Q3ProgressDialog;
class KHTMLPart;
class KUrl;

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

class Galleries;
class GalleryTalker;
class GAlbum;
class GPhoto;
class GAlbumViewItem;

class GalleryWindow : public KDialog
{
    Q_OBJECT

public:

    GalleryWindow(KIPI::Interface *interface, QWidget *parent, Galleries* pGalleries);
    ~GalleryWindow();

private:

    Q3ListView                *m_albumView;
    KHTMLPart                *m_photoView;
    QPushButton              *m_newAlbumBtn;
    QPushButton              *m_addPhotoBtn;
    QPushButton              *m_helpButton;
    QCheckBox                *m_captTitleCheckBox;
    QCheckBox                *m_captDescrCheckBox;
    QCheckBox                *m_resizeCheckBox;
    QSpinBox                 *m_dimensionSpinBox;
    GalleryTalker            *m_talker;
    Q3IntDict<GAlbumViewItem>  m_albumDict;
    QString                   m_lastSelectedAlbum;
    KIPI::Interface          *m_interface;
    KIPIPlugins::KPAboutData *m_about;

    Q3ProgressDialog                      *m_progressDlg;
    unsigned int                          m_uploadCount;
    unsigned int                          m_uploadTotal;
    Q3ValueList< QPair<QString,QString> >  m_uploadQueue;

    Galleries* mpGalleries;
    Gallery*   mpGallery;

private slots:

    void slotDoLogin();
    void slotLoginFailed( const QString& msg );
    void slotBusy( bool val );
    void slotError( const QString& msg );
    void slotAlbums( const Q3ValueList<GAlbum>& albumList );
    void slotPhotos( const Q3ValueList<GPhoto>& photoList );
    void slotAlbumSelected();
    void slotOpenPhoto( const KUrl& url );
    void slotNewAlbum();
    void slotAddPhotos();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed( const QString& msg );
    void slotAddPhotoCancel();
    void slotHelp();
};

}

#endif /* GALLERYWINDOW_H */
