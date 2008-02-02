/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with 
 *               a GPS device.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
#include <kio/previewjob.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "gpslistviewitem.h"
#include "gpsdataparser.h"
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
        about          = 0;
    }

    QLabel                   *gpxPointsLabel;
    QLabel                   *maxTimeLabel;

    QComboBox                *timeZoneCB;

    QCheckBox                *interpolateBox;

    KListView                *listView;

    KIntSpinBox              *maxGapInput;
    KIntSpinBox              *maxTimeInput;

    KSqueezedTextLabel       *gpxFileName;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about; 

    GPSDataParser             gpxParser;
};

GPSSyncDialog::GPSSyncDialog( KIPI::Interface* interface, QWidget* parent)
             : KDialogBase(Plain, i18n("Geolocalization"), 
                           Help|User1|User2|User3|Apply|Close, Close, 
                           parent, 0, true, false)
{
    d = new GPSSyncDialogPriv;
    d->interface = interface;

    setButtonText(User1, i18n("Correlate"));
    setButtonText(User2, i18n("Edit..."));
    setButtonText(User3, i18n("Remove"));

    setButtonTip(User1, i18n("Correlate in time and interpolate distance of data from GPX file with all images on the list."));
    setButtonTip(User2, i18n("Manually edit GPS coordinates of selected images from the list."));
    setButtonTip(User3, i18n("Remove GPS coordinates of selected images from the list."));

    enableButton(User1, false);
    enableButton(User2, true);
    enableButton(User3, true);

    QGridLayout *mainLayout = new QGridLayout(plainPage(), 3, 1, 0, marginHint());

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

    QGroupBox *settingsBox         = new QGroupBox(0, Qt::Vertical, i18n("Settings"), plainPage());
    QGridLayout *settingsBoxLayout = new QGridLayout(settingsBox->layout(), 8, 1,
                                                     KDialog::spacingHint());

    QPushButton *loadGPXButton = new QPushButton(i18n("Load GPX File..."), settingsBox);

    QLabel *gpxFileLabel = new QLabel(i18n("Current GPX file:"), settingsBox);
    d->gpxFileName       = new KSqueezedTextLabel(i18n("No GPX file"), settingsBox);
    d->gpxPointsLabel    = new QLabel(settingsBox);
    KSeparator *line     = new KSeparator(Horizontal, settingsBox);

    QLabel *maxGapLabel = new QLabel(i18n("Max. time gap (sec.):"), settingsBox);
    d->maxGapInput      = new KIntSpinBox(0, 1000000, 1, 30, 10, settingsBox);
    QWhatsThis::add(d->maxGapInput, i18n("<p>Sets the maximum difference in "
                    "seconds from a GPS track point to the image time to be matched. "
                    "If the time difference exceeds this setting, no match takes place."));

    QLabel *timeZoneLabel = new QLabel(i18n("Time zone:"), settingsBox);
    d->timeZoneCB         = new QComboBox( false, settingsBox );

    // See list of time zomes over the world :
    // http://en.wikipedia.org/wiki/List_of_time_zones
    // NOTE: Combo box strings are not i18n.
    d->timeZoneCB->insertItem("GMT-12:00");
    d->timeZoneCB->insertItem("GMT-11:00");
    d->timeZoneCB->insertItem("GMT-10:00");
    d->timeZoneCB->insertItem("GMT-09:30");
    d->timeZoneCB->insertItem("GMT-09:00");
    d->timeZoneCB->insertItem("GMT-08:00");
    d->timeZoneCB->insertItem("GMT-07:00");
    d->timeZoneCB->insertItem("GMT-06:00");
    d->timeZoneCB->insertItem("GMT-05:30");
    d->timeZoneCB->insertItem("GMT-05:00");
    d->timeZoneCB->insertItem("GMT-04:30");
    d->timeZoneCB->insertItem("GMT-04:00");
    d->timeZoneCB->insertItem("GMT-03:30");
    d->timeZoneCB->insertItem("GMT-03:00");
    d->timeZoneCB->insertItem("GMT-02:00");
    d->timeZoneCB->insertItem("GMT-01:00");
    d->timeZoneCB->insertItem("GMT+00:00");
    d->timeZoneCB->insertItem("GMT+01:00");
    d->timeZoneCB->insertItem("GMT+02:00");
    d->timeZoneCB->insertItem("GMT+03:00");
    d->timeZoneCB->insertItem("GMT+03:30");
    d->timeZoneCB->insertItem("GMT+04:00");
    d->timeZoneCB->insertItem("GMT+05:00");
    d->timeZoneCB->insertItem("GMT+05:30");    // See B.K.O # 149491
    d->timeZoneCB->insertItem("GMT+05:45");
    d->timeZoneCB->insertItem("GMT+06:00");
    d->timeZoneCB->insertItem("GMT+06:30");
    d->timeZoneCB->insertItem("GMT+07:00");
    d->timeZoneCB->insertItem("GMT+08:00");
    d->timeZoneCB->insertItem("GMT+08:45");
    d->timeZoneCB->insertItem("GMT+09:00");
    d->timeZoneCB->insertItem("GMT+09:30");
    d->timeZoneCB->insertItem("GMT+10:00");
    d->timeZoneCB->insertItem("GMT+10:30");
    d->timeZoneCB->insertItem("GMT+11:00");
    d->timeZoneCB->insertItem("GMT+11:30");
    d->timeZoneCB->insertItem("GMT+12:00");
    d->timeZoneCB->insertItem("GMT+12:45");
    d->timeZoneCB->insertItem("GMT+13:00");
    d->timeZoneCB->insertItem("GMT+14:00");
    QWhatsThis::add(d->timeZoneCB, i18n("<p>Sets the time zone of the camera during "
                    "photo shooting, so that the time stamps of the images "
                    "can be converted to GMT to match the GPS time reference"));

    d->interpolateBox = new QCheckBox(i18n("Interpolate"), settingsBox);
    QWhatsThis::add(d->interpolateBox, i18n("<p>Set this option to interpolate GPS track points "
                    "which are not closely matched to the GPX data file."));

    d->maxTimeLabel = new QLabel(i18n("Difference in min.:"), settingsBox);
    d->maxTimeInput = new KIntSpinBox(0, 240, 1, 15, 10, settingsBox);
    QWhatsThis::add(d->maxTimeInput, i18n("<p>Sets the maximum time difference in minutes (240 max.)"
                    " to interpolate GPX file points to image time data."));

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

    mainLayout->addMultiCellWidget(d->listView, 0, 2, 0, 1);
    mainLayout->addMultiCellWidget(settingsBox, 0, 1, 2, 2);
    mainLayout->setColStretch(1, 10);
    mainLayout->setRowStretch(2, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(I18N_NOOP("GPS Sync"),
                                            NULL,
                                            KAboutData::License_GPL,
                                            I18N_NOOP("A plugin to synchronize images' metadata with a GPS device"),
                                            "(c) 2006-2007, Gilles Caulier");

    d->about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                        "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
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
    delete d->about;
    delete d;
}

void GPSSyncDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("gpssync", "kipi-plugins");
}

