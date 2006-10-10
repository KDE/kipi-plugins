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

#include <qcombobox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvgroupbox.h>
#include <qgrid.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <ksqueezedtextlabel.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <klistview.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>
#include <knuminput.h>
#include <kseparator.h>

// Local includes.

#include "gpslistviewitem.h"
#include "gpsdataparser.h"
#include "pluginsversion.h"
#include "gpseditdialog.h"
#include "gpssyncdialog.h"
#include "gpssyncdialog.moc"

namespace KIPIGPSSyncPlugin
{

class GPSSyncDialogPriv
{
public:

    GPSSyncDialogPriv()
    {
        listView       = 0;
        interface      = 0;
        maxGapInput    = 0;
        gpxFileName    = 0;
        gpxPointsLabel = 0;
        timeZoneCB     = 0;
        interpolateBox = 0;
        maxTimeInput   = 0;
        maxTimeLabel   = 0;
    }

    QLabel             *gpxPointsLabel;
    QLabel             *maxTimeLabel;

    QComboBox          *timeZoneCB;

    QCheckBox          *interpolateBox;

    KListView          *listView;

    KIntSpinBox        *maxGapInput;
    KIntSpinBox        *maxTimeInput;

    KSqueezedTextLabel *gpxFileName;

    KIPI::Interface    *interface;

    GPSDataParser       gpxParser;
};

GPSSyncDialog::GPSSyncDialog( KIPI::Interface* interface, QWidget* parent)
             : KDialogBase(Plain, i18n("GPS Sync"), 
                           Help|User1|User2|User3|Apply|Close, Close, 
                           parent, 0, true, true )
{
    d = new GPSSyncDialogPriv;
    d->interface = interface;

    setButtonText(User1, i18n("Correlate"));
    setButtonText(User2, i18n("Edit..."));
    setButtonText(User3, i18n("Remove"));

    setButtonTip(User1, i18n("Correlate GPX file data with all pictures from the list."));
    setButtonTip(User2, i18n("Edit manually GPS coordinates of selected pictures form the list."));
    setButtonTip(User3, i18n("Remove GPS coordinates of selected pictures form the list."));

    enableButton(User1, false);
    enableButton(User2, true);
    enableButton(User3, true);

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
    QLabel *labelTitle = new QLabel( i18n("Syncronize Picture Metadata with a GPS Device"),
                                     headerFrame, "labelTitle" );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );

    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");

    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    // --------------------------------------------------------------

    d->listView = new KListView(plainPage());
    d->listView->addColumn( i18n("Thumbnail") );
    d->listView->addColumn( i18n("File Name") );
    d->listView->addColumn( i18n("Date Taken") );
    d->listView->addColumn( i18n("Latitude") );
    d->listView->addColumn( i18n("Longitude") );
    d->listView->addColumn( i18n("Altitude") );
    d->listView->addColumn( i18n("Status") );
    d->listView->setResizeMode(QListView::AllColumns);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setSorting(-1);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setSelectionMode(QListView::Extended);
    d->listView->setMinimumWidth(450);

    // ---------------------------------------------------------------

    QWidget *settingsBox = new QGroupBox(0, Qt::Vertical, i18n("Settings"), plainPage());
    QGridLayout* settingsBoxLayout = new QGridLayout(settingsBox->layout(), 8, 1,
                                                     KDialog::spacingHint());

    QPushButton *loadGPXButton = new QPushButton(i18n("Load GPX File..."), settingsBox);

    QLabel *gpxFileLabel = new QLabel(i18n("Current GPX file:"), settingsBox);
    d->gpxFileName       = new KSqueezedTextLabel(i18n("No GPX file"), settingsBox);
    d->gpxPointsLabel    = new QLabel(settingsBox);
    KSeparator *line     = new KSeparator(Horizontal, settingsBox);

    QLabel *maxGapLabel = new QLabel(i18n("Max gap time:"), settingsBox);
    d->maxGapInput      = new KIntSpinBox(0, 2000, 1, 30, 10, settingsBox);
    QWhatsThis::add(d->maxGapInput, i18n("<p>Set here the maximum distance in "
                    "seconds from a GPS point that a photo will be matched."));

