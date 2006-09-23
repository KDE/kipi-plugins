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

#include "imagelist.h"
#include "imagelistitem.h"
#include "ipodexportdialog.h"

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

#include <kdebug.h>
#include <kfileitem.h>
#include <kiconloader.h>
#include <kio/jobclasses.h>
#include <kio/global.h>
#include <kio/previewjob.h>
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
    }
    else //device opened! hooray!
    {
        /// Show iPod info stuff
        const Itdb_IpodInfo *ipodInfo = itdb_device_get_ipod_info( m_itdb->device );
        const gchar *modelString = 0;
        QString text;
        if( ipodInfo )
        {
            modelString = itdb_info_get_ipod_model_name_string( ipodInfo->ipod_model );
            text = i18n( "<p align=\"center\"><b>iPod %1 detected at: %2</b></p>")
                        .arg( modelString, mountPoint() );
        }
        else
            text = i18n( "<p align=\"center\"><b>iPod detected at: %1</b></p>")
                        .arg( mountPoint() );

        QLabel *info = new QLabel( text, box );

        info->setPaletteBackgroundColor( QColor(0,98,0) );
        info->setPaletteForegroundColor( Qt::white );
        info->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

        dvlay->addWidget( info );
    }


    QHGroupBox *destinationBox = new QHGroupBox( i18n("iPod"), box );

    m_ipodAlbumList = new KListView( destinationBox );
    m_ipodAlbumList->addColumn( i18n("Albums") );
    m_ipodAlbumList->setItemMargin( 3 );
    m_ipodAlbumList->setResizeMode( QListView::LastColumn );
    m_ipodAlbumList->setSelectionMode( QListView::Single );
    m_ipodAlbumList->setAllColumnsShowFocus( true );
    m_ipodAlbumList->setRootIsDecorated( true ); // show expand icons
    m_ipodAlbumList->setMinimumHeight( 130 );

    QWidget          *buttons = new QWidget( destinationBox );
    QVBoxLayout *buttonLayout = new QVBoxLayout( buttons, 0, spacingHint() );

    m_addAlbumButton = new QPushButton ( i18n( "&New..."), buttons, "addAlbumButton");
    m_remAlbumButton = new QPushButton ( i18n( "&Remove"), buttons, "remAlbumButton");
    QWhatsThis::add( m_addAlbumButton, i18n("Create a new photo album on the iPod."));
    QWhatsThis::add( m_remAlbumButton, i18n("Remove the selected photo albums from the iPod."));

    QLabel *icon = new QLabel( buttons );
    icon->setPixmap( KGlobal::iconLoader()->loadIcon( "ipod", KIcon::Desktop, KIcon::SizeEnormous ) );

    m_ipodPreview = new QLabel( buttons );
    m_ipodPreview->setFixedHeight( 80 );
    m_ipodPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_ipodPreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );

    buttonLayout->addWidget( m_addAlbumButton );
    buttonLayout->addWidget( m_remAlbumButton );
    buttonLayout->addWidget( m_ipodPreview );
    buttonLayout->addWidget( icon );
    buttonLayout->addStretch( 1 );

    dvlay->addWidget( destinationBox );

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

    m_imagePreview = new QLabel( urlBox );
    m_imagePreview->setFixedHeight( 80 );
    m_imagePreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_imagePreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    urlLayout_1->addWidget( m_imagePreview );
    QWhatsThis::add( m_imagePreview, i18n( "The preview of the selected image in the list." ) );
    urlLayout_1->addStretch( 1 );

    dvlay->addWidget( urlListBox );

    m_progress = new KProgress( box, "Progress" );
    m_progress->setTotalSteps(100);
    m_progress->setValue(0);
    QWhatsThis::add( m_progress, i18n("This is the current percentage of the task completed.") );

    dvlay->addWidget( m_progress );


    debug() << "Populating ipod view" << endl;
    /// populate the ipod view with a list of albums etc
    getIPodAlbums();

    /// add selected items to the ImageList
    KIPI::ImageCollection images = interface->currentSelection();

    if ( images.isValid() )
    {
        KURL::List selected = images.images();
        for( KURL::List::Iterator it = selected.begin(); it != selected.end(); ++it )
        {
            addUrlToList( (*it).path() );
        }
        enableButton( KDialogBase::User1, m_imageList->childCount() > 0 && m_itdb );
    }


    urlListBox->setEnabled( m_itdb );
    m_progress->setEnabled( m_itdb );

    /// connect the signals & slots

    connect( m_addAlbumButton, SIGNAL( clicked() ), SLOT( slotCreateIpodAlbum() ) );
    connect( m_remAlbumButton, SIGNAL( clicked() ), SLOT( slotDeleteIpodAlbum() ) );

    connect( this, SIGNAL( user1Clicked() ), SLOT( slotProcessStart() ) );

    connect( m_imageList, SIGNAL( addedDropItems(QStringList) ),
                    this,   SLOT( slotAddDropItems(QStringList) ) );

    connect( m_imageList, SIGNAL( currentChanged(QListViewItem*) ),
                    this,   SLOT( slotImageSelected(QListViewItem*) ) );

    connect( m_ipodAlbumList, SIGNAL( currentChanged(QListViewItem*) ),
                    this,   SLOT( slotIpodImageSelected(QListViewItem*) ) );

    connect( m_addImagesButton, SIGNAL( clicked() ),
                    this,         SLOT( slotImagesFilesButtonAdd() ) );

    connect( m_remImagesButton, SIGNAL( clicked() ),
                    this,         SLOT( slotImagesFilesButtonRem() ) );
}


