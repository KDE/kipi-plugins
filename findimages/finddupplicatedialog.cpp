//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEDIALOG.CPP
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
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
#include <qprogressdialog.h>

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
#include <klistview.h>

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

    QString comments()                      { return _ic.comment();        }
    QString name()                          { return _ic.name();           }
    int     items()                         { return _ic.images().count(); }
    KIPI::ImageCollection imageCollection() { return _ic;                  }

private:
    KIPI::ImageCollection _ic;
};


/////////////////////////////////////////////////////////////////////////////////////////////

FindDuplicateDialog::FindDuplicateDialog( KIPI::Interface* interface, QWidget *parent)
                   : KDialogBase( IconList, i18n("Configure"), Help|Ok|Cancel,
                                  Ok, parent, "FindDuplicateDialog", true, true ),
                                  m_interface( interface )
{
    setCaption(i18n("Find Duplicate Images"));
    setupSelection();
    setupPageMethod();
    aboutPage();
    page_setupSelection->setFocus();
    setHelp("findimages", "kipi-plugins");
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
                                  i18n("Album's Selection"),
                                  BarIcon("folder_image", KIcon::SizeMedium));

    QVBoxLayout *layout = new QVBoxLayout(page_setupSelection, 0, spacingHint() );

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( page_setupSelection );
    groupBox1->setFlat(false);
    groupBox1->setTitle(i18n("Select Albums to Find Duplicate Images For"));
    QGridLayout* grid = new QGridLayout( groupBox1, 2, 2 , 20, 20);

    m_AlbumsList = new KListView( groupBox1 );
    m_AlbumsList->setResizeMode( QListView::LastColumn );
    m_AlbumsList->addColumn("");
    m_AlbumsList->header()->hide();
    m_AlbumsList->setSelectionModeExt(KListView::Single);
    QWhatsThis::add( m_AlbumsList, i18n("<p>Selected here the Albums for which to find the "
                                        "duplicate images in your Albums database.") );
    grid->addMultiCellWidget(m_AlbumsList, 0, 2, 0, 1);

    KButtonBox* albumSelectionButtonBox = new KButtonBox( groupBox1, Vertical );
    QPushButton* buttonSelectAll = albumSelectionButtonBox->addButton ( i18n( "&Select All" ) );
    QWhatsThis::add( buttonSelectAll, i18n("<p>Select all Albums on the list.") );
    QPushButton* buttonInvertSelection = albumSelectionButtonBox->addButton ( i18n( "&Invert Selection" ));
    QWhatsThis::add( buttonInvertSelection, i18n("<p>Invert the Album selection on the list.") );
    QPushButton* buttonSelectNone = albumSelectionButtonBox->addButton ( i18n( "Select &None" ) );
    QWhatsThis::add( buttonSelectNone, i18n("<p>Deselect all Albums on the list.") );
    albumSelectionButtonBox->layout();
    grid->addMultiCellWidget(albumSelectionButtonBox, 0, 1, 2, 2);

    m_albumPreview = new QLabel( groupBox1 );
    m_albumPreview->setFixedHeight( 120 );
    m_albumPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_albumPreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( m_albumPreview, i18n( "Preview of the first image in the currently selected Album." ) );
    grid->addMultiCellWidget(m_albumPreview, 2, 2, 2, 2);

    layout->addWidget( groupBox1 );

    if ( !m_interface->hasFeature( KIPI::AlbumsUseFirstImagePreview) )
        m_albumPreview->hide();

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Album Description"), page_setupSelection );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The description of the current Album in the selection list.") );

    QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    m_AlbumComments = 0;
    if ( m_interface->hasFeature( KIPI::AlbumsHaveComments ) )
    {
        m_AlbumComments = new KSqueezedTextLabel( groupBox2 );
        m_AlbumComments->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
        groupBox2Layout->addWidget( m_AlbumComments );
    }

    m_AlbumCollection = 0;
    if ( m_interface->hasFeature( KIPI::AlbumsHaveCategory ) )
    {
        m_AlbumCollection = new KSqueezedTextLabel( groupBox2 );
        m_AlbumCollection->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
        groupBox2Layout->addWidget( m_AlbumCollection );
    }

    m_AlbumDate = 0;
    if ( m_interface->hasFeature( KIPI::AlbumsHaveCreationDate ) )
    {
        m_AlbumDate = new KSqueezedTextLabel( groupBox2 );
        m_AlbumDate->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
        groupBox2Layout->addWidget( m_AlbumDate );
    }

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

