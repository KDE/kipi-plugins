/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz@kde.org>
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

// Local includes.

#include "IpodHeader.h"
#include "ImageList.h"
#include "ImageListItem.h"
#include "IpodExportDialog.h"
#include "IpodListItem.h"

// Qt includes.

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGroupBox>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QMatrix>
#include <QPixmap>
#include <QPushButton>
#include <QTreeWidgetItem>

// KDE includes

#include <kdebug.h>
#include <kfileitem.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <kio/previewjob.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmountpoint.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kpushbutton.h>
#include <kmenu.h>
#include <khelpmenu.h>
#include <ktoolinvocation.h>
#include <kguiitem.h>

namespace KIPIIpodExportPlugin
{

UploadDialog *UploadDialog::s_instance = 0;

UploadDialog::UploadDialog(
#if KIPI_PLUGIN
    KIPI::Interface* interface,
#endif
    const QString &caption, QWidget *parent )
    : KDialog( parent)
#if KIPI_PLUGIN
    , m_interface( interface )
#endif
    , m_itdb( 0 )
    , m_ipodInfo( 0 )
    , m_ipodHeader( 0 )
    , m_transferring( false )
    , m_mountPoint( QString::null )
    , m_deviceNode( QString::null )
    , m_ipodAlbumList( 0 )
{
    s_instance = this;

    QWidget *box = new QWidget();
    setMainWidget(box);
    setCaption(caption);
    setButtons(Close|Help);
    setModal(false);

    // ---------------------------------------------------------------
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("iPod Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A tool to export image to an iPod device"),
                                           ki18n("(c) 2006-2008, Seb Ruiz"));

