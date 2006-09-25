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
#include <kinputdialog.h> //new album
#include <kio/previewjob.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmountpoint.h>
#include <kpopupmenu.h>
#include <kprogress.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include <libkipi/imagedialog.h>

#define debug() kdDebug( 51000 )

using namespace IpodExport;

UploadDialog::UploadDialog( KIPI::Interface* interface, QString caption, QWidget *parent )
    : KDialogBase( KDialogBase::Plain, caption, Help|User1|Close,
                   Cancel, parent, "SimpleUploadDialog", false, false, i18n("&Start"))
    , m_interface( interface )
    , m_itdb( 0 )
    , m_transferring( false )
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

    m_ipodAlbumList = new ImageList( ImageList::IpodType, destinationBox );
    m_ipodAlbumList->setMinimumHeight( 100 );

    QWidget          *buttons = new QWidget( destinationBox );
    QVBoxLayout *buttonLayout = new QVBoxLayout( buttons, 0, spacingHint() );

    m_createAlbumButton = new QPushButton( i18n("&New..."), buttons, "addAlbumButton");
    QWhatsThis::add( m_createAlbumButton, i18n("Create a new photo album on the iPod."));

#ifdef HAVE_ITDB_REMOVE_PHOTOS
    m_removeAlbumButton = new QPushButton( i18n("&Remove"), buttons, "remAlbumButton");
    m_renameAlbumButton = new QPushButton( i18n("R&ename..."), buttons, "renameAlbumsButton");

    m_removeAlbumButton->setEnabled( false );
    m_renameAlbumButton->setEnabled( false );

    QWhatsThis::add( m_removeAlbumButton, i18n("Remove the selected photos or albums from the iPod."));
    QWhatsThis::add( m_renameAlbumButton, i18n("Rename the selected photo album on the iPod."));
#endif

    QLabel *ipod_icon = new QLabel( buttons );
    ipod_icon->setPixmap( KGlobal::iconLoader()->loadIcon( "ipod", KIcon::Desktop, KIcon::SizeEnormous ) );

    m_ipodPreview = new QLabel( buttons );
    m_ipodPreview->setFixedHeight( 80 );
    m_ipodPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_ipodPreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );

    buttonLayout->addWidget( m_createAlbumButton );
#ifdef HAVE_ITDB_REMOVE_PHOTOS
    buttonLayout->addWidget( m_removeAlbumButton );
    buttonLayout->addWidget( m_renameAlbumButton );
#endif
    buttonLayout->addWidget( m_ipodPreview );
    buttonLayout->addWidget( ipod_icon );
    buttonLayout->addStretch( 1 );

    dvlay->addWidget( destinationBox );

    QHGroupBox *urlListBox = new QHGroupBox( i18n("Hard Disk"), box );
    QWidget* urlBox = new QWidget( urlListBox );
    QHBoxLayout* urlLayout = new QHBoxLayout( urlBox, 0, spacingHint() );
    m_imageList = new ImageList( ImageList::UploadType, urlBox );
    m_imageList->setMinimumHeight( 100 );
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
    QWhatsThis::add( m_imagePreview, i18n( "The preview of the selected image in the list." ) );

    QLabel *hdd_icon = new QLabel( urlBox );
    hdd_icon->setPixmap( KGlobal::iconLoader()->loadIcon( "hdd_unmount", KIcon::Desktop, KIcon::SizeEnormous ) );

    urlLayout_1->addWidget( m_imagePreview );
    urlLayout_1->addWidget( hdd_icon );
    urlLayout_1->addStretch( 1 );

    dvlay->addWidget( urlListBox );

    m_progress = new KProgress( box, "Progress" );
    m_progress->setTotalSteps(100);
    m_progress->setValue(0);
    QWhatsThis::add( m_progress, i18n("This is the current percentage of the task completed.") );

    dvlay->addWidget( m_progress );


    debug() << "Populating ipod view" << endl;
    /// populate the ipod view with a list of albums etc
    getIpodAlbums();

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

    enableButtons();

    destinationBox->setEnabled( m_itdb );
    urlListBox->setEnabled( m_itdb );
    m_progress->setEnabled( m_itdb );

    /// connect the signals & slots

    connect( m_createAlbumButton, SIGNAL( clicked() ), SLOT( slotCreateIpodAlbum() ) );

#ifdef HAVE_ITDB_REMOVE_PHOTOS
    connect( m_removeAlbumButton,    SIGNAL( clicked() ), SLOT( slotDeleteIpodAlbum() ) );
    connect( m_renameAlbumButton, SIGNAL( clicked() ), SLOT( slotRenameIpodAlbum() ) );
