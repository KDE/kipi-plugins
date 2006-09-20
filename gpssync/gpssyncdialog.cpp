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

// KDE includes.

#include <kprogress.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <klistview.h>
#include <kurlrequester.h>
#include <kconfig.h>
#include <klineedit.h>

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
             : KDialogBase(Plain, i18n("GPS Sync"), Help|Apply|Close, Close, parent, 
                           0, true, true ),
               m_interface( interface )
{
    // FIXME : handle Apply button with GPS file data parser result.
    //enableButton(Apply, false);

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
    mainLayout->addMultiCellWidget(headerFrame, 0, 0, 0, 1);

    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");

    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    // --------------------------------------------------------------

    m_listView = new KListView(plainPage());
    m_listView->addColumn( i18n("Picture") );
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

    // ---------------------------------------------------------------

    QWidget *settingsBox = new QGroupBox(0, Qt::Vertical, i18n("Settings"), plainPage());
    QGridLayout* settingsBoxLayout = new QGridLayout(settingsBox->layout(), 2, 1,
                                                     KDialog::spacingHint());

    QLabel *gpsFileLabel = new QLabel(i18n("GPS data file:"), settingsBox);
    m_gpsFile = new KURLRequester(KGlobalSettings::documentPath(), settingsBox);
    m_gpsFile->lineEdit()->setReadOnly(true);
    m_gpsFile->setMode(KFile::File | KFile::LocalOnly | KFile::ExistingOnly);    
    
    settingsBoxLayout->addMultiCellWidget(gpsFileLabel, 0, 0, 0, 1); 
    settingsBoxLayout->addMultiCellWidget(m_gpsFile, 1, 1, 0, 1); 

    // ---------------------------------------------------------------

    m_progressBar = new KProgress(plainPage());
    m_progressBar->setMaximumHeight( fontMetrics().height() );
    
    mainLayout->addMultiCellWidget(m_listView, 1, 3, 0, 0);
    mainLayout->addMultiCellWidget(settingsBox, 1, 1, 1, 1);
    mainLayout->addMultiCellWidget(m_progressBar, 2, 2, 1, 1);
    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(3, 10);
    
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

    connect(m_gpsFile, SIGNAL(urlSelected(const QString &)),
            this, SLOT(slotGPSFileSelected(const QString &)));

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
        if (QString(QImageIO::imageFormat((*it).path())).upper() == QString("JPEG"))
            new GPSListViewItem(m_listView, m_listView->lastItem(), *it);
}

void GPSSyncDialog::slotGPSFileSelected(const QString &path)
{
    // TODO: call here the GPS data file parser using GPSBabel to extract 
    // GPS data and set GPS positions to all pictures enable in the list.

    // FIXME : handle Apply button with GPS file data parser result.
    //enableButton(Apply, true);
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

void GPSSyncDialog::slotApply()
{
    m_progressBar->setTotalSteps(m_listView->childCount());
    m_progressBar->setProgress(0);
    
    QListViewItemIterator it( m_listView );
    while ( it.current() ) 
    {
        GPSListViewItem *item = (GPSListViewItem*) it.current();
        m_listView->setSelected(item, true);
        m_listView->ensureItemVisible(item);
        item->writeGPSInfoToFile();
        ++it;
        m_progressBar->advance(1);
    }
}

}  // NameSpace KIPIGPSSyncPlugin