void GPSSyncDialog::setImages( const KURL::List& images )
{
    for( KURL::List::ConstIterator it = images.begin(); it != images.end(); ++it )
        new GPSListViewItem(d->listView, d->listView->lastItem(), *it);

    KIO::PreviewJob *thumbnailJob = KIO::filePreview(images, 64);

    connect(thumbnailJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            this, SLOT(slotGotThumbnail(const KFileItem*, const QPixmap&)));
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
        QString msg = i18n("1 image from the list is not updated.",
                           "%n images from the list are not updated.", dirty);

        if (KMessageBox::No == KMessageBox::warningYesNo(this,
                     i18n("<p>%1\n"
                          "Do you really want to close this window without applying changes?</p>")
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
    d->timeZoneCB->setCurrentItem(config.readNumEntry("Time Zone", 16));  // GMT+00:00
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
        GPSListViewItem *item = dynamic_cast<GPSListViewItem*>(it.current());
        GPSDataContainer gpsData;
        QString tz = d->timeZoneCB->currentText();
        int hh     = QString(QString(tz[4])+QString(tz[5])).toInt();
        int mm     = QString(QString(tz[7])+QString(tz[8])).toInt();
        int offset = hh*3600 + mm*60;

        if (tz[3] == QChar('-'))
            offset = (-1)*offset;

        if (d->gpxParser.matchDate(item->dateTime(), 
                                   d->maxGapInput->value(),
                                   offset,
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

    QString msg = i18n("GPS data of 1 image have been updated on the list using the GPX data file.",
                       "GPS data of %n images have been updated on the list using the GPX data file.",
                       itemsUpdated);
    msg += '\n';
    msg += i18n("Press Apply button to update images metadata.");

    KMessageBox::information(this, msg, i18n("GPS Sync"));    
}

// Start the GPS coordinates editor dialog.
void GPSSyncDialog::slotUser2()
{
    if (!d->listView->currentItem())
    {
        KMessageBox::information(this, i18n("Please, select at least one image from "
                     "the list to edit GPS coordinates manually."), i18n("GPS Sync"));    
        return;
    }

    GPSListViewItem *item = dynamic_cast<GPSListViewItem*>(d->listView->currentItem());

    GPSEditDialog dlg(this, item->GPSInfo(), 
                      item->url().fileName(),
                      item->hasGPSInfo());

    if (dlg.exec() == KDialogBase::Accepted)
    {
        QListViewItemIterator it(d->listView);

        while (it.current())
        {
            if (it.current()->isSelected())
            {
                GPSListViewItem *selItem = dynamic_cast<GPSListViewItem*>(it.current());
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
        KMessageBox::information(this, i18n("Please, select at least one image from "
                     "the list to remove GPS coordinates."), i18n("GPS Sync"));    
        return;
    }

    QListViewItemIterator it(d->listView);

    while (it.current())
    {
        if (it.current()->isSelected())
        {
            GPSListViewItem *selItem = dynamic_cast<GPSListViewItem*>(it.current());
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
        GPSListViewItem *selItem = dynamic_cast<GPSListViewItem*>(it.current());
        d->listView->setSelected(selItem, true);
        d->listView->ensureItemVisible(selItem);
        selItem->writeGPSInfoToFile();
        images.append(selItem->url());
        ++it;
        kapp->processEvents();
    }

    // We use kipi interface refreshImages() method to tell to host than 
    // metadata from pictures have changed and need to be re-readed.

    d->interface->refreshImages(images);
}

void GPSSyncDialog::slotGotThumbnail(const KFileItem *item, const QPixmap& pix)
{
    QListViewItemIterator it(d->listView);

    while (it.current())
    {
        GPSListViewItem *selItem = dynamic_cast<GPSListViewItem*>(it.current());
        if (selItem->url() == item->url())
        {
            selItem->setPixmap(0, pix);
        }
        ++it;
    }
}

}  // NameSpace KIPIGPSSyncPlugin