    QLabel *timeZoneLabel = new QLabel(i18n("Time zone:"), settingsBox);
    d->timeZoneCB         = new QComboBox( false, settingsBox );
    d->timeZoneCB->insertItem(i18n("GMT-12:00"), 0);
    d->timeZoneCB->insertItem(i18n("GMT-11:00"), 1);
    d->timeZoneCB->insertItem(i18n("GMT-10:00"), 2);
    d->timeZoneCB->insertItem(i18n("GMT-09:00"), 3);
    d->timeZoneCB->insertItem(i18n("GMT-08:00"), 4);
    d->timeZoneCB->insertItem(i18n("GMT-07:00"), 5);
    d->timeZoneCB->insertItem(i18n("GMT-06:00"), 6);
    d->timeZoneCB->insertItem(i18n("GMT-05:00"), 7);
    d->timeZoneCB->insertItem(i18n("GMT-04:00"), 8);
    d->timeZoneCB->insertItem(i18n("GMT-03:00"), 9);
    d->timeZoneCB->insertItem(i18n("GMT-02:00"), 10);
    d->timeZoneCB->insertItem(i18n("GMT-01:00"), 11);
    d->timeZoneCB->insertItem(i18n("GMT"),       12);
    d->timeZoneCB->insertItem(i18n("GMT+01:00"), 13);
    d->timeZoneCB->insertItem(i18n("GMT+02:00"), 14);
    d->timeZoneCB->insertItem(i18n("GMT+03:00"), 15);
    d->timeZoneCB->insertItem(i18n("GMT+04:00"), 16);
    d->timeZoneCB->insertItem(i18n("GMT+05:00"), 17);
    d->timeZoneCB->insertItem(i18n("GMT+06:00"), 18);
    d->timeZoneCB->insertItem(i18n("GMT+07:00"), 19);
    d->timeZoneCB->insertItem(i18n("GMT+08:00"), 20);
    d->timeZoneCB->insertItem(i18n("GMT+09:00"), 21);
    d->timeZoneCB->insertItem(i18n("GMT+10:00"), 22);
    d->timeZoneCB->insertItem(i18n("GMT+11:00"), 23);
    d->timeZoneCB->insertItem(i18n("GMT+12:00"), 24);
    QWhatsThis::add(d->timeZoneCB, i18n("<p>Set here the time zone where the "
                    "pictures were taken in, so that the times of the pictures "
                    "can be adjusted to match the GPS data"));

    d->interpolateBox = new QCheckBox(i18n("Interpolate"), settingsBox);
    QWhatsThis::add(d->interpolateBox, i18n("<p>Set on this option to interpolate GPS points "
                    "witch are not matches properly with the GPX data file."));

    d->maxTimeLabel = new QLabel(i18n("Max. distance time:"), settingsBox);
    d->maxTimeInput = new KIntSpinBox(0, 240, 1, 15, 10, settingsBox);
    QWhatsThis::add(d->maxTimeInput, i18n("<p>Set here the maximum distance time in minutes "
                    "to get matched points from GPX file around a GPS point to interpolate."));

    settingsBoxLayout->addMultiCellWidget(loadGPXButton, 0, 0, 0, 1);     
    settingsBoxLayout->addMultiCellWidget(gpxFileLabel, 1, 1, 0, 1);     
    settingsBoxLayout->addMultiCellWidget(d->gpxFileName, 2, 2, 0, 1);     
    settingsBoxLayout->addMultiCellWidget(d->gpxPointsLabel, 3, 3, 0, 1);     
    settingsBoxLayout->addMultiCellWidget(line, 4, 4, 0, 1);     
    settingsBoxLayout->addMultiCellWidget(maxGapLabel, 5, 5, 0, 0); 
    settingsBoxLayout->addMultiCellWidget(d->maxGapInput, 5, 5, 1, 1); 
    settingsBoxLayout->addMultiCellWidget(timeZoneLabel, 6, 6, 0, 0); 
    settingsBoxLayout->addMultiCellWidget(d->timeZoneCB, 6, 6, 1, 1); 
    settingsBoxLayout->addMultiCellWidget(d->interpolateBox, 7, 7, 0, 1); 
    settingsBoxLayout->addMultiCellWidget(d->maxTimeLabel, 8, 8, 0, 0); 
    settingsBoxLayout->addMultiCellWidget(d->maxTimeInput, 8, 8, 1, 1); 