    m_about->addAuthor(ki18n("Seb Ruiz"), ki18n("Author and Maintainer"),
                       "ruiz@kde.org");

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                       "caulier dot gilles at gmail dot com");

    disconnect( this, SIGNAL( helpClicked() ), this, SLOT( slotHelp() ) );

    KHelpMenu* helpMenu = new KHelpMenu( this, m_about, false );

    QAction *handbook = new QAction( i18n("Plugin Handbook"), this );
    connect( handbook, SIGNAL( triggered(bool) ), this, SLOT( slotHelp() ) );
    
    helpMenu->menu()->removeAction( helpMenu->menu()->actions().first() );
    helpMenu->menu()->insertAction( helpMenu->menu()->actions().first(), handbook );

    button( Help )->setDelayedMenu( helpMenu->menu() );

    // ------------------------------------------------------------

    QGridLayout *grid = new QGridLayout(box);
    m_ipodHeader      = new IpodHeader(box);

    m_destinationBox = new QGroupBox( i18n("iPod"), box );
    m_ipodAlbumList  = new ImageList( ImageList::IpodType, m_destinationBox );
    m_ipodAlbumList->setMinimumHeight( 80 );

    QWidget          *buttons = new QWidget( m_destinationBox );
    QVBoxLayout *buttonLayout = new QVBoxLayout( buttons );

    m_createAlbumButton = new QPushButton( i18n("&New..."), buttons );
    m_createAlbumButton->setWhatsThis( i18n( "Create a new photo album on the iPod." ) );

    m_removeAlbumButton = new QPushButton( i18n("&Remove"), buttons );
    m_renameAlbumButton = new QPushButton( i18n("R&ename..."), buttons );

    m_removeAlbumButton->setEnabled( false );
    m_renameAlbumButton->setEnabled( false );

    m_removeAlbumButton->setWhatsThis( i18n("Remove the selected photos or albums from the iPod.") );
    m_renameAlbumButton->setWhatsThis( i18n("Rename the selected photo album on the iPod.") );

    QLabel *ipod_icon = new QLabel( buttons );
    ipod_icon->setPixmap(KIconLoader::global()->loadIcon( "multimedia-player-apple-ipod",
                                                         KIconLoader::Desktop,
                                                         KIconLoader::SizeHuge ) );

    m_ipodPreview = new QLabel( buttons );
    m_ipodPreview->setFixedHeight( 80 );
    m_ipodPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_ipodPreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );

    buttonLayout->addWidget( m_createAlbumButton );
    buttonLayout->addWidget( m_removeAlbumButton );
    buttonLayout->addWidget( m_renameAlbumButton );
    buttonLayout->addWidget( m_ipodPreview );
    buttonLayout->addStretch( 1 );
    buttonLayout->addWidget( ipod_icon );
    buttonLayout->setMargin( 0 );
    buttonLayout->setMargin( spacingHint() );

    m_urlListBox           = new QGroupBox( i18n("Hard Disk"), box );
    QWidget* urlBox        = new QWidget( m_urlListBox );
    QHBoxLayout* urlLayout = new QHBoxLayout( urlBox );
    m_uploadList           = new ImageList( ImageList::UploadType, urlBox );
    m_uploadList->setMinimumHeight( 80 );

    m_uploadList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

    QVBoxLayout* uploadPaneLayout = new QVBoxLayout();
    m_addImagesButton             = new QPushButton( i18n("&Add..."), urlBox );
    m_addImagesButton->setWhatsThis(i18n("Add images to be queued for the iPod."));

    urlLayout->addWidget( m_uploadList );
    urlLayout->addLayout( uploadPaneLayout );
    urlLayout->setMargin( 0 );
    urlLayout->setMargin( 0 );

    m_remImagesButton = new QPushButton( i18n( "&Remove" ), urlBox );
    m_remImagesButton->setWhatsThis( i18n("Remove selected image from the list.") );

    m_transferImagesButton = new QPushButton( i18n( "&Transfer" ), urlBox );
    m_transferImagesButton->setWhatsThis( i18n("Transfer images to the selected iPod album.") );

    m_imagePreview = new QLabel( urlBox );
    m_imagePreview->setFixedHeight( 80 );
    m_imagePreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_imagePreview->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred) );
    m_imagePreview->setWhatsThis( i18n("The preview of the selected image in the list.") );

    QLabel *hdd_icon = new QLabel( urlBox );
    hdd_icon->setPixmap(KIconLoader::global()->loadIcon("computer",
                                                        KIconLoader::Desktop,
                                                        KIconLoader::SizeHuge));

    uploadPaneLayout->addWidget( m_addImagesButton );
    uploadPaneLayout->addWidget( m_remImagesButton );
    uploadPaneLayout->addWidget( m_transferImagesButton );
    uploadPaneLayout->addWidget( m_imagePreview );
    uploadPaneLayout->addStretch( 1 );
    uploadPaneLayout->addWidget( hdd_icon );
    uploadPaneLayout->setSpacing( spacingHint() );
    uploadPaneLayout->setMargin( 0 );

    grid->addWidget( m_ipodHeader,     0, 0, 1, 2 );
    grid->addWidget( m_urlListBox,     1, 0, 1, 1 );
    grid->addWidget( m_destinationBox, 1, 1, 1, 1 );
    grid->setSpacing( spacingHint() );
    grid->setMargin( 0 );

    /// populate the ipod view with a list of albums etc
    refreshDevices();

#if KIPI_PLUGIN
    /// add selected items to the ImageList
    KIPI::ImageCollection images = interface->currentSelection();

    if( images.isValid() )
    {
        KUrl::List selected = images.images();
        for( KUrl::List::Iterator it = selected.begin(); it != selected.end(); ++it )
        {
            addUrlToList( (*it).path() );
        }
    }
#endif

    enableButtons();

    /// connect the signals & slots
