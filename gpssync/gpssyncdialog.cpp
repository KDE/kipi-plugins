/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2004-05-16
 * Description : a plugin to synchronize metadata pictures 
 *               with a GPS device.
 *
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.

#include <qlayout.h>
#include <qlabel.h>
#include <qvgroupbox.h>
#include <qgrid.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qimage.h>
#include <qfileinfo.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <klistview.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>

// LibKipi includes.

#include <libkipi/imageinfo.h>

// Local includes.

#include "gpslistviewitem.h"
#include "pluginsversion.h"
#include "gpssyncdialog.h"
#include "gpssyncdialog.moc"

namespace KIPIGPSSyncPlugin
{

GPSSyncDialog::GPSSyncDialog( KIPI::Interface* interface, QWidget* parent)
             : KDialogBase(Plain, i18n("GPS Sync"), Help|User1|Apply|Close, Close, parent, 
                           0, true, true ),
               m_interface( interface )
{
    setButtonText( User1, i18n("Load GPX File..."));
    enableButton(Apply, false);

    QGridLayout *mainLayout = new QGridLayout(plainPage(), 3, 1, 0, marginHint());

    //---------------------------------------------

    QFrame *headerFrame = new QFrame( plainPage() );
    headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed
    layout->setSpacing( 0 );
    QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
    pixmapLabelLeft->setScaledContents( false );
    layout->addWidget( pixmapLabelLeft );
    QLabel *labelTitle = new QLabel( i18n("Syncronize Picture Metadata with a GPS Device"), headerFrame, "labelTitle" );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );

    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");

    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    // --------------------------------------------------------------

    m_listView = new KListView(plainPage());
    m_listView->addColumn( i18n("Thumbnail") );
    m_listView->addColumn( i18n("Filename") );
    m_listView->addColumn( i18n("Altitude") );
    m_listView->addColumn( i18n("Latitude") );
    m_listView->addColumn( i18n("Longitude") );
    m_listView->addColumn( i18n("Date") );
    m_listView->addColumn( i18n("Extrapoled") );
    m_listView->setResizeMode(QListView::AllColumns);
    m_listView->setAllColumnsShowFocus(true);
    m_listView->setSorting(-1);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setSelectionMode(QListView::Single);
    m_listView->setMinimumWidth(450);
    mainLayout->addMultiCellWidget(headerFrame, 0, 0, 0, 1);
    mainLayout->addMultiCellWidget(m_listView, 1, 3, 0, 1);
    mainLayout->setRowStretch(1, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    QPushButton *helpButton = actionButton( Help );

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("GPS Sync"),
                                       kipiplugins_version,
                                       I18N_NOOP("A Plugin to synchronize pictures metadata with a GPS device"),
                                       KAboutData::License_GPL,
                                       "(c) 2006, Gilles Caulier",
                                       0,
                                       "http://extragear.kde.org/apps/kipi");

    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                     "caulier dot gilles at kdemail dot net");

    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("GPS Sync Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    helpButton->setPopup( helpMenu->menu() );
    readSettings();
}

GPSSyncDialog::~GPSSyncDialog()
{
}

void GPSSyncDialog::setImages( const KURL::List& images )
{
    // We only add all JPEG file only on the list because Exiv2 can't 
    // update metadata on others file formats.

    for( KURL::List::ConstIterator it = images.begin(); it != images.end(); ++it )
    {
        QFileInfo fi((*it).path());
        QString ext = fi.extension().upper();

        if (ext == QString("JPG") || ext == QString("JPEG") || ext == QString("JPE"))
            new GPSListViewItem(m_listView, m_listView->lastItem(), *it);
    }
}

// Load GPX data file.
void GPSSyncDialog::slotUser1()
{
    KURL loadGPXFile = KFileDialog::getOpenURL(KGlobalSettings::documentPath(),
                                               QString( "*.gpx" ), this,
                                               QString( i18n("Select GPX File to Load")) );
    if( loadGPXFile.isEmpty() )
       return;

    enableButton(Apply, false);
    m_gpxParser.clear();
    bool ret = m_gpxParser.loadGPXFile(loadGPXFile);

    if (!ret)
    {
        KMessageBox::error(this, i18n("Cannot parse %1 GPX file!")
                           .arg(loadGPXFile.fileName()), i18n("GPS Sync"));    
        enableButton(Apply, false);
        return;
    }

    if (m_gpxParser.numPoints() <= 0)
    {
        KMessageBox::sorry(this, i18n("The %1 GPX file do not have a date-time track to use!")
                           .arg(loadGPXFile.fileName()), i18n("GPS Sync"));    
        enableButton(Apply, false);
        return;
    }

    enableButton(Apply, true);
    matchGPSAndPhoto();
}

void GPSSyncDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("gpssync", "kipi-plugins");
}

void GPSSyncDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void GPSSyncDialog::slotClose()
{
    saveSettings();
    KDialogBase::slotClose();
}

void GPSSyncDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");

    resize(configDialogSize(config, QString("GPS Sync Dialog")));
}

void GPSSyncDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");

    saveDialogSize(config, QString("GPS Sync Dialog"));
    config.sync();
}

void GPSSyncDialog::matchGPSAndPhoto()
{
    int itemsUpdated = 0;

    QListViewItemIterator it( m_listView );
    while ( it.current() ) 
    {
        GPSListViewItem *item = (GPSListViewItem*) it.current();
        double alt =0.0, lat=0.0, lng = 0.0;
        if (m_gpxParser.parseDates(item->getDateTime(), 30, alt, lat, lng))
        {
            item->setGPSInfo(alt, lat, lng);
            itemsUpdated++;
        }
        ++it;
    }

    if (itemsUpdated == 0)
    {
        KMessageBox::sorry(this, i18n("Cannot find pictures to correlate with GPX file data."),
                           i18n("GPS Sync"));    
        enableButton(Apply, false);
        return;
    }

    KMessageBox::information(this, i18n("GPS data of %1 picture(s) have been updated on "
                             "the list using the GPX data file.\n"
                       "Press Apply button to update picture(s) metadata.")
                       .arg(itemsUpdated), i18n("GPS Sync"));    
    enableButton(Apply, true);
}

void GPSSyncDialog::slotApply()
{
    QListViewItemIterator it( m_listView );
    while ( it.current() ) 
    {
        GPSListViewItem *item = (GPSListViewItem*) it.current();
        m_listView->setSelected(item, true);
        m_listView->ensureItemVisible(item);
        item->writeGPSInfoToFile();
        ++it;
        kapp->processEvents();
    }
}

}  // NameSpace KIPIGPSSyncPlugin