    // ---------------------------------------------------------------

    mainLayout->addMultiCellWidget(headerFrame, 0, 0, 0, 2);
    mainLayout->addMultiCellWidget(d->listView, 1, 3, 0, 1);
    mainLayout->addMultiCellWidget(settingsBox, 1, 1, 2, 2);
    mainLayout->setColStretch(1, 10);
    mainLayout->setRowStretch(3, 10);

    // ---------------------------------------------------------------
    // About data and help button.

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
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    connect(loadGPXButton, SIGNAL(released()),
            this, SLOT(slotLoadGPXFile()));

    connect(d->interpolateBox, SIGNAL(toggled(bool)),
            d->maxTimeLabel, SLOT(setEnabled(bool)));

    connect(d->interpolateBox, SIGNAL(toggled(bool)),
            d->maxTimeInput, SLOT(setEnabled(bool)));

    readSettings();
}

GPSSyncDialog::~GPSSyncDialog()
{
    delete d;
}

void GPSSyncDialog::setImages( const KURL::List& images )
{
    for( KURL::List::ConstIterator it = images.begin(); it != images.end(); ++it )
        new GPSListViewItem(d->listView, d->listView->lastItem(), *it);
}

void GPSSyncDialog::slotLoadGPXFile()
{
    KURL loadGPXFile = KFileDialog::getOpenURL(KGlobalSettings::documentPath(),
                                               i18n("%1|GPS Exchange Format").arg("*.gpx"), this,
                                               i18n("Select GPX File to Load") );
    if( loadGPXFile.isEmpty() )
       return;

    d->gpxParser.clear();
    bool ret = d->gpxParser.loadGPXFile(loadGPXFile);

    if (!ret)
    {
        KMessageBox::error(this, i18n("Cannot parse %1 GPX file!")
                           .arg(loadGPXFile.fileName()), i18n("GPS Sync"));    
        enableButton(User1, false);
        return;
    }

    if (d->gpxParser.numPoints() <= 0)
    {
        KMessageBox::sorry(this, i18n("The %1 GPX file do not have a date-time track to use!")
                           .arg(loadGPXFile.fileName()), i18n("GPS Sync"));    
        enableButton(User1, false);
        return;
    }

    d->gpxFileName->setText(loadGPXFile.fileName());
    d->gpxPointsLabel->setText(i18n("Points parsed: %1").arg(d->gpxParser.numPoints()));
    enableButton(User1, true);
    slotUser1();
}

void GPSSyncDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("gpssync", "kipi-plugins");
}

void GPSSyncDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    if (!promptUserClose()) 
    {
        e->ignore();
        return;
    }

    saveSettings();
    e->accept();
}

void GPSSyncDialog::slotClose()
{
    if (!promptUserClose()) return;
    saveSettings();
    KDialogBase::slotClose();
}

bool GPSSyncDialog::promptUserClose()
{
    // Check if one item is dirty in the list.

    QListViewItemIterator it( d->listView );
    int dirty = 0;

    while ( it.current() ) 
    {
        GPSListViewItem *item = (GPSListViewItem*) it.current();
        if (item->isDirty())
            dirty++;
        ++it;
    }

    if (dirty > 0)
    {
        QString msg = i18n("1 picture from the list isn't updated.",
                           "%n pictures from the list isn't updated.", dirty);

        if (KMessageBox::No == KMessageBox::warningYesNo(this,
                     i18n("<p>%1\n"
                          "Do you want really to close this window without applying changes?</p>")
                          .arg(msg)))
            return false;
    }

    return true;
}

void GPSSyncDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");
    d->maxGapInput->setValue(config.readNumEntry("Max Gap Time", 30));
    d->timeZoneCB->setCurrentItem(config.readNumEntry("Time Zone", 12));
    d->interpolateBox->setChecked(config.readBoolEntry("Interpolate", false));
    d->maxTimeInput->setValue(config.readNumEntry("Max Inter Dist Time", 15));
    
    d->maxTimeLabel->setEnabled(d->interpolateBox->isChecked());
    d->maxTimeInput->setEnabled(d->interpolateBox->isChecked());
    resize(configDialogSize(config, QString("GPS Sync Dialog")));
}

void GPSSyncDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");
    config.writeEntry("Max Gap Time", d->maxGapInput->value() );
    config.writeEntry("Time Zone", d->timeZoneCB->currentItem() );
    config.writeEntry("Interpolate", d->interpolateBox->isChecked() );
    config.writeEntry("Max Inter Dist Time", d->maxTimeInput->value() );
    saveDialogSize(config, QString("GPS Sync Dialog"));
    config.sync();
}

// Correlate the GPS positions from Pictures using a GPX file data.
void GPSSyncDialog::slotUser1()
{
    int itemsUpdated = 0;

    QListViewItemIterator it( d->listView );
    while ( it.current() ) 
    {
        GPSListViewItem *item = (GPSListViewItem*) it.current();
        GPSDataContainer gpsData;

        if (d->gpxParser.matchDate(item->getDateTime(), 
                                   d->maxGapInput->value(),
                                   d->timeZoneCB->currentItem()-12,
                                   d->interpolateBox->isChecked(),
                                   d->maxTimeInput->value()*60, 
                                   gpsData))
        {
            item->setGPSInfo(gpsData);
            itemsUpdated++;
        }
        ++it;
    }

    if (itemsUpdated == 0)
    {
        KMessageBox::sorry(this, i18n("Cannot find pictures to correlate with GPX file data."),
                           i18n("GPS Sync"));    
        return;
    }

    QString msg = i18n("GPS data of 1 picture have been updated on the list using the GPX data file.\n"
                       "Press <b>Apply</b> button to update picture metadata.",
                       "GPS data of %n pictures have been updated on the list using the GPX data file."
                       "Press <b>Apply</b> button to update pictures metadata.",
                       itemsUpdated);

    KMessageBox::information(this, msg, i18n("GPS Sync"));    
}

// Start the GPS coordinates editor dialog.
void GPSSyncDialog::slotUser2()
{
    if (!d->listView->currentItem())
    {
        KMessageBox::information(this, i18n("Please, select at least one picture from "
                     "the list to edit GPS coordinates manually."), i18n("GPS Sync"));    
        return;
    }

    GPSListViewItem* item = (GPSListViewItem*)d->listView->currentItem();

    GPSEditDialog dlg(this, item->getGPSInfo(), 
                      item->getUrl().fileName(),
                      item->hasGPSInfo());

    if (dlg.exec() == KDialogBase::Accepted)
    {
        QListViewItemIterator it(d->listView);

        while (it.current())
        {
            if (it.current()->isSelected())
            {
                GPSListViewItem *selItem = (GPSListViewItem*)it.current();
                selItem->setGPSInfo(dlg.getGPSInfo(), true, true);
            }
            ++it;
        }
    }
}

// Remove GPS coordinates from pictures.
void GPSSyncDialog::slotUser3()
{
    if (!d->listView->currentItem())
    {
        KMessageBox::information(this, i18n("Please, select at least one picture from "
                     "the list to remove GPS coordinates."), i18n("GPS Sync"));    
        return;
    }

    QListViewItemIterator it(d->listView);

    while (it.current())
    {
        if (it.current()->isSelected())
        {
            GPSListViewItem *selItem = (GPSListViewItem*)it.current();
            selItem->eraseGPSInfo();
        }
        ++it;
    }
}

void GPSSyncDialog::slotApply()
{
    KURL::List images;

    QListViewItemIterator it( d->listView );
    while ( it.current() ) 
    {
        GPSListViewItem *item = (GPSListViewItem*) it.current();
        d->listView->setSelected(item, true);
        d->listView->ensureItemVisible(item);
        item->writeGPSInfoToFile();
        images.append(item->getUrl());

        // TODO : new libkipi method to store GPS info in host database.

        ++it;
        kapp->processEvents();
    }
    
    d->interface->refreshImages(images);
}

}  // NameSpace KIPIGPSSyncPlugin
