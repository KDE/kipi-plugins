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

#include "ipodexportdialog.h"

#include <qcheckbox.h>
#include <qfile.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>

#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmountpoint.h>
#include <kstandarddirs.h>
#include <kurl.h>

#define debug() kdDebug( 51000 )

UploadDialog::UploadDialog( KIPI::Interface* interface, QString caption, QWidget *parent )
    : KDialogBase( KDialogBase::Plain, caption, Help|User1|Cancel,
                   Cancel, parent, "SimpleUploadDialog", false, false, i18n("&Start"))
    , m_interface( interface )
    , m_itdb( 0 )
    , m_mountPoint( QString::null )
    , m_deviceNode( QString::null )
{
    KIPI::ImageCollection images = interface->currentSelection();

    if ( images.isValid() )
        m_selectedImages = images.images();

    QWidget       *box = plainPage();
    QVBoxLayout *dvlay = new QVBoxLayout( box, 6 );

    dvlay->setMargin( 2 );

    QFrame *headerFrame = new QFrame( box );
    headerFrame->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed
//     layout->setSpacing( 0 );

    QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
    pixmapLabelLeft->setScaledContents( false );

    QLabel *labelTitle = new QLabel( caption, headerFrame, "labelTitle" );

    layout->addWidget( pixmapLabelLeft );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );

    QString directory;
    KGlobal::dirs()->addResourceType( "kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data" );
    directory = KGlobal::dirs()->findResourceDir( "kipi_banner_left", "banner_left.png" );

    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    dvlay->addWidget( headerFrame );

    QHBoxLayout *albumLayout = new QHBoxLayout( this );
    QLabel      *albumLabel  = new QLabel( i18n("Destination &Album:" ), this );
    albumLayout->setMargin( 2 );

    openDevice();

    QStringList albums = getIPodAlbums();

    m_albumCombo = new KComboBox( this );
    m_albumCombo->insertStringList( albums );
    albumLabel->setBuddy( m_albumCombo );

    albumLayout->addWidget( albumLabel );
    albumLayout->addWidget( m_albumCombo );

    QHBoxLayout *newAlbumLayout = new QHBoxLayout( this );
    m_newAlbumCheckBox = new QCheckBox( i18n("Create a new album:"), this );
    m_newAlbumLineEdit = new KLineEdit( this );
    newAlbumLayout->setMargin( 2 );

    KIPI::ImageCollection album = m_interface->currentAlbum();
    if ( album.isValid() )
        m_newAlbumLineEdit->setText( album.name() ); //sane default

    newAlbumLayout->addWidget( m_newAlbumCheckBox );
    newAlbumLayout->addWidget( m_newAlbumLineEdit );

    dvlay->addLayout( albumLayout );
    dvlay->addLayout( newAlbumLayout );

    bool noAlbums = albums.isEmpty();
    m_newAlbumCheckBox->setChecked( noAlbums );
    m_newAlbumLineEdit->setEnabled( noAlbums );
    m_albumCombo->setEnabled( !noAlbums );

    connect( m_newAlbumCheckBox, SIGNAL( toggled(bool) ), SLOT( slotNewAlbumChecked(bool) ) );

    connect( this, SIGNAL( user1Clicked() ), SLOT( slotProcessStart() ) );
}


const QStringList
UploadDialog::getIPodAlbums()
{
    KIPI::ImageCollection selection = m_interface->currentSelection();

    if ( !selection.isValid() ) return QStringList();

    KURL::List images = selection.images();

    QStringList albums;
    for( GList *it = m_itdb->photoalbums; it; it = it->next )
    {
        Itdb_PhotoAlbum *ipodAlbum = (Itdb_PhotoAlbum *)it->data;
        albums << ipodAlbum->name;
    }

    return albums;
}

const QString
UploadDialog::getDestinationAlbum()
{
    if( m_newAlbumCheckBox->isChecked() )
        return m_newAlbumLineEdit->text();

    return m_albumCombo->currentText();
}

void
UploadDialog::slotNewAlbumChecked( bool on )
{
    m_newAlbumLineEdit->setEnabled( on );
    m_albumCombo->setEnabled( !on );
}

void
UploadDialog::slotProcessStart()
{
    QString albumName = getDestinationAlbum();

    for( KURL::List::Iterator it = m_selectedImages.begin();
         it != m_selectedImages.end(); ++it )
    {
        debug() << "Uploading " << (*it).path().utf8() << " to ipod album " << albumName.utf8() << endl;
        KIPI::ImageInfo info = m_interface->info( *it );

        itdb_photodb_add_photo( m_itdb, albumName.utf8(), (*it).path().utf8() );
    }

    debug() << "Writing database" << endl;
    GError *err = 0;
    if( !itdb_photodb_write( m_itdb, &err ) )
        debug() << "OH CRAP!" << endl;

    debug() << "Finished writing database" << endl;

    if( err )
        debug() << "Failed with error: " << err << endl;
}


bool
UploadDialog::openDevice()
{
    if( m_itdb )
    {
        debug() <<  "iPod at " << mountPoint() << " already opened" << endl;
        return false;
    }


    // try to find a mounted ipod
    bool ipodFound = false;
    KMountPoint::List currentmountpoints = KMountPoint::currentMountPoints();
    for( KMountPoint::List::Iterator mountiter = currentmountpoints.begin();
         mountiter != currentmountpoints.end();
         ++mountiter )
    {
        QString devicenode = (*mountiter)->mountedFrom();
        QString mountpoint = (*mountiter)->mountPoint();

        if( !mountPoint().isEmpty() &&
             mountpoint != mountPoint() )
            continue;

        if( mountpoint.startsWith( "/proc" ) ||
            mountpoint.startsWith( "/sys" )  ||
            mountpoint.startsWith( "/dev" )  ||
            mountpoint.startsWith( "/boot" ) )
            continue;

        if( !deviceNode().isEmpty() &&
             devicenode != deviceNode() )
            continue;

        GError *err = 0;
        m_itdb = itdb_photodb_parse( QFile::encodeName( mountpoint ), &err );
        if( err )
        {
            debug() << "could not parse itdb at " << mountpoint << endl;
            g_error_free( err );
            if( m_itdb )
            {
                itdb_photodb_free( m_itdb );
                m_itdb = 0;
            }
            continue;
        }

        if( mountPoint().isEmpty() )
            m_mountPoint = mountpoint;
        ipodFound = true;
        break;
    }

    if( !ipodFound )
    {
        debug() << "No mounted iPod found" << endl;
        return false;
    }

    debug() << "iPod found mounted at " << mountPoint() << endl;

    if( !m_itdb )
    {
        debug() << "could not find iTunesDB on device mounted at " << mountPoint()
                << ". Trying to create a new one. " << endl;

        m_itdb = itdb_photodb_new();

        if( !m_itdb )
        {
            debug() << "Could not initialise photodb..." << endl;
            return false;
        }
    }

    itdb_device_set_mountpoint( m_itdb->device, mountPoint().utf8() );

    return true;
}
