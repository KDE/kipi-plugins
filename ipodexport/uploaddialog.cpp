/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz at kde dot org>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "uploaddialog.moc"

// Libgdk includes

extern "C"
{
#include <gdk-pixbuf/gdk-pixbuf.h>
}

// Qt includes

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
#include <QTimer>
#include <QPointer>
#include <QCloseEvent>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <kguiitem.h>
#include <khelpmenu.h>
#include <kinputdialog.h>
#include <kio/previewjob.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kmountpoint.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kurl.h>
#include <kdeversion.h>

// Libkipi includes

#include <libkipi/imagecollection.h>

// Local includes

#include "ipodheader.h"
#include "imagelist.h"
#include "imagelistitem.h"
#include "ipodlistitem.h"
#include "kpimageinfo.h"
#include "kpimagedialog.h"
#include "kpmetadata.h"

namespace KIPIIpodExportPlugin
{

UploadDialog* UploadDialog::s_instance = 0;

UploadDialog::UploadDialog(const QString& caption, QWidget* const /*parent*/)
    : KPToolDialog(0),
      m_transferring(false),
      m_itdb(0),
      m_ipodInfo(0),
      m_ipodHeader(0),
      m_mountPoint(QString()),
      m_deviceNode(QString()),
      m_ipodAlbumList(0)
{
    s_instance         = this;
    QWidget* const box = new QWidget();
    setMainWidget( box );
    setCaption( caption );
    setButtons( Close|Help );
    setModal( false );

    // ---------------------------------------------------------------
    // About data and help button.

    KPAboutData* about = new KPAboutData(ki18n("iPod Export"),
                                         0,
                                         KAboutData::License_GPL,
                                         ki18n("A tool to export image to an iPod device"),
                                         ki18n("(c) 2006-2008, Seb Ruiz\n"
                                               "(c) 2008-2013, Gilles Caulier"));

    about->addAuthor(ki18n("Seb Ruiz"), ki18n("Author and Maintainer"),
                     "ruiz@kde.org");

    about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->setHandbookEntry("ipodexport");
    setAboutData(about);

    // ------------------------------------------------------------

    QGridLayout* const grid = new QGridLayout(box);
    m_ipodHeader            = new IpodHeader(box);

    // Setup widgets and layout for the source
    m_urlListBox = new QGroupBox( i18n("Hard Disk"), box );

    QHBoxLayout* const urlLayout = new QHBoxLayout;

    m_uploadList = new ImageList( ImageList::UploadType, this );
    m_uploadList->setMinimumHeight( 80 );

    m_uploadList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

    QVBoxLayout* const uploadUrlLayout = new QVBoxLayout;
    m_addImagesButton = new QPushButton( i18n("&Add..."), this );
    m_addImagesButton->setWhatsThis(i18n("Add images to be queued for the iPod."));

    m_remImagesButton = new QPushButton( i18n( "&Remove" ), this );
    m_remImagesButton->setWhatsThis( i18n("Remove selected image from the list.") );

    m_transferImagesButton = new QPushButton( i18n( "&Transfer" ), this );
    m_transferImagesButton->setWhatsThis( i18n("Transfer images to the selected iPod album.") );

    m_imagePreview = new QLabel;
    m_imagePreview->setFixedHeight( 80 );
    m_imagePreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_imagePreview->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred) );
    m_imagePreview->setWhatsThis( i18n("The preview of the selected image in the list.") );

    QLabel* const hdd_icon = new QLabel;
    hdd_icon->setPixmap(KIconLoader::global()->loadIcon("computer",
                                                        KIconLoader::Desktop,
                                                        KIconLoader::SizeHuge));

    uploadUrlLayout->addWidget( m_addImagesButton );
    uploadUrlLayout->addWidget( m_remImagesButton );
    uploadUrlLayout->addWidget( m_transferImagesButton );
    uploadUrlLayout->addWidget( m_imagePreview );
    uploadUrlLayout->addStretch( 1 );
    uploadUrlLayout->addWidget( hdd_icon );
    uploadUrlLayout->setSpacing( spacingHint() );

    urlLayout->addWidget( m_uploadList );
    urlLayout->addLayout( uploadUrlLayout );

    m_urlListBox->setLayout( urlLayout );

    // Setup widgets and layout for the iPod (destination)
    m_destinationBox                        = new QGroupBox( i18n("iPod"), box );
    QHBoxLayout* const destinationBoxLayout = new QHBoxLayout( m_destinationBox );

    m_ipodAlbumList = new ImageList( ImageList::IpodType, this );
    m_ipodAlbumList->setMinimumHeight( 80 );

    QVBoxLayout* const ipodButtonLayout = new QVBoxLayout;

    m_createAlbumButton = new QPushButton( i18n("&New..."), this );
    m_createAlbumButton->setWhatsThis( i18n( "Create a new photo album on the iPod." ) );

    m_removeAlbumButton = new QPushButton( i18n("&Remove"), this );
    m_renameAlbumButton = new QPushButton( i18n("R&ename..."), this );

    m_removeAlbumButton->setEnabled( false );
    m_renameAlbumButton->setEnabled( false );

    m_removeAlbumButton->setWhatsThis( i18n("Remove the selected photos or albums from the iPod.") );
    m_renameAlbumButton->setWhatsThis( i18n("Rename the selected photo album on the iPod.") );

    QLabel* const ipod_icon = new QLabel;
    ipod_icon->setPixmap(KIconLoader::global()->loadIcon("multimedia-player-apple-ipod",
                                                         KIconLoader::Desktop,
                                                         KIconLoader::SizeHuge ) );

    m_ipodPreview = new QLabel;
    m_ipodPreview->setFixedHeight( 80 );
    m_ipodPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_ipodPreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );

    ipodButtonLayout->addWidget( m_createAlbumButton );
    ipodButtonLayout->addWidget( m_removeAlbumButton );
    ipodButtonLayout->addWidget( m_renameAlbumButton );
    ipodButtonLayout->addWidget( m_ipodPreview );
    ipodButtonLayout->addStretch( 1 );
    ipodButtonLayout->addWidget( ipod_icon );

    destinationBoxLayout->addWidget( m_ipodAlbumList );
    destinationBoxLayout->addLayout( ipodButtonLayout );

    m_destinationBox->setLayout( destinationBoxLayout );

    // Add sub-layouts to the main layout
    grid->addWidget( m_ipodHeader,     0, 0, 1, 2 ); // colspan=2
    grid->addWidget( m_urlListBox,     1, 0, 1, 1 );
    grid->addWidget( m_destinationBox, 1, 1, 1, 1 );
    grid->setSpacing( spacingHint() );
    grid->setMargin( 0 );

    /// populate the ipod view with a list of albums etc
    refreshDevices();

    loadImagesFromCurrentSelection();

    enableButtons();

    /// connect the signals & slots

    connect(m_createAlbumButton, SIGNAL(clicked()),
            this, SLOT(createIpodAlbum()) );

    connect(m_removeAlbumButton, SIGNAL(clicked()),
            this, SLOT(deleteIpodAlbum()) );

    connect(m_renameAlbumButton, SIGNAL(clicked()),
            this, SLOT(renameIpodAlbum()) );

    connect(m_addImagesButton, SIGNAL(clicked()),
            this, SLOT(imagesFilesButtonAdd()) );

    connect(m_remImagesButton, SIGNAL(clicked()),
            this, SLOT(imagesFilesButtonRem()) );

    connect(m_transferImagesButton, SIGNAL(clicked()),
            this, SLOT(startTransfer()) );

    connect(m_uploadList, SIGNAL(signalAddedDropItems(QStringList)),
            this, SLOT(addDropItems(QStringList)) );

    connect(m_uploadList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(imageSelected(QTreeWidgetItem*)));

    connect(m_ipodAlbumList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(ipodItemSelected(QTreeWidgetItem*)));
}

