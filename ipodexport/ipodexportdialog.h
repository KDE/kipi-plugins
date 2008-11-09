/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <me at sebruiz dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef IPOD_EXPORTDIALOG_H
#define IPOD_EXPORTDIALOG_H

extern "C"
{
#include <gpod/itdb.h>
}

#define KIPI_PLUGIN 1

// Qt includes.

#include <QPixmap>
#include <QLabel>

// KDE includes.

#include <kdialog.h>

// Kipi includes.

#if KIPI_PLUGIN
#include <libkipi/interface.h>
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "imagedialog.h"
#endif

class Q3ListViewItem;
class Q3HGroupBox;

class QCheckBox;
class QLabel;
class QPushButton;

class K3ListView;
class K3ListViewItem;

class KComboBox;
class KFileItem;
class KLineEdit;
class KUrl;

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIIpodExportPlugin
{

class ImageList;
class IpodAlbumItem;
class IpodPhotoItem;
class IpodHeader;

class UploadDialog : public KDialog
{
Q_OBJECT

public:

    UploadDialog(
#if KIPI_PLUGIN
                 KIPI::Interface* interface,
#endif
                 const QString& caption, QWidget *parent=0 );

    ~UploadDialog();

    static UploadDialog *instance() { return s_instance; }

    QString ipodModel() const;
    QString mountPoint() { return m_mountPoint; }
    QString deviceNode() { return m_deviceNode; }

private slots:

    void startTransfer();

    void addDropItems( QStringList filesPath );

    void imageSelected( Q3ListViewItem *item );
    void gotImagePreview( const KFileItem* , const QPixmap &pixmap );

    void ipodItemSelected( Q3ListViewItem *item );
    void ipodShowContextMenu( Q3ListViewItem * ) {};

    void imagesFilesButtonAdd();
    void imagesFilesButtonRem();

    void createIpodAlbum();
    void deleteIpodAlbum();
    void renameIpodAlbum();

    void refreshDevices();
    void updateSysInfo();

    void slotHelp();

private:

    void addUrlToList( QString file );
    bool deleteIpodAlbum( IpodAlbumItem *album );
    bool deleteIpodPhoto( IpodPhotoItem *photo );
    void enableButtons();
    void getIpodAlbums();
    void getIpodAlbumPhotos( IpodAlbumItem *item, Itdb_PhotoAlbum *album );
    Itdb_Artwork *photoFromId( const uint id );
    void reloadIpodAlbum( IpodAlbumItem *item, Itdb_PhotoAlbum *album );

    bool openDevice(); // connect to the ipod

#if KIPI_PLUGIN
    Interface       *m_interface;
    KPAboutData     *m_about;
#endif

    Itdb_PhotoDB    *m_itdb;
    Itdb_IpodInfo   *m_ipodInfo;
    IpodHeader      *m_ipodHeader;
    bool             m_transferring;

    Q3ListViewItem  *m_destinationAlbum;

    Q3HGroupBox     *m_destinationBox;
    Q3HGroupBox     *m_urlListBox;

    QPushButton     *m_createAlbumButton;
    QPushButton     *m_removeAlbumButton;
    QPushButton     *m_renameAlbumButton;
    QPushButton     *m_addImagesButton;
    QPushButton     *m_remImagesButton;
    QPushButton     *m_transferImagesButton;

    QLabel          *m_imagePreview;
    QLabel          *m_ipodPreview;

    QString          m_mountPoint;
    QString          m_deviceNode;

    K3ListView      *m_ipodAlbumList;

    ImageList       *m_uploadList;

    static UploadDialog *s_instance;
};

} // namespace KIPIIpodExportPlugin

#endif // IPOD_EXPORTDIALOG_H