#if 0
    connect(m_createAlbumButton, SIGNAL( clicked() ),
            this, SLOT( createIpodAlbum() ));

    connect(m_removeAlbumButton, SIGNAL( clicked() ),
            this, SLOT( deleteIpodAlbum() ));

    connect(m_renameAlbumButton, SIGNAL( clicked() ),
            this, SLOT( renameIpodAlbum() ));

    connect(m_uploadList, SIGNAL( addedDropItems(QStringList) ),
            this, SLOT( addDropItems(QStringList) ));

    connect(m_uploadList, SIGNAL( currentChanged(Q3ListViewItem*) ),
            this, SLOT( imageSelected(Q3ListViewItem*) ));

    connect(m_ipodAlbumList, SIGNAL( currentChanged(Q3ListViewItem*) ),
            this, SLOT( ipodItemSelected(Q3ListViewItem*) ));

    connect(m_addImagesButton, SIGNAL( clicked() ),
            this, SLOT( imagesFilesButtonAdd() ));

    connect(m_remImagesButton, SIGNAL( clicked() ),
            this, SLOT( imagesFilesButtonRem() ));

    connect(m_transferImagesButton, SIGNAL( clicked() ),
            this, SLOT( startTransfer() ));
#endif
}

UploadDialog::~UploadDialog()
{
    if( m_itdb )
        itdb_photodb_free( m_itdb );

    delete m_about;
}

void UploadDialog::slotHelp()
{
    KToolInvocation::invokeHelp("ipodexport", "kipi-plugins");
}

void UploadDialog::getIpodAlbums()
{
    if( !m_itdb )
        return;

    kDebug(51000) << "populating ipod view" << endl;

    // clear cache
    m_ipodAlbumList->clear();

    for( GList *it = m_itdb->photoalbums; it; it = it->next )
    {
        Itdb_PhotoAlbum *ipodAlbum = (Itdb_PhotoAlbum *) it->data;

        kDebug(51000) << " found album: " << ipodAlbum->name << endl;

        IpodAlbumItem *albumItem = new IpodAlbumItem( m_ipodAlbumList, ipodAlbum );

        m_ipodAlbumList->addTopLevelItem( albumItem );

        albumItem->setIcon( 0, KIcon("folder") );
        getIpodAlbumPhotos( albumItem, ipodAlbum );
    }
}

void UploadDialog::getIpodAlbumPhotos(IpodAlbumItem *item, Itdb_PhotoAlbum *album)
{
    if( !item || !album || !m_itdb )
        return;

    IpodPhotoItem *last = 0;
    for( GList *it = album->members; it; it = it->next )
    {
        Itdb_Artwork *photo = (Itdb_Artwork*) it->data;
        gint photo_id = photo->id;
        last = new IpodPhotoItem( item, last, photo );
        last->setText( 0, QString::number( photo_id ) );
        last->setIcon( 0, KIcon("image-jp2") );
    }
}

void UploadDialog::reloadIpodAlbum( IpodAlbumItem *item, Itdb_PhotoAlbum *album )
{
    if( !item ) return;

    while( item->child( 0 ) )
        delete item->child( 0 ); // clear the items, so we can reload them again

    Itdb_PhotoAlbum *ipodAlbum = 0;
    for( GList *it = m_itdb->photoalbums; it; it = it->next )
    {
        ipodAlbum = (Itdb_PhotoAlbum *)it->data;
        if( strcmp( ipodAlbum->name, album->name ) == 0 )
            break; // we found the album
    }

    dynamic_cast<IpodAlbumItem*>(item)->setPhotoAlbum( ipodAlbum );

    getIpodAlbumPhotos( item, ipodAlbum );
}

void UploadDialog::enableButtons()
{
#if 0
    // enable the start button only if there are albums to transfer to, items to transfer
    // and a database to add to!
    const bool transfer = m_uploadList->childCount()     > 0 && // we have items to transfer
                          m_ipodAlbumList->childCount() > 0  && // the ipod has albums
                          !m_transferring                    && // we aren't transferring
                          m_ipodAlbumList->selectedItem()    && // selected a destination album
                          m_itdb;

    m_transferImagesButton->setEnabled(transfer);

    enableButton(KDialog::Close, !m_transferring);

    const Q3ListViewItem *ipodSelection = m_ipodAlbumList->selectedItem();
    const bool isMasterLibrary = (ipodSelection == m_ipodAlbumList->firstChild());

    m_removeAlbumButton->setEnabled(ipodSelection && !isMasterLibrary);
    m_renameAlbumButton->setEnabled(ipodSelection && !isMasterLibrary && ipodSelection->depth() == 0);
#endif
}