UploadDialog::~UploadDialog()
{
    if( m_itdb )
        itdb_photodb_free( m_itdb );
}

void UploadDialog::slotClose()
{
    m_uploadList->clear();
    done(Close);
}

void UploadDialog::closeEvent(QCloseEvent* e)
{
    if (!e) return;

    m_uploadList->clear();
    e->accept();
}

void UploadDialog::reactivate()
{
    loadImagesFromCurrentSelection();
    show();
}

void UploadDialog::loadImagesFromCurrentSelection()
{
#if KIPI_PLUGIN
    /// add selected items to the ImageList
    ImageCollection images = iface()->currentSelection();

    if( images.isValid() )
    {
        KUrl::List selected = images.images();

        for( KUrl::List::Iterator it = selected.begin(); it != selected.end(); ++it )
        {
            addUrlToList( (*it).path() );
        }
    }
#endif
}

void UploadDialog::getIpodAlbums()
{
    if( !m_itdb )
        return;

    kDebug() << "populating ipod view" ;

    // clear cache
    m_ipodAlbumList->clear();

    for( GList* it = m_itdb->photoalbums; it; it = it->next )
    {
        Itdb_PhotoAlbum* const ipodAlbum = (Itdb_PhotoAlbum*) it->data;

        kDebug() << " found album: " << ipodAlbum->name ;

        IpodAlbumItem* const albumItem   = new IpodAlbumItem( m_ipodAlbumList, ipodAlbum );
        m_ipodAlbumList->addTopLevelItem( albumItem );
        getIpodAlbumPhotos( albumItem, ipodAlbum );
    }
}

