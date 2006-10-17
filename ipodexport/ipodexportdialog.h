/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IPOD_EXPORTDIALOG_H
#define IPOD_EXPORTDIALOG_H

extern "C" {
#include <gpod/itdb.h>
}

#define KIPI_PLUGIN 1

#include <kdialogbase.h>

#if KIPI_PLUGIN
#include <libkipi/interface.h>
#endif

class ImageList;

class QCheckBox;
class QLabel;
class QPushButton;
class KComboBox;
class KFileItem;
class KLineEdit;
class KListView;
class KListViewItem;
class KProgress;
class KURL;

namespace IpodExport
{

class UploadDialog : public KDialogBase
{
    Q_OBJECT

    public:
        UploadDialog(
                        #if KIPI_PLUGIN
                        KIPI::Interface* interface,
                        #endif
                        QString caption, QWidget *parent=0 );

        ~UploadDialog()
        {
            if( m_itdb )
                itdb_photodb_free( m_itdb );
        }

        QString mountPoint() { return m_mountPoint; }
        QString deviceNode() { return m_deviceNode; }

    private slots:
        void slotProcessStart();

        void slotAddDropItems( QStringList filesPath );

        void slotImageSelected( QListViewItem *item );
        void slotGotPreview( const KFileItem* , const QPixmap &pixmap );

        void slotIpodItemSelected( QListViewItem *item );
        void slotIpodShowContextMenu( QListViewItem * ) { }

        void slotImagesFilesButtonAdd();
        void slotImagesFilesButtonRem();

        void slotCreateIpodAlbum();
        void slotDeleteIpodAlbum();
        void slotRenameIpodAlbum();

    private:
        void addUrlToList( QString file );
        void enableButtons();
        void getIpodAlbums();
        void getIpodAlbumPhotos( KListViewItem *item, Itdb_PhotoAlbum *album );
        void reloadIpodAlbum( KListViewItem *item, Itdb_PhotoAlbum *album );

        bool openDevice(); // connect to the ipod

#if KIPI_PLUGIN
        KIPI::Interface *m_interface;
#endif
        Itdb_PhotoDB    *m_itdb;
        bool             m_transferring;

        KProgress       *m_progress;
        QPushButton     *m_createAlbumButton;
        QPushButton     *m_removeAlbumButton;
        QPushButton     *m_renameAlbumButton;
        QPushButton     *m_addImagesButton;
        QPushButton     *m_remImagesButton;
        ImageList       *m_imageList;
        KListView       *m_ipodAlbumList;
        QLabel          *m_imagePreview;
        QLabel          *m_ipodPreview;

        QString          m_mountPoint;
        QString          m_deviceNode;
};

}

#endif // IPOD_EXPORTDIALOG_H
