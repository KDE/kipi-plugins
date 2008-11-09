//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
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

class QCheckBox;
class Q3HGroupBox;
class QLabel;
class QPushButton;
class KComboBox;
class KFileItem;
class KLineEdit;
class K3ListView;
class K3ListViewItem;
class KUrl;

namespace IpodExport
{

class ImageList;
class IpodAlbumItem;
class IpodPhotoItem;
class IpodHeader;

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
        void ipodShowContextMenu( Q3ListViewItem * ) { }

        void imagesFilesButtonAdd();
        void imagesFilesButtonRem();

        void createIpodAlbum();
        void deleteIpodAlbum();
        void renameIpodAlbum();

        void refreshDevices();
        void updateSysInfo();

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
        KIPI::Interface *m_interface;
#endif
        Itdb_PhotoDB    *m_itdb;
        Itdb_IpodInfo   *m_ipodInfo;
        IpodHeader      *m_ipodHeader;
        bool             m_transferring;

        Q3ListViewItem   *m_destinationAlbum;

        QPushButton     *m_createAlbumButton;
        QPushButton     *m_removeAlbumButton;
        QPushButton     *m_renameAlbumButton;
        QPushButton     *m_addImagesButton;
        QPushButton     *m_remImagesButton;
        QPushButton     *m_transferImagesButton;
        ImageList       *m_uploadList;
        K3ListView       *m_ipodAlbumList;
        QLabel          *m_imagePreview;
        QLabel          *m_ipodPreview;

        Q3HGroupBox      *m_destinationBox;
        Q3HGroupBox      *m_urlListBox;

        QString          m_mountPoint;
        QString          m_deviceNode;

        static UploadDialog *s_instance;
};

}

#endif // IPOD_EXPORTDIALOG_H