void UploadDialog::getIpodAlbumPhotos(IpodAlbumItem* const item, Itdb_PhotoAlbum* const album)
{
    if( !item || !album || !m_itdb )
        return;

    IpodPhotoItem* last = 0;

    for( GList* it = album->members; it; it = it->next )
    {
        Itdb_Artwork* const photo = (Itdb_Artwork*) it->data;
        gint photo_id             = photo->id;
        last                      = new IpodPhotoItem( item, last, photo );
        last->setText( 0, QString::number( photo_id ) );
    }
}

void UploadDialog::reloadIpodAlbum(IpodAlbumItem* const item, Itdb_PhotoAlbum* const album )
{
    if( !item ) return;

    while( item->child( 0 ) )
        delete item->child( 0 ); // clear the items, so we can reload them again

    Itdb_PhotoAlbum* ipodAlbum = 0;

    for( GList* it = m_itdb->photoalbums; it; it = it->next )
    {
        ipodAlbum = (Itdb_PhotoAlbum*)it->data;

        if( strcmp( ipodAlbum->name, album->name ) == 0 )
            break; // we found the album
    }

    dynamic_cast<IpodAlbumItem*>(item)->setPhotoAlbum( ipodAlbum );

    getIpodAlbumPhotos( item, ipodAlbum );
}

void UploadDialog::enableButtons()
{
    // enable the start button only if there are albums to transfer to, items to transfer
    // and a database to add to!
    const bool transfer = m_uploadList->model()->hasChildren()    && // we have items to transfer
                          m_ipodAlbumList->model()->hasChildren() && // the ipod has albums
                          !m_transferring                         && // we aren't transferring
                          m_ipodAlbumList->currentItem()          && // selected a destination album
                          m_itdb;

    m_transferImagesButton->setEnabled( transfer );

    enableButton( Close, !m_transferring );

    const QList<QTreeWidgetItem*> ipodSelection = m_ipodAlbumList->selectedItems();
    const bool hasSelection                     = ipodSelection.count() != 0;
    const bool isMasterLibrary                  = hasSelection && ipodSelection.first() == m_ipodAlbumList->topLevelItem( 0 );
    const bool isAlbum                          = hasSelection && ( dynamic_cast<IpodAlbumItem*>( ipodSelection.first() ) != 0 );

    m_removeAlbumButton->setEnabled( hasSelection && !isMasterLibrary );
    m_renameAlbumButton->setEnabled( hasSelection && !isMasterLibrary && isAlbum );
}

void UploadDialog::startTransfer()
{
    if( !m_itdb || !m_uploadList->model()->hasChildren() )
        return;

    QTreeWidgetItem* const selected = m_ipodAlbumList->currentItem();
    IpodAlbumItem* const ipodAlbum  = dynamic_cast<IpodAlbumItem*>( selected );

    if( !selected || !ipodAlbum )
        return;

    m_transferring               = true;
    Itdb_PhotoAlbum* const album = ipodAlbum->photoAlbum();

    enableButton(User1, false);
    enableButton(Close, false);

    GError* err = 0;

    while( QTreeWidgetItem* const item = m_uploadList->takeTopLevelItem(0) )
    {
#define item static_cast<ImageListItem*>(item)
        kDebug() << "Uploading "      << item->pathSrc()
                 << " to ipod album " << album->name ;

        Itdb_Artwork* const art = itdb_photodb_add_photo( m_itdb, QFile::encodeName( item->pathSrc() ), 0, 0, &err );

        if( !art )
        {
            if( err )
            {
                kDebug() << "Error adding photo " << item->pathSrc() << " to database:"
                              << err->message ;
                err = 0;
            }
        }
        else
        {
            itdb_photodb_photoalbum_add_photo( m_itdb, album, art, 0 );
        }

        delete item;
#undef item
    }

    itdb_photodb_write( m_itdb, &err );

    if( err )
        kDebug() << "Failed with error: " << err->message ;

    reloadIpodAlbum( ipodAlbum, album );

    IpodAlbumItem* const library = static_cast<IpodAlbumItem*>( m_ipodAlbumList->topLevelItem(0) );
    reloadIpodAlbum( library, library->photoAlbum() );

    m_transferring = false;

    enableButtons();
}