void
UploadDialog::getIPodAlbums()
{
    if( !m_itdb )
        return;

    KListViewItem *last = 0;
    for( GList *it = m_itdb->photoalbums; it; it = it->next )
    {
        Itdb_PhotoAlbum *ipodAlbum = (Itdb_PhotoAlbum *)it->data;
        debug() << "found album: " << ipodAlbum->name << endl;
        last = new KListViewItem( m_ipodAlbumList, last, ipodAlbum->name );
        last->setPixmap( 0, KGlobal::iconLoader()->loadIcon( "folder", KIcon::Toolbar, KIcon::SizeSmall ) );
        getIPodAlbumPhotos( last, ipodAlbum );
    }
}

void
UploadDialog::getIPodAlbumPhotos( KListViewItem *item, Itdb_PhotoAlbum *album )
{
    if( !item || !album || !m_itdb )
        return;

    KListViewItem *last = 0;
    for( GList *it = album->members; it; it = it->next )
    {
        gint photo_id = GPOINTER_TO_INT( it->data );
        last = new KListViewItem( item, last, QString::number( photo_id ) );
        last->setPixmap( 0, KGlobal::iconLoader()->loadIcon( "image", KIcon::Toolbar, KIcon::SizeSmall ) );
    }
}

void
UploadDialog::slotProcessStart()
{
    if( !m_itdb || !m_imageList->childCount() )
        return;

    disconnect( this, SIGNAL( user1Clicked() ), this, SLOT( slotProcessStart() ) );
       connect( this, SIGNAL( user1Clicked() ), this, SLOT( slotProcessStop()  ) );

    showButtonCancel( false );
    setButtonText( User1, i18n("&Stop") );

    m_progress->setTotalSteps( m_imageList->childCount() + 1 ); // +1 for writing the database

    QString albumName = QString("test");

    for( QListViewItem *item = m_imageList->firstChild(); item; item = item->nextSibling() )
    {
    #define item static_cast<ImageListItem*>(item)
        debug() << "Uploading " << item->pathSrc().utf8() << " to ipod album " << albumName.utf8() << endl;

        itdb_photodb_add_photo( m_itdb, albumName.utf8(), item->pathSrc().utf8() );

        m_progress->advance( 1 );
    #undef  item
    }

    debug() << "Writing database" << endl;
    GError *err = 0;
    itdb_photodb_write( m_itdb, &err );

    debug() << "Finished writing database" << endl;
    if( err ) debug() << "Failed with error: " << err << endl;

    m_progress->advance( 1 );

    slotProcessFinished();
}

void
UploadDialog::slotProcessFinished()
{
    setButtonText( User1, i18n("&Close") );

    disconnect( this, SIGNAL( user1Clicked() ), this, SLOT( slotProcessStop() ) );
       connect( this, SIGNAL( user1Clicked() ), this, SLOT( slotOk() ) );
}

void
UploadDialog::slotIpodImageSelected( QListViewItem *item )
{
    m_ipodPreview->clear();

    if( !item || item->depth() != 1 )
        return;

    uint index = item->text(0).toInt();
    Itdb_Thumb *thumb = 0;

    for( GList *it = m_itdb->photos; it; it = it->next )
    {
        Itdb_Artwork *artwork = (Itdb_Artwork *)it->data;
        if( artwork->id == index )
        {
            thumb = itdb_artwork_get_thumb_by_type( artwork, ITDB_THUMB_PHOTO_SMALL );
            break;
        }
    }

    if( !thumb )
    {
        debug() << "No thumb was found" << endl;
        return;
    }

    debug() << "Thumb found with length: " << thumb->image_data_len << endl;

    QPixmap pix;
    pix.loadFromData( thumb->image_data, thumb->image_data_len );
    m_ipodPreview->setPixmap( pix );
}

void
UploadDialog::slotImageSelected( QListViewItem *item )
{
    if( !item || m_imageList->childCount() == 0 )
    {
        m_imagePreview->clear();
        return;
    }

    ImageListItem *pitem = static_cast<ImageListItem*>( item );
    if ( !pitem ) return;

    m_imagePreview->clear();

    QString IdemIndexed = "file:" + pitem->pathSrc();

    KURL url( IdemIndexed );

    KIO::PreviewJob* m_thumbJob = KIO::filePreview( url, m_imagePreview->height() );

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

    m_imagePreview->setPixmap(pix);
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

    enableButton( KDialogBase::User1, m_imageList->childCount() > 0 );
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

    enableButton( KDialogBase::User1, m_imageList->childCount() > 0 );
}

void
UploadDialog::addUrlToList( QString file )
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
        if( m_itdb )
        {
            itdb_photodb_free( m_itdb );
            m_itdb = 0;
        }
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
