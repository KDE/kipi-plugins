//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEDIALOG.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Include files for Qt

#include <qlabel.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlistview.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qfileinfo.h>

// Include files for KDE

#include <klocale.h>
#include <kfontdialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kurlrequester.h>
#include <kurl.h>
#include <kdirsize.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kglobalsettings.h>
#include <kmessagebox.h>
#include <kbuttonbox.h>
#include <kapplication.h>
#include <ksqueezedtextlabel.h>
#include <kio/global.h>
#include <kio/previewjob.h>

// Include files for KIPI

#include <libkipi/imagecollection.h>

// Local include files

#include "finddupplicatedialog.h"

namespace KIPIFindDupplicateImagesPlugin
{

class AlbumItem : public QCheckListItem
{
public:
    AlbumItem(QListView * parent, const KIPI::ImageCollection& ic )
            : QCheckListItem( parent, ic.name(), QCheckListItem::CheckBox), _ic(ic)
    {}

    QString comments()     { return _ic.comment(); }
    QString name()         { return _ic.name();       }
    KIPI::ImageCollection imageCollection() { return _ic; }
    int     items()        { return _ic.images().count();      }

private:
    KIPI::ImageCollection _ic;
};


/////////////////////////////////////////////////////////////////////////////////////////////

FindDuplicateDialog::FindDuplicateDialog( KIPI::Interface* interface, QWidget *parent)
                   : KDialogBase( IconList, i18n("Configure"), Help|Ok|Cancel,
                                  Ok, parent, "FindDuplicateDialog", true, true ), m_dialogOk( false ),
                                  m_interface( interface )
{
    // Get the image files filters from the hosts app.
     
    m_ImagesFilesSort = m_interface->fileExtensions();
  
    setCaption(i18n("Find Duplicate Images"));
    setupSelection();
    setupPageMethod();
    aboutPage();
    page_setupSelection->setFocus();
    setHelp("findimages", "kipi-plugins");
    setAlbumsList();
    resize( 500, 500 );
}


/////////////////////////////////////////////////////////////////////////////////////////////

FindDuplicateDialog::~FindDuplicateDialog()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::setupSelection(void)
{
    page_setupSelection = addPage(i18n("Selection"),
                                  i18n("Albums selection"),
                                  BarIcon("endturn", KIcon::SizeMedium));

    QVBoxLayout *layout = new QVBoxLayout(page_setupSelection, 0, spacingHint() );

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( page_setupSelection );
    groupBox1->setFlat(false);
    groupBox1->setTitle(i18n("Select Albums for to find duplicate images"));
    QGridLayout* grid = new QGridLayout( groupBox1, 2, 2 , 20, 20);

    m_AlbumsList = new QListView( groupBox1 );
    m_AlbumsList->setResizeMode( QListView::LastColumn );
    m_AlbumsList->addColumn("");
    m_AlbumsList->header()->hide();
    QWhatsThis::add( m_AlbumsList, i18n("<p>Selected here the Albums for to find the "
                                        "duplicate images in your Albums database.") );
    grid->addMultiCellWidget(m_AlbumsList, 0, 2, 0, 1);

    KButtonBox* albumSelectionButtonBox = new KButtonBox( groupBox1, Vertical );
    QPushButton* buttonSelectAll = albumSelectionButtonBox->addButton ( i18n( "&Select all" ) );
    QWhatsThis::add( buttonSelectAll, i18n("<p>Select all Albums in the list.") );
    QPushButton* buttonInvertSelection = albumSelectionButtonBox->addButton ( i18n( "&Invert selection" ));
    QWhatsThis::add( buttonInvertSelection, i18n("<p>Invert the Albums selection in the list.") );
    QPushButton* buttonSelectNone = albumSelectionButtonBox->addButton ( i18n( "Select &none" ) );
    QWhatsThis::add( buttonSelectNone, i18n("<p>Deselect all Albums in the list.") );
    albumSelectionButtonBox->layout();
    grid->addMultiCellWidget(albumSelectionButtonBox, 0, 1, 2, 2);

    m_albumPreview = new QLabel( groupBox1 );
    m_albumPreview->setFixedHeight( 120 );
    m_albumPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_albumPreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( m_albumPreview, i18n( "Preview of the first image in the current selected Album." ) );
    grid->addMultiCellWidget(m_albumPreview, 2, 2, 2, 2);

    layout->addWidget( groupBox1 );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Album description"), page_setupSelection );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The description of the current Album in the selection list.") );

    QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    m_AlbumComments = new KSqueezedTextLabel( groupBox2 );
    m_AlbumComments->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumComments );

    m_AlbumCollection = new KSqueezedTextLabel( groupBox2 );
    m_AlbumCollection->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumCollection );

    m_AlbumDate = new KSqueezedTextLabel( groupBox2 );
    m_AlbumDate->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumDate );

    m_AlbumItems = new KSqueezedTextLabel( groupBox2 );
    m_AlbumItems->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumItems );

    layout->addWidget( groupBox2 );
    layout->addStretch(1);

    //---------------------------------------------

    connect( buttonSelectAll, SIGNAL( clicked() ),
             this, SLOT( slotbuttonSelectAll() ) );

    connect( buttonInvertSelection, SIGNAL( clicked() ),
             this, SLOT( slotbuttonInvertSelection() ) );

    connect( buttonSelectNone, SIGNAL( clicked() ),
             this, SLOT( slotbuttonSelectNone() ) );

    connect( m_AlbumsList, SIGNAL( currentChanged( QListViewItem * ) ),
             this, SLOT( albumSelected( QListViewItem * ) ) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::setAlbumsList(void)
{
    QValueList<KIPI::ImageCollection> albums = m_interface->allAlbums();

    for( QValueList<KIPI::ImageCollection>::ConstIterator it = albums.begin(); it != albums.end(); ++it ) {
        KIPI::ImageCollection album = *it;

        /*AlbumItem *item = */new AlbumItem( m_AlbumsList, album );

        // I have no idea how we handle this.  1 Jun. 2004 23:30 -- Jesper K. Pedersen
#ifdef TEMPORARILY_REMOVED
        if (album == Digikam::AlbumManager::instance()->currentAlbum())
           {
           item->setOn(true);
           item->setSelected(true);
           albumSelected( item );
           currentAlbum = item;
           }
        else
           item->setOn(false);
#endif
        }

        // I have no idea how we handle this.  1 Jun. 2004 23:30 -- Jesper K. Pedersen
#ifdef TEMPORARILY_REMOVED
    if (currentAlbum != 0)
       m_AlbumsList->ensureItemVisible(currentAlbum);
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::setupPageMethod(void)
{
    QString whatsThis;
    page_setupMethod = addPage( i18n("Method and cache"), i18n("Find duplicate method and cache configuration"),
                                BarIcon("run", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupMethod, 0, spacingHint() );

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( 2, Qt::Horizontal, i18n("Method"), page_setupMethod );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );

    QLabel *m_labelsearchMethod = new QLabel( i18n("Search method:"), groupBox1 );
    m_findMethod = new QComboBox(false, groupBox1);
    m_findMethod->insertItem(i18n("Almost"));
    m_findMethod->insertItem(i18n("Fast"));
    m_findMethod->setCurrentText (i18n("Almost"));
    QWhatsThis::add( m_findMethod, i18n("<p>Select here the search method of duplicate "
                     "images in Albums database.<p>"
                     "<b>Almost</b>: the algorithm calculate an approximate difference between images. "
                     "This method is a slower method but robust. You can affine the threesholding by the "
                     "\"Approximate Threeshold\" parameter.<p>"
                     "<b>Fast</b>: the algorithm compare bit per bit the files for a fast images parsing. "
                     "This method is a faster method but not robust."));
    m_labelsearchMethod->setBuddy( m_findMethod );

    QLabel *m_label_approximateThreeshold = new QLabel (i18n("Approximate Threeshold:"), groupBox1);
    vlay->addWidget( m_label_approximateThreeshold );

    m_approximateThreshold = new KIntNumInput(88, groupBox1);
    m_approximateThreshold->setRange(60, 100, 1, true );
    QWhatsThis::add( m_approximateThreshold, i18n("<p>Select here the approximate threeshold "
                                                  "value in percent for the almost find duplicate method. "
                                                  "This value is used by the algorithm to distinguish 2 "
                                                  "similar images. The default value is 88.") );
    vlay->addWidget( groupBox1 );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( 1, Qt::Horizontal, i18n("Cache maintenance"), page_setupMethod );
    new QLabel(i18n("The find duplicate images process use a cache folder images fingerprint\n"
                    "for a speed Albums items analyse. The cache location is \"~/.findduplicate\"."), groupBox2);

    QPushButton* updateCache = new QPushButton( groupBox2, "UpdateCache" );
    updateCache->setText(i18n( "&Update cache" ));
    QWhatsThis::add( updateCache, i18n("<p>Updating the cache data for the selected Albums.") );
    QPushButton* purgeCache = new QPushButton( groupBox2, "PurgeCacheAlbumsSelected" );
    purgeCache->setText(i18n( "&Purge cache (Albums selected)" ));
    QWhatsThis::add( purgeCache, i18n("<p>Purge the cache data for the selected Albums.") );
    QPushButton* purgeAllCache = new QPushButton( groupBox2, "PurgeAllCache" );
    purgeAllCache->setText(i18n( "&Purge all cache" ));
    QWhatsThis::add( purgeAllCache, i18n("<p>Purge the cache data for all Albums.") );
    vlay->addWidget( groupBox2 );

    vlay->addStretch(1);

    //---------------------------------------------

    connect(m_findMethod, SIGNAL(activated(const QString &)),
            this, SLOT(slotfindMethodChanged(const QString &)));

    connect(updateCache, SIGNAL(clicked()),
            this, SLOT(slotUpdateCache()));

    connect(purgeCache, SIGNAL(clicked()),
            this, SLOT(slotPurgeCache()));

    connect(purgeAllCache, SIGNAL(clicked()),
            this, SLOT(slotPurgeAllCache()));

    slotfindMethodChanged(m_findMethod->currentText());
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::aboutPage(void)
{
    page_about = addPage( i18n("About"), i18n("About kipi find duplicate images"),
                          BarIcon("kipi", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_about, 0, spacingHint() );

    QLabel *label = new QLabel( i18n("A KIPI plugin for find duplicate images\n\n"
                                     "Author: Gilles Caulier\n\n"
                                     "Email: caulier dot gilles at free.fr\n\n"
                                     "This plugin is based on ShowImg implementation\n"
                                     "by Richard Groult <rgroult at jalix.org>\n"), page_about);

    vlay->addWidget(label);
    vlay->addStretch(1);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotfindMethodChanged(const QString &string)
{
    if ( string == i18n("Almost") )
       m_approximateThreshold->setEnabled(true);
    else
       m_approximateThreshold->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotUpdateCache(void)
{
    QValueList<KIPI::ImageCollection> albumsList = getAlbumsSelection();
    QStringList albumsListPath;

    for( QValueList<KIPI::ImageCollection>::ConstIterator album = albumsList.begin(); album != albumsList.end(); ++album ) {
        if ( !albumsListPath.contains( (*album).path().path() ) )
            albumsListPath.append( (*album).path().path() );
    }


    if ( albumsListPath.isEmpty() == true )
       KMessageBox::sorry(0, i18n("You must selected at least an Album for update cache process!"));
    else
       emit updateCache(albumsListPath);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotPurgeCache(void)
{
    QValueList<KIPI::ImageCollection> albumsList = getAlbumsSelection();
    QStringList albumsListPath;

    for( QValueList<KIPI::ImageCollection>::ConstIterator album = albumsList.begin(); album != albumsList.end(); ++album ) {
        if ( !albumsListPath.contains( (*album).path().path() ) )
            albumsListPath.append( (*album).path().path() );
    }

    if ( albumsListPath.isEmpty() == true )
       KMessageBox::sorry(0, i18n("You must selected at least an Album for purge cache process!"));
    else
       emit clearCache(albumsListPath);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotPurgeAllCache(void)
{
     emit clearAllCache();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::albumSelected( QListViewItem * item )
{
    if ( !item ) return;

    AlbumItem *pitem = static_cast<AlbumItem*>( item );
    if ( pitem == NULL ) return;
    m_AlbumComments->setText( i18n("Comment: %1").arg(pitem->comments()) );
#ifdef TEMPORARILY_REMOVED
    m_AlbumCollection->setText( i18n("Collection: %1").arg(pitem->collection()) );
    m_AlbumDate->setText( i18n("Date: %1").arg( pitem->date().toString(( Qt::LocalDate ) ) ) );
#endif
    m_AlbumItems->setText( i18n("Items: %1").arg( pitem->items() ) );

    m_albumPreview->clear();

    KURL url = pitem->imageCollection().images()[0];

    KIO::PreviewJob* thumbJob = KIO::filePreview( url, m_albumPreview->height() );

    connect(thumbJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotPreview(const KFileItem*, const QPixmap&)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotGotPreview(const KFileItem* /*url*/, const QPixmap &pixmap)
{
    m_albumPreview->setPixmap(pixmap);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotOk()
{
    if (getAlbumsSelection().isEmpty() == true)
       {
       KMessageBox::sorry(0, i18n("You must selected at least an Album for to find duplicate images!"));
       return;
       }

    accept();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotbuttonSelectAll(void)
{
    QListViewItemIterator it( m_AlbumsList );

    while ( it.current() )
        {
        AlbumItem *item = static_cast<AlbumItem*>( it.current() );

        if (!item->isOn())
            item->setOn(true);

        ++it;
        }

    albumSelected( m_AlbumsList->currentItem() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotbuttonInvertSelection(void)
{
    QListViewItemIterator it( m_AlbumsList );

    while ( it.current() )
        {
        AlbumItem *item = static_cast<AlbumItem*>( it.current() );

        if (!item->isOn())
            item->setOn(true);
        else
            item->setOn(false);

        ++it;
        }

    albumSelected( m_AlbumsList->currentItem() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotbuttonSelectNone(void)
{
    QListViewItemIterator it( m_AlbumsList );

    while ( it.current() )
        {
        AlbumItem *item = static_cast<AlbumItem*>( it.current() );

        if (item->isOn())
            item->setOn(false);

        ++it;
        }

    albumSelected( m_AlbumsList->currentItem() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

QValueList<KIPI::ImageCollection> FindDuplicateDialog::getAlbumsSelection(void)
{
    QValueList<KIPI::ImageCollection> AlbumsListSelected;
    QListViewItemIterator it( m_AlbumsList );

    while ( it.current() )
        {
        AlbumItem *item = static_cast<AlbumItem*>( it.current() );

        if (item->isOn())
            AlbumsListSelected.append( item->imageCollection() );

        ++it;
        }

    return (AlbumsListSelected);
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString FindDuplicateDialog::getFindMethod() const
{
    return m_findMethod->currentText();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::setFindMethod(QString Value)
{
    return m_findMethod->setCurrentText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const int FindDuplicateDialog::getApproximateThreeshold() const
{
    return m_approximateThreshold->value();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::setApproximateThreeshold(int Value)
{
    return m_approximateThreshold->setValue( Value );
}

}  // NameSpace KIPIFindDupplicateImagesPlugin

#include "finddupplicatedialog.moc"