void UploadDialog::ipodItemSelected(QTreeWidgetItem* item)
{
    m_ipodPreview->clear();

    if( m_ipodAlbumList->currentItem() )
        m_ipodAlbumList->currentItem()->setSelected( true );

    // Need a singleshot, else the selected items don't get updated.
    QTimer::singleShot(0, this, SLOT(enableButtons()));

    IpodPhotoItem* const ipoi = dynamic_cast<IpodPhotoItem*>(item);
    if( !ipoi )
        return;

    Itdb_Artwork* const artwork = ipoi->artwork();

    if (!artwork)
        return;

    GdkPixbuf* gpixbuf = 0;

    // First arg in itdb_artwork_get_pixbuf(...) is pointer to Itdb_Device struct,
    // in kipiplugin-ipodexport it is m_itdb->device. i hope it _is_ initialiezed
    gpixbuf = (GdkPixbuf*) itdb_artwork_get_pixbuf( m_itdb->device, artwork, -1, -1 );

    if( !gpixbuf )
    {
        kDebug() << "no thumb was found" ;
        return;
    }

//     GdkPixbuf* buf = itdb_thumb_get_gdk_pixbuf( m_itdb->device, thumb );
//     int size       = 0;
//     QImage *image  = buf->convertToImage();
//     kDebug() << "image size: " << image->size() ;
//
//     QPixmap pix;
//     pix.convertFromImage( image );
//     m_ipodPreview->setPixmap( pix );

    // memory release
    g_object_unref ( gpixbuf );
}

void UploadDialog::imageSelected(QTreeWidgetItem* item)
{
    if( !item || m_transferring )
    {
        m_imagePreview->clear();
        return;
    }

    ImageListItem* const pitem = dynamic_cast<ImageListItem*>( item );

    if ( !pitem )
        return;

    m_imagePreview->clear();

    QString IdemIndexed = "file:" + pitem->pathSrc();

    KUrl url( IdemIndexed );

    if ( !url.isValid() )
        return;

#if KDE_IS_VERSION(4,7,0)
    KFileItemList items;
    items.append(KFileItem(KFileItem::Unknown, KFileItem::Unknown, url, true));
    KIO::PreviewJob* const m_thumbJob = KIO::filePreview(items, QSize(m_imagePreview->height(), m_imagePreview->height()));
#else
    KIO::PreviewJob* const m_thumbJob = KIO::filePreview(url, m_imagePreview->height());
#endif

    connect(m_thumbJob, SIGNAL(gotPreview(const KFileItem*,QPixmap)),
            this,   SLOT(gotImagePreview(const KFileItem*,QPixmap)) );
}

