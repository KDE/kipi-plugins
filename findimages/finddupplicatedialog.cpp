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
#include <klistview.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

// Include files for KIPI

#include <libkipi/version.h>
#include <libkipi/imagecollection.h>
#include <libkipi/imagecollectionselector.h>

// Local include files

#include "finddupplicatedialog.h"

namespace KIPIFindDupplicateImagesPlugin
{

FindDuplicateDialog::FindDuplicateDialog( KIPI::Interface* interface, QWidget *parent)
                   : KDialogBase( IconList, i18n("Configure"), Help|Ok|Cancel,
                                  Ok, parent, "FindDuplicateDialog", true, true ),
                                  m_interface( interface )
{
    setCaption(i18n("Find Duplicate Images"));
    setupSelection();
    setupPageMethod();
    page_setupSelection->setFocus();
    resize( 650, 500 );

    // About data and help button.

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Find Duplicate Images"),
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin to find duplicate images\n"
                                                 "This plugin is based on ShowImg implementation algorithm"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Gilles Caulier",
                                       0,
                                       "http://extragear.kde.org/apps/kipi.php");

    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at free.fr");

    about->addAuthor("Richard Groult", I18N_NOOP("Find duplicate images algorithm"),
                     "rgroult at jalix.org");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Find Duplicate Images Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );
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
    m_imageCollectionSelector = new KIPI::ImageCollectionSelector(page_setupSelection, m_interface);
    layout->addWidget(m_imageCollectionSelector);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::setupPageMethod(void)
{
    QString whatsThis;
    page_setupMethod = addPage( i18n("Method & Cache"), i18n("Find-Duplicates Method & Cache Configuration"),
                                BarIcon("run", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupMethod, 0, spacingHint() );

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( 2, Qt::Horizontal, i18n("Method"), page_setupMethod );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );

    QLabel *m_labelsearchMethod = new QLabel( i18n("Search method:"), groupBox1 );
    m_findMethod = new QComboBox(false, groupBox1);
    m_findMethod->insertItem(i18n("Almost"), MethodAlmost);
    m_findMethod->insertItem(i18n("Fast"), MethodFast);
    m_findMethod->setCurrentItem ( MethodAlmost );
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
                    "to speed up the analysis of Albums items."), groupBox2);

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

    connect(m_findMethod, SIGNAL(activated(int)),
            this, SLOT(slotfindMethodChanged(int)));

    connect(updateCache, SIGNAL(clicked()),
            this, SLOT(slotUpdateCache()));

    connect(purgeCache, SIGNAL(clicked()),
            this, SLOT(slotPurgeCache()));

    connect(purgeAllCache, SIGNAL(clicked()),
            this, SLOT(slotPurgeAllCache()));

    slotfindMethodChanged(m_findMethod->currentItem());
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("findduplicateimages",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotfindMethodChanged(int index)
{
    if ( index == MethodAlmost )
       m_approximateThreshold->setEnabled(true);
    else
       m_approximateThreshold->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotUpdateCache(void)
{
    QValueList<KIPI::ImageCollection> albumsList = getSelectedAlbums();
    QStringList albumsListPath;

    for( QValueList<KIPI::ImageCollection>::ConstIterator album = albumsList.begin() ;
         album != albumsList.end() ; ++album )
        {
        if ( !albumsListPath.contains( (*album).path().path() ) )
            albumsListPath.append( (*album).path().path() );
        }

    if ( albumsListPath.isEmpty() == true )
       KMessageBox::sorry(this, i18n("You must select at least one Album for the update cache process."));
    else
       emit updateCache(albumsListPath);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotPurgeCache(void)
{
    QValueList<KIPI::ImageCollection> albumsList = getSelectedAlbums();

    QStringList albumsListPath;

    for( QValueList<KIPI::ImageCollection>::ConstIterator album = albumsList.begin() ;
         album != albumsList.end() ; ++album )
        {
        if ( !albumsListPath.contains( (*album).path().path() ) )
            albumsListPath.append( (*album).path().path() );
        }

    if ( albumsListPath.isEmpty() == true )
       KMessageBox::sorry(this, i18n("You must select at least one Album for the purge cache process."));
    else
       emit clearCache(albumsListPath);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotPurgeAllCache(void)
{
     emit clearAllCache();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::slotOk()
{
    if (getSelectedAlbums().isEmpty() == true)
       {
       KMessageBox::sorry(this, i18n("You must selected at least one Album to find duplicate images for."));
       return;
       }

    accept();
}


/////////////////////////////////////////////////////////////////////////////////////////////

QValueList<KIPI::ImageCollection> FindDuplicateDialog::getSelectedAlbums() const
{
    return m_imageCollectionSelector->selectedImageCollections();
}


/////////////////////////////////////////////////////////////////////////////////////////////

int FindDuplicateDialog::getFindMethod() const
{
    return m_findMethod->currentItem();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateDialog::setFindMethod(int method)
{
    return m_findMethod->setCurrentItem( method );
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