#endif

    connect( this, SIGNAL( user1Clicked() ), SLOT( slotProcessStart() ) );

    connect( m_imageList, SIGNAL( addedDropItems(QStringList) ),
                    this,   SLOT( slotAddDropItems(QStringList) ) );

    connect( m_imageList, SIGNAL( currentChanged(QListViewItem*) ),
                    this,   SLOT( slotImageSelected(QListViewItem*) ) );

    connect( m_ipodAlbumList, SIGNAL( currentChanged(QListViewItem*) ),
                    this,   SLOT( slotIpodItemSelected(QListViewItem*) ) );

    connect( m_addImagesButton, SIGNAL( clicked() ),
                    this,         SLOT( slotImagesFilesButtonAdd() ) );

    connect( m_remImagesButton, SIGNAL( clicked() ),
                    this,         SLOT( slotImagesFilesButtonRem() ) );
}


void
UploadDialog::getIpodAlbums()
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
        getIpodAlbumPhotos( last, ipodAlbum );
    }
}

void
UploadDialog::getIpodAlbumPhotos( KListViewItem *item, Itdb_PhotoAlbum *album )
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
UploadDialog::reloadIpodAlbum( const QString &album )
{
    QListViewItem *item = 0;
    for( item = m_ipodAlbumList->firstChild(); item; item = item->nextSibling() )
    {
        if( item->text(0) == album )
            break;
    }

    if( !item ) return;

    while( item->firstChild() )
        delete item->firstChild(); // clear the items, so we can reload them again

    Itdb_PhotoAlbum *ipodAlbum = 0;
    for( GList *it = m_itdb->photoalbums; it; it = it->next )
    {
        ipodAlbum = (Itdb_PhotoAlbum *)it->data;
        if( strcmp( ipodAlbum->name, album.utf8() ) == 0 )
            break; // we found the album
    }

    getIpodAlbumPhotos( static_cast<KListViewItem*>(item), ipodAlbum );
}

void
UploadDialog::enableButtons()
{
    // enable the start button only if there are albums to transfer to, items to transfer
    // and a database to add to!
    enableButton( KDialogBase::User1, m_imageList->childCount()     > 0 &&
                                      m_ipodAlbumList->childCount() > 0 &&
                                      !m_transferring                   &&
                                      m_imageList->selectedItem()       &&
                                      !m_imageList->selectedItem()->depth() &&
                                      m_itdb );
    enableButton( KDialogBase::Close, !m_transferring );
}

void
UploadDialog::slotProcessStart()
{
    if( !m_itdb || !m_imageList->childCount() )
        return;

    QListViewItem *selected = m_ipodAlbumList->selectedItem();
    if( !selected || selected->depth() != 0 /*not album*/)
        return;

    m_transferring = true;

    QString albumName = selected->text( 0 );

    enableButton( KDialogBase::User1, false );
    enableButton( KDialogBase::Close, false );

    m_progress->setTotalSteps( m_imageList->childCount() + 1 ); // +1 for writing the database

    while( QListViewItem *item = m_imageList->firstChild() )
    {
    #define item static_cast<ImageListItem*>(item)
        debug() << "Uploading " << item->pathSrc().utf8() << " to ipod album " << albumName.utf8() << endl;
        itdb_photodb_add_photo( m_itdb, albumName.utf8(), item->pathSrc().utf8() );
        m_progress->advance( 1 );
        delete item;
    #undef  item
    }

    debug() << "Writing database" << endl;
    GError *err = 0;
    itdb_photodb_write( m_itdb, &err );

    debug() << "Finished writing database" << endl;
    if( err ) debug() << "Failed with error: " << err << endl;

    m_progress->advance( 1 );

    reloadIpodAlbum( albumName );

    m_transferring = false;

    enableButtons();
}

void
UploadDialog::slotIpodItemSelected( QListViewItem *item )
{
    m_ipodPreview->clear();

    // only let the user transfer to directories which are selected
    enableButton( KDialogBase::User1, item && item->depth() == 0 );

#ifdef HAVE_ITDB_REMOVE_PHOTOS
    m_removeAlbumButton->setEnabled( item );
    m_renameAlbumButton->setEnabled( item && item->depth() == 0 );
#endif

    if( m_ipodAlbumList->currentItem() )
        m_ipodAlbumList->currentItem()->setSelected( true );

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

//     uchar *data = itdb_thumb_get_rgb_data( m_itdb->device, thumb );

//     QPixmap pix( thumb->width, thumb->height );
//     pix.loadFromData( data,  );
//     m_ipodPreview->setPixmap( pix );
}

