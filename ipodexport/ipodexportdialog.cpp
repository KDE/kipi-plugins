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
#include "imagelist.h"
#include "imagelistitem.h"

#include <qcheckbox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qframe.h>
#include <qhgroupbox.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qvgroupbox.h>
#include <qwhatsthis.h>
#include <qwmatrix.h>

#include <kcombobox.h>
#include <kdebug.h>
#include <kfileitem.h>
#include <kio/jobclasses.h>
#include <kio/global.h>
#include <kio/previewjob.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmountpoint.h>
#include <kprogress.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include <libkipi/imagedialog.h>

#define debug() kdDebug( 51000 )

UploadDialog::UploadDialog( KIPI::Interface* interface, QString caption, QWidget *parent )
    : KDialogBase( KDialogBase::Plain, caption, Help|User1|Cancel,
                   Cancel, parent, "SimpleUploadDialog", false, false, i18n("&Start"))
    , m_interface( interface )
    , m_itdb( 0 )
    , m_mountPoint( QString::null )
    , m_deviceNode( QString::null )
{
    QWidget       *box = plainPage();
    QVBoxLayout *dvlay = new QVBoxLayout( box, 6 );

    dvlay->setMargin( 2 );

    QFrame *headerFrame = new QFrame( box );
    headerFrame->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed

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


    if( !openDevice() )
    {
        /// No iPod warning frame
        QLabel *warning = new QLabel( i18n("<p align=\"center\"><b>No iPod was detected</b></p>"), box );
        warning->setPaletteBackgroundColor( QColor(147,18,18) );
        warning->setPaletteForegroundColor( Qt::white );

        warning->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

        dvlay->addWidget( warning );

        enableButton( KDialogBase::User1, false );
    }


    QHBoxLayout *albumLayout = new QHBoxLayout( box );
    QLabel      *albumLabel  = new QLabel( i18n("Destination &Album:" ), box );
    albumLayout->setMargin( 2 );

    QStringList albums = getIPodAlbums();

    m_albumCombo = new KComboBox( box );
    m_albumCombo->insertStringList( albums );
    albumLabel->setBuddy( m_albumCombo );

    albumLayout->addWidget( albumLabel );
    albumLayout->addWidget( m_albumCombo );

    QHBoxLayout *newAlbumLayout = new QHBoxLayout( box );
    m_newAlbumCheckBox = new QCheckBox( i18n("Create a new album:"), box );
    m_newAlbumLineEdit = new KLineEdit( box );
    newAlbumLayout->setMargin( 2 );

    KIPI::ImageCollection album = m_interface->currentAlbum();
    if ( album.isValid() )
        m_newAlbumLineEdit->setText( album.name() ); //sane default

    newAlbumLayout->addWidget( m_newAlbumCheckBox );
    newAlbumLayout->addWidget( m_newAlbumLineEdit );

    dvlay->addLayout( albumLayout );
    dvlay->addLayout( newAlbumLayout );

    QHGroupBox *urlListBox = new QHGroupBox( box );
    QWidget* urlBox = new QWidget( urlListBox );
    QHBoxLayout* urlLayout = new QHBoxLayout( urlBox, 0, spacingHint() );
    m_imageList = new ImageList( urlBox );
    urlLayout->addWidget( m_imageList );

    m_imageList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

    QVBoxLayout* urlLayout_1 = new QVBoxLayout( urlLayout );
    m_addImagesButton = new QPushButton ( i18n( "&Add..." ), urlBox );
    urlLayout_1->addWidget( m_addImagesButton );
    QWhatsThis::add( m_addImagesButton, i18n("Add images to be queued for the iPod.") );

    m_remImagesButton = new QPushButton ( i18n( "&Remove" ), urlBox );
    urlLayout_1->addWidget( m_remImagesButton );
    QWhatsThis::add( m_remImagesButton, i18n("Remove selected image from the list.") );

    m_imageLabel = new QLabel( urlBox );
    m_imageLabel->setFixedHeight( 80 );
    m_imageLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_imageLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    urlLayout_1->addWidget( m_imageLabel );
    QWhatsThis::add( m_imageLabel, i18n( "The preview of the selected image in the list." ) );
    urlLayout_1->addStretch( 1 );

    dvlay->addWidget( urlListBox );

    m_progress = new KProgress( box, "Progress" );
    m_progress->setTotalSteps(100);
    m_progress->setValue(0);
    QWhatsThis::add( m_progress, i18n("This is the current percentage of the task completed.") );

    dvlay->addWidget( m_progress );

    /// enable/disable as appropriate
    bool noAlbums = albums.isEmpty();
    // disable the options if there is no ipod
    // default to add a new album if no albums are found
    m_newAlbumCheckBox->setChecked( noAlbums && m_itdb );
    m_newAlbumCheckBox->setEnabled( m_itdb );
    m_newAlbumLineEdit->setEnabled( noAlbums && m_itdb );
    m_albumCombo->setEnabled( !noAlbums && m_itdb );

    urlListBox->setEnabled( m_itdb );
    m_progress->setEnabled( m_itdb );

    /// add selected items to the ImageList
    KIPI::ImageCollection images = interface->currentSelection();

    if ( images.isValid() )
    {
        KURL::List selected = images.images();
        for( KURL::List::Iterator it = selected.begin(); it != selected.end(); ++it )
        {
            addUrlToList( (*it).path() );
        }
    }


    connect( m_newAlbumCheckBox, SIGNAL( toggled(bool) ), SLOT( slotNewAlbumChecked(bool) ) );

    connect( this, SIGNAL( user1Clicked() ), SLOT( slotProcessStart() ) );

    connect( m_imageList, SIGNAL( addedDropItems(QStringList) ),
                    this,   SLOT( slotAddDropItems(QStringList) ) );

    connect( m_imageList, SIGNAL( currentChanged(QListViewItem*) ),
                    this,   SLOT( slotImageSelected(QListViewItem*) ) );

    connect( m_addImagesButton, SIGNAL( clicked() ),
                    this,         SLOT( slotImagesFilesButtonAdd() ) );

    connect( m_remImagesButton, SIGNAL( clicked() ),
                    this,         SLOT( slotImagesFilesButtonRem() ) );
}