bool FindDuplicateDialog::setAlbumsList(void)
{
    AlbumItem *currentAlbum = 0;
    int current = 0;
    m_stopParsingAlbum = false;

    QValueList<KIPI::ImageCollection> albums = m_interface->allAlbums();

    m_progressDlg = new QProgressDialog (i18n("Parsing Albums; please wait...."),
                                         i18n("&Cancel"), 0, 0, 0, true);

    connect(m_progressDlg, SIGNAL(cancelled()),
            this, SLOT(slotStopParsingAlbums()));

    m_progressDlg->show();

    for( QValueList<KIPI::ImageCollection>::ConstIterator it = albums.begin(); it != albums.end(); ++it )
        {
        if (m_stopParsingAlbum == true)
           {
           delete m_progressDlg;
           return false;
           }

        m_progressDlg->setProgress(current, albums.count());
        kapp->processEvents();
        ++current;

        KIPI::ImageCollection album = *it;

        AlbumItem *item = new AlbumItem( m_AlbumsList, album );

        if ( m_interface->currentAlbum().isValid() )
           {
           if ( (*it).name() == m_interface->currentAlbum().name() )
              {
              item->setOn(true);
              item->setSelected(true);
              albumSelected( item );
              currentAlbum = item;
              }
           else
              item->setOn(false);
           }
        }

    if (currentAlbum != 0)
       m_AlbumsList->ensureItemVisible(currentAlbum);

    delete m_progressDlg;
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotStopParsingAlbums(void)
{
    m_stopParsingAlbum = true;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::setupPageMethod(void)
{
    QString whatsThis;
    page_setupMethod = addPage( i18n("Method & Cache"), i18n("Find-Duplicates Method and Cache Configuration"),
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
    QWhatsThis::add( m_findMethod, i18n("<p>Select here the search method used to find duplicate "
                     "images in the Albums database.<p>"
                     "<b>Almost</b>: the algorithm calculates an approximate difference between images. "
                     "This method is slower but robust. You can affine the thresholding using the "
                     "\"Approximate Threshold\" parameter.<p>"
                     "<b>Fast</b>: the algorithm compares bit-by-bit the files for fast image parsing. "
                     "This method is faster but is not as robust."));
    m_labelsearchMethod->setBuddy( m_findMethod );

    QLabel *m_label_approximateThreeshold = new QLabel (i18n("Approximate threshold:"), groupBox1);
    vlay->addWidget( m_label_approximateThreeshold );

    m_approximateThreshold = new KIntNumInput(88, groupBox1);
    m_approximateThreshold->setRange(60, 100, 1, true );
    QWhatsThis::add( m_approximateThreshold, i18n("<p>Select here the approximate threshold "
                                                  "value, as a percentage, for the 'Almost' find-duplicates method. "
                                                  "This value is used by the algorithm to distinguish two "
                                                  "similar images. The default value is 88.") );
    vlay->addWidget( groupBox1 );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( 1, Qt::Horizontal, i18n("Cache Maintenance"), page_setupMethod );
    new QLabel(i18n("The find-duplicate-images process uses a cache folder for images' fingerprints\n"
                    "to speed up the analysis of Albums items. The cache location is \"~/.findduplicate\"."), groupBox2);

    QPushButton* updateCache = new QPushButton( groupBox2, "UpdateCache" );
    updateCache->setText(i18n( "&Update Cache" ));
    QWhatsThis::add( updateCache, i18n("<p>Update the cache data for the selected Albums.") );
    QPushButton* purgeCache = new QPushButton( groupBox2, "PurgeCacheAlbumsSelected" );
    purgeCache->setText(i18n( "&Purge Cache (albums selected)" ));
    QWhatsThis::add( purgeCache, i18n("<p>Purge the cache data for the selected Albums.") );
    QPushButton* purgeAllCache = new QPushButton( groupBox2, "PurgeAllCache" );
    purgeAllCache->setText(i18n( "&Purge All Caches" ));
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
    page_about = addPage( i18n("About"), i18n("About KIPI's 'Find Duplicate Images'"),
                          BarIcon("kipi", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_about, 0, spacingHint() );

    QLabel *label = new QLabel( i18n("A KIPI plugin to find duplicate images\n\n"
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

    for( QValueList<KIPI::ImageCollection>::ConstIterator album = albumsList.begin() ;
         album != albumsList.end() ; ++album )
        {
        if ( !albumsListPath.contains( (*album).path().path() ) )
            albumsListPath.append( (*album).path().path() );
        }

    if ( albumsListPath.isEmpty() == true )
       KMessageBox::sorry(0, i18n("You must select at least one Album for the update cache process."));
    else
       emit updateCache(albumsListPath);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotPurgeCache(void)
{
    QValueList<KIPI::ImageCollection> albumsList = getAlbumsSelection();
    QStringList albumsListPath;

    for( QValueList<KIPI::ImageCollection>::ConstIterator album = albumsList.begin() ;
         album != albumsList.end() ; ++album )
        {
        if ( !albumsListPath.contains( (*album).path().path() ) )
            albumsListPath.append( (*album).path().path() );
        }

    if ( albumsListPath.isEmpty() == true )
       KMessageBox::sorry(0, i18n("You must select at least one Album for the purge cache process."));
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

    
    if ( m_interface->hasFeature( KIPI::AlbumsHaveComments ) && m_AlbumComments )
    {
        m_AlbumComments->setText( i18n("Comment: %1").arg(pitem->comments()) );
    }

    if ( m_interface->hasFeature( KIPI::AlbumsHaveCategory ) && m_AlbumCollection )
    {
        m_AlbumCollection->setText( i18n("Collection: %1")
                                    .arg(pitem->imageCollection().category()) );
    }

    if ( m_interface->hasFeature( KIPI::AlbumsHaveCreationDate ) && m_AlbumDate )
    {
        m_AlbumDate->setText( i18n("Date: %1")
                              .arg( pitem->imageCollection().date()
                                    .toString(Qt::LocalDate) ) );
    }
    
    m_AlbumItems->setText( i18n("Items: %1").arg( pitem->items() ) );

    m_albumPreview->clear();

    KURL url = pitem->imageCollection().images()[0];

    KIO::PreviewJob* thumbJob = KIO::filePreview( url, m_albumPreview->height() );

    connect(thumbJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotPreview(const KFileItem*, const QPixmap&)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotGotPreview(const KFileItem*, const QPixmap &pixmap)
{
    m_albumPreview->setPixmap(pixmap);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotOk()
{
    if (getAlbumsSelection().isEmpty() == true)
       {
       KMessageBox::sorry(0, i18n("You must selected at least one Album to find duplicate images for."));
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