void UploadDialog::startTransfer()
{
#if 0
    if( !m_itdb || !m_uploadList->childCount() )
        return;

    Q3ListViewItem *selected = m_ipodAlbumList->selectedItem();
    if( !selected || selected->depth() != 0 /*not album*/)
        return;

    m_transferring = true;

#define selected static_cast<IpodAlbumItem*>( selected )

    Itdb_PhotoAlbum *album = selected->photoAlbum();

    enableButton(KDialog::User1, false);
    enableButton(KDialog::Close, false);

    GError *err = 0;

    while( Q3ListViewItem *item = m_uploadList->firstChild() )
    {
#define item static_cast<ImageListItem*>(item)
        kDebug(51000) << "Uploading "      << item->pathSrc()
                      << " to ipod album " << album->name << endl;

        Itdb_Artwork *art = itdb_photodb_add_photo( m_itdb, QFile::encodeName( item->pathSrc() ), 0, 0, &err );
        if( !art )
        {
            if( err )
            {
                kDebug(51000) << "Error adding photo " << item->pathSrc() << " to database:"
                              << err->message << endl;
                err = 0;
            }
        }
        else
            itdb_photodb_photoalbum_add_photo( m_itdb, album, art, 0 );

        delete item;
#undef item
    }

    itdb_photodb_write( m_itdb, &err );
    if( err ) kDebug(51000) << "Failed with error: " << err->message << endl;

    reloadIpodAlbum( selected, album );

    IpodAlbumItem *library = static_cast<IpodAlbumItem*>( m_ipodAlbumList->firstChild() );
    reloadIpodAlbum( library, library->photoAlbum() );

    m_transferring = false;

    enableButtons();
#undef selected
#endif
}

void UploadDialog::ipodItemSelected( QTreeWidgetItem *item )
{
    m_ipodPreview->clear();

    if( m_ipodAlbumList->currentItem() )
        m_ipodAlbumList->currentItem()->setSelected( true );

    enableButtons();

#define item dynamic_cast<IpodPhotoItem*>(item)
    if( !item )
        return;

    Itdb_Artwork *artwork = item->artwork();
    Itdb_Thumb *thumb = itdb_artwork_get_thumb_by_type( artwork, ITDB_THUMB_PHOTO_SMALL );

    if( !thumb )
    {
        kDebug(51000) << "no thumb was found" << endl;
        return;
    }
#undef item

//     GdkPixbuf *buf = itdb_thumb_get_gdk_pixbuf( m_itdb->device, thumb );
//     int size = 0;
//     QImage *image = buf->convertToImage();
//     kDebug(51000) << "image size: " << image->size() << endl;
//
//     QPixmap pix;
//     pix.convertFromImage( image );
//     m_ipodPreview->setPixmap( pix );
}

void UploadDialog::imageSelected( QTreeWidgetItem *item )
{
    if( !item || m_transferring )
    {
        m_imagePreview->clear();
        return;
    }

    ImageListItem *pitem = static_cast<ImageListItem*>( item );
    if ( !pitem ) return;

    m_imagePreview->clear();

    QString IdemIndexed = "file:" + pitem->pathSrc();

    KUrl url( IdemIndexed );

    KIO::PreviewJob* m_thumbJob = KIO::filePreview( url, m_imagePreview->height() );

    connect( m_thumbJob, SIGNAL( gotPreview(const KFileItem*, const QPixmap&) ),
                   this,   SLOT( gotImagePreview(const KFileItem*, const QPixmap&) ) );
}

