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

#include <kdialogbase.h>
#include <libkipi/interface.h>

class ImageList;

class QCheckBox;
class QLabel;
class QPushButton;
class KComboBox;
class KFileItem;
class KLineEdit;
class KProgress;
class KURL;

class UploadDialog : public KDialogBase
{
    Q_OBJECT

    public:
        UploadDialog( KIPI::Interface* interface, QString caption, QWidget *parent=0 );
        ~UploadDialog()
        {
            if( m_itdb )
                itdb_photodb_free( m_itdb );
        }

        const QString getDestinationAlbum();

        QString mountPoint() { return m_mountPoint; }
        QString deviceNode() { return m_deviceNode; }

    private slots:
        void slotNewAlbumChecked( bool on );
        void slotProcessStart();
        void slotProcessFinished();

        void slotAddDropItems( QStringList filesPath );

        void slotImageSelected( QListViewItem * item );
        void slotGotPreview( const KFileItem* , const QPixmap &pixmap );

        void slotImagesFilesButtonAdd();
        void slotImagesFilesButtonRem();

    private:
        bool openDevice(); // connect to the ipod
        const QStringList getIPodAlbums();
        void addUrlToList( QString &file );

        KIPI::Interface *m_interface;
        Itdb_PhotoDB    *m_itdb;

        KComboBox       *m_albumCombo;
        QCheckBox       *m_newAlbumCheckBox;
        KLineEdit       *m_newAlbumLineEdit;
        KProgress       *m_progress;
        QPushButton     *m_addImagesButton;
        QPushButton     *m_remImagesButton;
        ImageList       *m_imageList;
        QLabel          *m_imageLabel;

        QString          m_mountPoint;
        QString          m_deviceNode;
};

#endif // IPOD_EXPORTDIALOG_H