const QStringList
UploadDialog::getIPodAlbums()
{
    if( !m_itdb )
        return QStringList();

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
    enableButton( KDialogBase::User1, false );

    if( !m_itdb )
        return;

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
    itdb_photodb_write( m_itdb, &err );

    debug() << "Finished writing database" << endl;

    if( err )
        debug() << "Failed with error: " << err << endl;
}

void
UploadDialog::slotProcessFinished()
{
    setButtonText( User1, i18n("&Close") );

    disconnect( this, SIGNAL( user1Clicked() ), this, SLOT( slotProcessStop() ) );
       connect( this, SIGNAL( user1Clicked() ), this, SLOT( slotOk() ) );
}

void
UploadDialog::slotImageSelected( QListViewItem *item )
{
    if( !item || m_imageList->childCount() == 0 )
    {
        m_imageLabel->clear();
        return;
    }

    ImageListItem *pitem = static_cast<ImageListItem*>( item );
    if ( !pitem ) return;

    m_imageLabel->clear();

    QString IdemIndexed = "file:" + pitem->pathSrc();

    KURL url( IdemIndexed );

    KIO::PreviewJob* m_thumbJob = KIO::filePreview( url, m_imageLabel->height() );

    connect( m_thumbJob, SIGNAL( gotPreview(const KFileItem*, const QPixmap&) ),
                   this,   SLOT( slotGotPreview(const KFileItem*, const QPixmap&) ) );
}

void
UploadDialog::slotGotPreview(const KFileItem* url, const QPixmap &pixmap)
{
    QPixmap pix( pixmap );

    // Rotate the thumbnail compared to the angle the host application dictate
    KIPI::ImageInfo info = m_interface->info( url->url() );
    if ( info.angle() != 0 )
    {
        QImage img = pix.convertToImage();
        QWMatrix matrix;

        matrix.rotate( info.angle() );
        img = img.xForm( matrix );
        pix.convertFromImage( img );
    }

    m_imageLabel->setPixmap(pix);
}

void
UploadDialog::slotImagesFilesButtonAdd()
{
    QStringList fileList;

    KURL::List urls = KIPI::ImageDialog::getImageURLs( this, m_interface );

    for( KURL::List::Iterator it = urls.begin() ; it != urls.end() ; ++it )
        fileList << (*it).path();

    if ( urls.isEmpty() ) return;

    slotAddDropItems( fileList );
}

void
UploadDialog::slotImagesFilesButtonRem()
{
    QPtrList<QListViewItem> selected = m_imageList->selectedItems();

    for( QListViewItem *it = selected.first(); it; it = selected.next() )
        delete it;
}


void
UploadDialog::slotAddDropItems(QStringList filesPath)
{
    if( filesPath.isEmpty() ) return;

    for( QStringList::Iterator it = filesPath.begin() ; it != filesPath.end() ; ++it )
    {
        QString currentDropFile = *it;

        // Check if the new item already exist in the list.

        bool itemExists = false;

        QListViewItemIterator it2( m_imageList );

        while( it2.current() )
        {
            ImageListItem *item = static_cast<ImageListItem*>(it2.current());

            if( item->pathSrc() == currentDropFile.section('/', 0, -1) )
            {
                itemExists = true;
                break;
            }
            ++it2;
        }

        if( !itemExists )
            addUrlToList( currentDropFile );
    }
}

void
UploadDialog::addUrlToList( QString &file )
{
    QFileInfo *fi = new QFileInfo( file );

    new ImageListItem( m_imageList, file.section('/', 0, -1), fi->fileName(), QString::null );

    delete fi;
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