void UploadDialog::gotImagePreview( const KFileItem* url, const QPixmap& pixmap )
{
#if KIPI_PLUGIN
    QPixmap pix( pixmap );

    // Rotate the thumbnail compared to the angle the host application dictate
    KIPI::ImageInfo info = m_interface->info( url->url() );
    if ( info.angle() != 0 )
    {
        QImage img = pix.toImage();
        QMatrix matrix;

        matrix.rotate( info.angle() );
        img = img.transformed( matrix );
        pix.fromImage( img );
    }

    m_imagePreview->setPixmap(pix);
#else
    Q_UNUSED( url );
    m_imagePreview->setPixmap( pixmap );
#endif
}

void UploadDialog::imagesFilesButtonAdd()
{
    QStringList fileList;
    KUrl::List urls;

#if KIPI_PLUGIN
    urls = KIPIPlugins::ImageDialog::getImageURLs(this, m_interface);
#else
    const QString filter = QString( "*.jpg *.jpeg *.jpe *.tiff *.gif *.png *.bmp|" + i18n("Image files") );
    KFileDialog dlg( QString::null, filter, this, "addImagesDlg", true );
    dlg.setCaption( i18n("Add Images") );
    dlg.setMode( KFile::Files | KFile::Directory );
    dlg.exec();
    urls = dlg.selectedURLs();
#endif

    for( KUrl::List::Iterator it = urls.begin() ; it != urls.end() ; ++it )
        fileList << (*it).path();

    if ( urls.isEmpty() ) return;

    addDropItems( fileList );
}

void UploadDialog::imagesFilesButtonRem()
{
#if 0
    QList<Q3ListViewItem*> selected = m_uploadList->selectedItems();

    Q3ListViewItem *it;
    foreach(it, selected)
        delete it;

    enableButton(KDialog::User1, m_uploadList->childCount() > 0);
#endif
}

void UploadDialog::createIpodAlbum()
{
#if 0
    QString helper;

#if KIPI_PLUGIN
    KIPI::ImageCollection album = m_interface->currentAlbum();
    if( album.isValid() )
        helper = album.name();
#endif

    bool ok = false;
    QString newAlbum = KInputDialog::getText( i18n("New iPod Photo Album"),
                                              i18n("Create a new album:"),
                                              helper, &ok, this );
    if( ok )
    {
        kDebug(51000) << "creating album " << newAlbum << endl;

        IpodAlbumItem *last = static_cast<IpodAlbumItem*>(m_ipodAlbumList->lastItem()); // FIXME?? O(n)

        Itdb_PhotoAlbum *photoAlbum = itdb_photodb_photoalbum_create( m_itdb, QFile::encodeName( newAlbum ), -1/*end*/ );
        // add the new album to the list view
        IpodAlbumItem *i = new IpodAlbumItem( m_ipodAlbumList, last, photoAlbum );
        i->setPixmap( 0, KIconLoader::global()->loadIcon( "folder", KIconLoader::Toolbar, KIconLoader::SizeSmall ) );
        m_ipodAlbumList->clearSelection();
        m_ipodAlbumList->setSelected( i, true );

        // commit the changes to the iPod
        GError *err = 0;
        itdb_photodb_write( m_itdb, &err );
    }
#endif
}

void UploadDialog::renameIpodAlbum()
{
#if 0
    IpodAlbumItem *selected = dynamic_cast<IpodAlbumItem*>(m_ipodAlbumList->selectedItem());

    // only allow renaming of album items
    if( !selected || selected->depth() != 0 ) return;

    bool ok = false;
    QString newName = KInputDialog::getText( i18n("Rename iPod Photo Album"),
                                             i18n("New album title:"),
                                             selected->text(0), &ok, this );
    if( ok )
    {
        // change the name on the ipod, and rename the listviewitem
        selected->setName( newName );
        // commit changes to the iPod
        GError *err = 0;
        itdb_photodb_write( m_itdb, &err );
    }
#endif
}