void UploadDialog::gotImagePreview(const KFileItem* url, const QPixmap& pixmap)
{
#if KIPI_PLUGIN
    QPixmap pix( pixmap );

    // Rotate the thumbnail compared to the angle the host application dictate
    KPImageInfo info(url->url());

    if ( info.orientation() != KPMetadata::ORIENTATION_UNSPECIFIED )
    {
        QImage img     = pix.toImage();
        QMatrix matrix = RotationMatrix::toMatrix(info.orientation());
        img            = img.transformed( matrix );
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
    KUrl::List  urls;

#if KIPI_PLUGIN
    urls = KPImageDialog::getImageUrls(this);
#else
    const QString filter      = QString( "*.jpg *.jpeg *.jpe *.tiff *.gif *.png *.bmp|" + i18n("Image files") );
    QPointer<KFileDialog> dlg = new KFileDialog( QString(), filter, this );
//    QPointer<KFileDialog> dlg = new KFileDialog( QString::null, filter, this, "addImagesDlg", true );
    dlg->setCaption( i18n("Add Images") );
    dlg->setMode( KFile::Files | KFile::Directory );
    dlg->exec();
    urls = dlg->selectedUrls();
    delete dlg;
#endif

    if ( urls.isEmpty() ) return;

    for( KUrl::List::ConstIterator it = urls.constBegin() ; it != urls.constEnd() ; ++it )
        fileList << (*it).path();

    addDropItems( fileList );
}

void UploadDialog::imagesFilesButtonRem()
{
    QList<QTreeWidgetItem*> selected = m_uploadList->selectedItems();
    qDeleteAll( selected );

    enableButton( User1, m_uploadList->model()->hasChildren() );
}

void UploadDialog::createIpodAlbum()
{
    QString helper;

#if KIPI_PLUGIN
    ImageCollection album = iface()->currentAlbum();

    if( album.isValid() )
        helper = album.name();
#endif

    bool ok          = false;
    QString newAlbum = KInputDialog::getText( i18n("New iPod Photo Album"),
                                              i18n("Create a new album:"),
                                              helper, &ok, this );
    if( ok )
    {
        kDebug() << "creating album " << newAlbum ;

        Itdb_PhotoAlbum* const photoAlbum = itdb_photodb_photoalbum_create( m_itdb, QFile::encodeName( newAlbum ), -1/*end*/ );
        // add the new album to the list view
        new IpodAlbumItem( m_ipodAlbumList, photoAlbum );
        m_ipodAlbumList->clearSelection();

        // commit the changes to the iPod
        GError* err = 0;
        itdb_photodb_write( m_itdb, &err );
    }
}

void UploadDialog::renameIpodAlbum()
{
    QList<QTreeWidgetItem*> selectedItems = m_ipodAlbumList->selectedItems();

    // only allow renaming of 1 album item
    if( selectedItems.size() != 1 )
        return;

    IpodAlbumItem* const selected = dynamic_cast<IpodAlbumItem*>( selectedItems.first() );

    if( !selected )
        return;

    bool ok         = false;
    QString newName = KInputDialog::getText( i18n("Rename iPod Photo Album"),
                                             i18n("New album title:"),
                                             selected->text(0), &ok, this );
    if( ok )
    {
        // change the name on the ipod, and rename the listviewitem
        selected->setName( newName );
        // commit changes to the iPod
        GError* err = 0;
        itdb_photodb_write( m_itdb, &err );
    }
}

bool UploadDialog::deleteIpodPhoto(IpodPhotoItem* const photo) const
{
    if( !photo )
        return false;

    IpodAlbumItem* album = static_cast<IpodAlbumItem*>( photo->parent() );

    if( !album )
        return false;

    Itdb_Artwork* const artwork = photo->artwork();

    if( !artwork )
    {
        kDebug() << "Could not find photo artwork with id: " << photo->text(0) ;
        return false;
    }

    Itdb_PhotoAlbum* const photo_album = album->photoAlbum();
    itdb_photodb_remove_photo( m_itdb, photo_album, artwork );

    // if we remove from the library, remove from all sub albums too
    if( photo_album->album_type == 0x01 ) // master album
    {
        for( int i = 1; // skip library
             i < m_ipodAlbumList->topLevelItemCount(); ++ i )
        {
            QTreeWidgetItem* const albumItem = m_ipodAlbumList->topLevelItem( i );

            for( int j = 0; j < albumItem->childCount(); ++j )
            {
                QTreeWidgetItem* const photoItem = albumItem->child( j );

                if( photoItem->text(0) == photo->text(0) ) // FIXME
                {
                    kDebug() << "removing reference to photo from album " << albumItem->text(0) ;
                    delete photoItem;
                    break; // Items can't be duplicated in the same album
                }
            }
        }
    }

    return true;
}

bool UploadDialog::deleteIpodAlbum(IpodAlbumItem* const album) const
{
    kDebug() << "deleting album: " << album->name() << ", and removing all photos" ;
    itdb_photodb_photoalbum_remove( m_itdb, album->photoAlbum(), true/*remove photos*/);

    return true;
}

void UploadDialog::deleteIpodAlbum()
{
    QList<QTreeWidgetItem*> selected = m_ipodAlbumList->selectedItems();

    Q_FOREACH(QTreeWidgetItem* const item, selected)
    {
        IpodAlbumItem* const album = dynamic_cast<IpodAlbumItem*>( item );

        if( album )
        {
            if( deleteIpodAlbum( album ) )
               delete album;

            continue;
        }

        IpodPhotoItem* const photo = dynamic_cast<IpodPhotoItem*>( item );

        if( photo )
        {
            if( deleteIpodPhoto( photo ) )
                delete photo;

            continue;
        }
    }

    GError* err = 0;
    itdb_photodb_write( m_itdb, &err );
}

void UploadDialog::addDropItems(const QStringList& filesPath)
{
    if( filesPath.isEmpty() )
        return;

    Q_FOREACH( const QString& dropFile, filesPath )
    {
        // TODO: Check if the new item already exist in the list.
        addUrlToList( dropFile );
    }

    enableButton( User1, m_uploadList->model()->hasChildren() > 0 );
}

void UploadDialog::addUrlToList(const QString& file)
{
    QFileInfo fi( file );
    new ImageListItem( m_uploadList, file.section('/', 0, -1), fi.fileName() );
}

bool UploadDialog::openDevice()
{
    if( m_itdb )
    {
        kDebug() <<  "ipod at " << m_mountPoint << " already opened" ;
        return false;
    }

    // try to find a mounted ipod
    bool ipodFound                       = false;
    KMountPoint::List currentmountpoints = KMountPoint::currentMountPoints();

    for( KMountPoint::List::Iterator mountiter = currentmountpoints.begin();
         mountiter != currentmountpoints.end(); ++mountiter )
    {
        QString devicenode = (*mountiter)->mountedFrom();
        QString mountpoint = (*mountiter)->mountPoint();

        if( !m_mountPoint.isEmpty() &&
             mountpoint != m_mountPoint )
            continue;

        if( mountpoint.startsWith( QLatin1String("/proc") ) ||
            mountpoint.startsWith( QLatin1String("/sys") )  ||
            mountpoint.startsWith( QLatin1String("/dev") )  ||
            mountpoint.startsWith( QLatin1String("/boot") ) )
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
        ipodFound   = true;
        GError* err = 0;
        m_itdb      = itdb_photodb_parse( QFile::encodeName( mountpoint ), &err );

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
        kDebug() << "no mounted ipod found" ;

        if( m_itdb )
        {
            itdb_photodb_free( m_itdb );
            m_itdb = 0;
        }

        return false;
    }

    kDebug() << "ipod found mounted at " << m_mountPoint ;

    /// No photodb was able to be parsed, so offer to initialise the ipod for the user.
    if( !m_itdb )
    {
        kDebug() << "could not find iTunesDB on device mounted at " << m_mountPoint ;

        QString msg = i18n( "An iPod photo database could not be found on the device mounted at %1. "
                            "Initialize the iPod photo database?",
                            m_mountPoint);

        if( KMessageBox::warningContinueCancel( this, msg, i18n( "Initialize iPod Photo Database?" ),
                         KGuiItem(i18n("&Initialize"), "new") ) == KMessageBox::Continue )
        {

            m_itdb = itdb_photodb_create( QFile::encodeName( m_mountPoint ) );

            if( !m_itdb )
            {
                kDebug() << "Could not initialise photodb..." ;
                return false;
            }

            itdb_device_set_mountpoint( m_itdb->device, QFile::encodeName( m_mountPoint ) );

            GError* err = 0;
            itdb_photodb_write( m_itdb, &err );
        }
        else
        {
            return false;
        }
    }

    return true;
}

Itdb_Artwork* UploadDialog::photoFromId(const uint id) const
{
    if( !m_itdb )
        return 0;

    for( GList* it = m_itdb->photos; it; it=it->next )
    {
        Itdb_Artwork* const photo = (Itdb_Artwork*)it->data;

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

    return QString();
}

void UploadDialog::refreshDevices()
{
    kDebug() << "refreshing ipod devices" ;

    if( !m_ipodHeader )
        return;

    m_ipodHeader->disconnect();

    if( !openDevice() )
    {
        m_ipodHeader->setViewType( IpodHeader::NoIpod );

        connect(m_ipodHeader, SIGNAL(refreshDevices()),
                this, SLOT(refreshDevices()));
    }
    else //device opened! hooray!
    {
        m_ipodInfo          = const_cast<Itdb_IpodInfo*>( itdb_device_get_ipod_info( m_itdb->device ) );
        const QString model = ipodModel();

        if( !m_ipodInfo || model.isEmpty() || model == "Invalid" )
        {
            kDebug() << "the ipod model must be set before photos can be added" ;
            m_ipodHeader->setViewType( IpodHeader::IncompatibleIpod );

            connect(m_ipodHeader, SIGNAL(updateSysInfo()),
                    this, SLOT(updateSysInfo()) );

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
    kDebug() << "updateSysInfo()" ;
}

} // namespace KIPIIpodExportPlugin
