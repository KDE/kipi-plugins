//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEDIALOG.CPP
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at gmail dot com>
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
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
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
#include <khelpmenu.h>
#include <kpopupmenu.h>

// Include files for KIPI

#include <libkipi/imagecollection.h>
#include <libkipi/imagecollectionselector.h>

// Local include files

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "finddupplicatedialog.h"
#include "finddupplicatedialog.moc"

namespace KIPIFindDupplicateImagesPlugin
{

FindDuplicateDialog::FindDuplicateDialog( KIPI::Interface* interface, QWidget *parent)
                   : KDialogBase( IconList, i18n("Configure"), Help|Ok|Cancel,
                                  Ok, parent, "FindDuplicateDialog", true, false ),
                                  m_interface( interface )
{
    setCaption(i18n("Find Duplicate Images"));
    setupSelection();
    setupPageMethod();
    page_setupSelection->setFocus();
    resize( 650, 500 );

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Find Duplicate Images"),
                                           kipiplugins_version,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to find duplicate images\n"
                                                     "This plugin is based on ShowImg implementation algorithm"),
                                           "(c) 2003-2004, Gilles Caulier");

    m_about->addAuthor("Jesper K. Pedersen", I18N_NOOP("Maintainer"),
                       "blackie at kde dot org");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Original author"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor("Richard Groult", I18N_NOOP("Find duplicate images algorithm"),
                       "rgroult at jalix.org");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

FindDuplicateDialog::~FindDuplicateDialog()
{
    delete m_about;
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

    (void) new QLabel (i18n("Approximate threshold:"), groupBox1);

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
                    "to speed up the analysis of items from Albums."), groupBox2);

    QPushButton* updateCache = new QPushButton( groupBox2, "UpdateCache" );
    updateCache->setText(i18n( "&Update Cache" ));
    QWhatsThis::add( updateCache, i18n("<p>Update the cache data for the selected Albums.") );
    QPushButton* purgeCache = new QPushButton( groupBox2, "PurgeCacheAlbumsSelected" );
    purgeCache->setText(i18n( "&Purge Cache (Albums Selected)" ));
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
       KMessageBox::sorry(this, i18n("You must select at least one Album for which to find duplicate images."));
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