bool UploadDialog::deleteIpodPhoto( IpodPhotoItem *photo )
{
#if 0
    if( !photo )
        return false;

    IpodAlbumItem *album = static_cast<IpodAlbumItem *>( photo->parent() );

    if( !album )
        return false;

    Itdb_Artwork *artwork = photo->artwork();

    if( !artwork )
    {
        kDebug(51000) << "Could not find photo artwork with id: " << photo->text(0) << endl;
        return false;
    }

    Itdb_PhotoAlbum *photo_album = album->photoAlbum();
    itdb_photodb_remove_photo( m_itdb, photo_album, artwork );

    // if we remove from the library, remove from all sub albums too
    if( photo_album->album_type == 0x01 ) // master album
    {
        for( Q3ListViewItem *albumIt = m_ipodAlbumList->firstChild()->nextSibling(); //skip library
             albumIt; albumIt = albumIt->nextSibling() )
        {
            for( Q3ListViewItem *photoIt = albumIt->firstChild();
                 photoIt; photoIt = photoIt->nextSibling() )
            {
                if( photoIt->text(0) == photo->text(0) )
                {
                    kDebug(51000) << "removing reference to photo from album " << albumIt->text(0) << endl;
                    delete photoIt;
                }
            }
        }
    }
#else
    Q_UNUSED(photo);
#endif
    return true;
}

bool UploadDialog::deleteIpodAlbum( IpodAlbumItem *album )
{
    kDebug(51000) << "deleting album: " << album->name() << ", and removing all photos" << endl;
    itdb_photodb_photoalbum_remove( m_itdb, album->photoAlbum(), true/*remove photos*/);

    return true;
}

void UploadDialog::deleteIpodAlbum()
{
#if 0
    Q3ListViewItem *selected = m_ipodAlbumList->selectedItem();
    if( !selected ) return;

    bool result = false;
    switch( selected->depth() )
    {
        case 0: //album
            result = deleteIpodAlbum( dynamic_cast<IpodAlbumItem*>( selected ) );
            break;

        case 1: //image
            result = deleteIpodPhoto( dynamic_cast<IpodPhotoItem*>( selected ) );
            break;
    }

    if( result ) //selected item may have been deleted by deleteIpodPhoto
        delete selected;

    GError *err = 0;
    itdb_photodb_write( m_itdb, &err );
#endif
}