void
UploadDialog::slotImageSelected( QListViewItem *item )
{
    if( !item || m_imageList->childCount() == 0 || m_transferring)
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
UploadDialog::slotCreateIpodAlbum()
{
    QString helper;
    KIPI::ImageCollection album = m_interface->currentAlbum();
    if( album.isValid() )
        helper = album.name();

    bool ok = false;
    QString newAlbum = KInputDialog::getText( i18n("New iPod Photo Album"),
                                              i18n("Create a new album:"),
                                              helper, &ok, this );
    if( ok )
    {
        itdb_photodb_photoalbum_new( m_itdb, newAlbum.utf8() );
        // add the new album to the list view
        KListViewItem *i = new KListViewItem( m_ipodAlbumList, 0, newAlbum );
        i->setPixmap( 0, KGlobal::iconLoader()->loadIcon( "folder", KIcon::Toolbar, KIcon::SizeSmall ) );
        // commit the changes to the iPod
        GError *err = 0;
        itdb_photodb_write( m_itdb, &err );
    }
}

void
UploadDialog::slotRenameIpodAlbum()
{
#ifdef HAVE_ITDB_REMOVE_PHOTOS
    QListViewItem *selected = m_ipodAlbumList->selectedItem();

    // only allow renaming of album items
    if( !selected || selected->depth() != 0 ) return;

    QString oldName = selected->text(0);
    bool ok = false;
    QString newName = KInputDialog::getText( i18n("Rename iPod Photo Album"),
                                             i18n("New album title:"),
                                             i18n("Album"), &ok, this );
    if( ok )
    {
        // change the name on the ipod, and rename the listviewitem
        itdb_photodb_rename_photoalbum( m_itdb, oldName.utf8(), newName.utf8() );
        selected->setText( 0, newName );
        // commit changes to the iPod
        GError *err = 0;
        itdb_photodb_write( m_itdb, &err );
    }
#endif
}

void
UploadDialog::slotDeleteIpodAlbum()
{
#ifdef HAVE_ITDB_REMOVE_PHOTOS
    QListViewItem *selected = m_ipodAlbumList->selectedItem();
    if( !selected ) return;

    GError *err = 0;

    QString text = selected->text(0);

    switch( selected->depth() )
    {
        case 0: //album
            debug() << "Deleting album: " << text << endl;
            if( itdb_photodb_remove_photoalbum( m_itdb, text.utf8() ) )
                delete selected;
            else
                debug() << "Error deleting album, oh no!" << endl;
            break;

        case 1: //image
            debug() << "Deleting image with id: " << text << endl;
            if( itdb_photodb_remove_photo( m_itdb, text.toInt() ) )
                delete selected; // remove the item from the listview if there was no error.
            else
                debug() << "Error deleting image, oh no!" << endl;

            itdb_photodb_write( m_itdb, &err );
    }
#endif
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

        /// Detecting an ipod. Since the user may never have created a photodb, or have deleted the
        /// Photo directory, we must try to find a better way to determine if an ipod really exists
        /// at this mount point. So, check if the iPod_Control directory exists.
        GError *err = 0;
        Itdb_iTunesDB *db = itdb_parse( QFile::encodeName( mountpoint ), &err );
        if( err )
        {
            debug() << "could not parse itdb at " << mountpoint << endl;
            g_error_free( err );
            if( db )
            {
                itdb_free( db );
                db = 0;
            }
            continue;
        }

        if( mountPoint().isEmpty() )
            m_mountPoint = mountpoint;

        ipodFound = true;
        m_itdb = itdb_photodb_parse( QFile::encodeName( mountpoint ), &err );
        if( err )
        {
            g_error_free( err );
            if( m_itdb )
            {
                itdb_photodb_free( m_itdb );
                m_itdb = 0;
            }
        }
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
        debug() << "could not find iTunesDB on device mounted at " << mountPoint() << endl;

        QString msg = i18n( "An iPod photo database could not be found on device mounted at %1. "
                "Should I try to initialize your iPod photo database?" ).arg( mountPoint() );

        if( KMessageBox::warningContinueCancel( this, msg, i18n( "Initialize iPod Photo Database?" ),
                    KGuiItem(i18n("&Initialize"), "new") ) == KMessageBox::Continue )
        {

            m_itdb = itdb_photodb_new();
            itdb_device_set_mountpoint( m_itdb->device, mountPoint().utf8() );

            if( !m_itdb )
            {
                debug() << "Could not initialise photodb..." << endl;
                return false;
            }
        }
        else
            return false;
    }

    return true;
}