void UploadDialog::addDropItems( QStringList filesPath )
{
#if 0
    if( filesPath.isEmpty() ) return;

    for( QStringList::Iterator it = filesPath.begin() ; it != filesPath.end() ; ++it )
    {
        QString currentDropFile = *it;

        // Check if the new item already exist in the list.

        bool itemExists = false;

        Q3ListViewItemIterator it2( m_uploadList );

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

    enableButton( KDialog::User1, m_uploadList->childCount() > 0 );
#else
    Q_UNUSED(filesPath);
#endif
}

void UploadDialog::addUrlToList( QString file )
{
    QFileInfo *fi = new QFileInfo( file );

    new ImageListItem( m_uploadList, file.section('/', 0, -1), fi->fileName() );

    delete fi;
}


bool UploadDialog::openDevice()
{
    if( m_itdb )
    {
        kDebug(51000) <<  "ipod at " << m_mountPoint << " already opened" << endl;
        return false;
    }

    // try to find a mounted ipod
    bool ipodFound = false;

    KMountPoint::List currentmountpoints = KMountPoint::currentMountPoints();
    for( KMountPoint::List::Iterator mountiter = currentmountpoints.begin();
         mountiter != currentmountpoints.end(); ++mountiter )
    {
        QString devicenode = (*mountiter)->mountedFrom();
        QString mountpoint = (*mountiter)->mountPoint();

        if( !m_mountPoint.isEmpty() &&
             mountpoint != m_mountPoint )
            continue;

        if( mountpoint.startsWith( "/proc" ) ||
            mountpoint.startsWith( "/sys" )  ||
            mountpoint.startsWith( "/dev" )  ||
            mountpoint.startsWith( "/boot" ) )
            continue;

        if( !m_deviceNode.isEmpty() &&
             devicenode != m_deviceNode )
            continue;

        /// Detecting whether an iPod exists.
        QString path = QString( itdb_get_control_dir( QFile::encodeName( mountpoint ) ) );
        QDir d( path );

        if( path.isEmpty() || !d.exists() )
            continue;

        if( m_mountPoint.isEmpty() )
            m_mountPoint = mountpoint;

        /// Here, we have found an ipod, but we are not sure if the photo db exists.
        /// Try and parse it to determine whether we have initialised the iPod.
        ipodFound = true;
        GError *err = 0;
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
        kDebug(51000) << "no mounted ipod found" << endl;
        if( m_itdb )
        {
            itdb_photodb_free( m_itdb );
            m_itdb = 0;
        }
        return false;
    }

    kDebug(51000) << "ipod found mounted at " << m_mountPoint << endl;

    /// No photodb was able to be parsed, so offer to initialise the ipod for the user.
    if( !m_itdb )
    {
        kDebug(51000) << "could not find iTunesDB on device mounted at " << m_mountPoint << endl;

        QString msg = i18n( "An iPod photo database could not be found on device mounted at %1. "
                            "Should I try to initialize your iPod photo database?",
                            m_mountPoint);

        if( KMessageBox::warningContinueCancel( this, msg, i18n( "Initialize iPod Photo Database?" ),
                    KGuiItem(i18n("&Initialize"), "new") ) == KMessageBox::Continue )
        {

            m_itdb = itdb_photodb_create( QFile::encodeName( m_mountPoint ) );
            itdb_device_set_mountpoint( m_itdb->device, QFile::encodeName( m_mountPoint ) );

            if( !m_itdb )
            {
                kDebug(51000) << "Could not initialise photodb..." << endl;
                return false;
            }

            GError *err = 0;
            itdb_photodb_write( m_itdb, &err );
        }
        else
            return false;
    }

    return true;
}

Itdb_Artwork *UploadDialog::photoFromId(const uint id)
{
    if( !m_itdb )
        return 0;

    for( GList *it = m_itdb->photos; it; it=it->next )
    {
        Itdb_Artwork *photo = (Itdb_Artwork*)it->data;
        if( !photo )
            return 0;

        if( photo->id == id )
            return photo;
    }
    return 0;
}

QString UploadDialog::ipodModel() const
{
    if( m_ipodInfo )
        return QString( itdb_info_get_ipod_model_name_string( m_ipodInfo->ipod_model ) );

    return QString::null;
}


void UploadDialog::refreshDevices()
{
    kDebug(51000) << "refreshing ipod devices" << endl;

    if( !m_ipodHeader )
        return;

    m_ipodHeader->disconnect();

    if( !openDevice() )
    {
        m_ipodHeader->setViewType( IpodHeader::NoIpod );

        connect(m_ipodHeader, SIGNAL( refreshDevices() ),
                this, SLOT( refreshDevices() ));
    }
    else //device opened! hooray!
    {
        m_ipodInfo = const_cast<Itdb_IpodInfo*>( itdb_device_get_ipod_info( m_itdb->device ) );
        const QString model = ipodModel();

        if( !m_ipodInfo || model.isEmpty() || model == "Invalid" )
        {
            kDebug(51000) << "the ipod model must be set before photos can be added" << endl;
            m_ipodHeader->setViewType( IpodHeader::IncompatibleIpod );

            connect(m_ipodHeader, SIGNAL( updateSysInfo() ),
                    this, SLOT( updateSysInfo() ) );

            return;
        }
        else
        {
            m_ipodHeader->setViewType( IpodHeader::ValidIpod );
        }
    }

    if( m_ipodAlbumList )
        getIpodAlbums();

    m_destinationBox->setEnabled( m_itdb );
    m_urlListBox->setEnabled( m_itdb );
}

void UploadDialog::updateSysInfo()
{
    kDebug(51000) << "updateSysInfo()" << endl;
}

} // namespace KIPIIpodExportPlugin
