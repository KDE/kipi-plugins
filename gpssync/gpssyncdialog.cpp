/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with
 *               a GPS device.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "gpssyncdialog.h"
#include "gpssyncdialog.moc"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include <QRadioButton>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kseparator.h>
#include <ksqueezedtextlabel.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// Local includes

#include "gpsdataparser.h"
#include "gpseditdialog.h"
#include "gpslistviewitem.h"
#include "imageslist.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "gpslistviewcontextmenu.h"
#include "gpsbookmarkowner.h"

namespace KIPIGPSSyncPlugin
{

class GPSSyncDialogPriv
{
public:

    GPSSyncDialogPriv()
    : gpxPointsLabel(0),
      maxTimeLabel(0),
      timeZoneGroup(0),
      timeZoneSystem(0),
      timeZoneManual(0),
      timeZoneCB(0),
      offsetSign(0),
      offsetMin(0),
      offsetSec(0),
      interpolateBox(0),
      maxGapInput(0),
      maxTimeInput(0),
      gpxFileName(0),
      interface(0),
      about(0),
      imagesList(0),
      gpxParser(),
      gpxFileOpenLastDirectory(KGlobalSettings::documentPath())
    {
    }

    QLabel                   *gpxPointsLabel;
    QLabel                   *maxTimeLabel;

    QButtonGroup             *timeZoneGroup;
    QRadioButton             *timeZoneSystem;
    QRadioButton             *timeZoneManual;
    KComboBox                *timeZoneCB;
    KComboBox                *offsetSign;
    KIntSpinBox              *offsetMin;
    KIntSpinBox              *offsetSec;

    QCheckBox                *interpolateBox;

    KIntSpinBox              *maxGapInput;
    KIntSpinBox              *maxTimeInput;

    KSqueezedTextLabel       *gpxFileName;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;
    KIPIPlugins::ImagesList  *imagesList;

    GPSDataParser             gpxParser;
    KUrl                      gpxFileOpenLastDirectory;

    GPSBookmarkOwner         *bookmarkOwner;
};

GPSSyncDialog::GPSSyncDialog(KIPI::Interface* interface, QWidget* parent)
             : KDialog(parent), d(new GPSSyncDialogPriv)
{
    d->interface = interface;

    setButtons(Help|User1|User2|User3|Apply|Close);
    setDefaultButton(Close);
    setCaption(i18n("Geolocation"));
    setModal(true);

    setButtonText(User1, i18n("Correlate"));
    setButtonText(User2, i18n("Edit..."));
    setButtonText(User3, i18n("Remove"));

    setButtonToolTip(User1, i18n("Correlate in time and interpolate distance of data from GPX file with all images on the list."));
    setButtonToolTip(User2, i18n("Manually edit GPS coordinates of selected images from the list."));
    setButtonToolTip(User3, i18n("Remove GPS coordinates of selected images from the list."));

    enableButton(User1, false);
    enableButton(User2, true);
    enableButton(User3, true);

    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QGridLayout *mainLayout = new QGridLayout(page);

    // --------------------------------------------------------------

    d->imagesList = new KIPIPlugins::ImagesList(d->interface, this);
    d->imagesList->setControlButtonsPlacement(KIPIPlugins::ImagesList::NoControlButtons);
    d->imagesList->enableDragAndDrop(false);
    d->imagesList->setAllowRAW(true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User1,
                                       i18n("Date"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User2,
                                       i18n("Latitude"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User3,
                                       i18n("Longitude"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User4,
                                       i18n("Altitude"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User5,
                                       i18n("Status"), true);
    d->imagesList->setMinimumWidth(450);

    d->bookmarkOwner = new GPSBookmarkOwner(this);
    
    // add the context menu provider to the imagesList:
    new GPSListViewContextMenu(d->imagesList, d->bookmarkOwner);

//    d->listView->setIconSize(QSize(64, 64));
//    d->listView->setColumnWidth(0, 70);

    // ---------------------------------------------------------------

    QGroupBox *settingsBox         = new QGroupBox(i18n("Settings"), page);
    QGridLayout *settingsBoxLayout = new QGridLayout(settingsBox);

    QPushButton *loadGPXButton = new QPushButton(i18n("Load GPX File..."), settingsBox);

    QLabel *gpxFileLabel = new QLabel(i18n("Current GPX file:"), settingsBox);
    d->gpxFileName       = new KSqueezedTextLabel(i18n("No GPX file"), settingsBox);
    d->gpxPointsLabel    = new QLabel(settingsBox);
    KSeparator *line     = new KSeparator(Qt::Horizontal, settingsBox);

    QLabel *maxGapLabel = new QLabel(i18n("Max. time gap (sec.):"), settingsBox);
    d->maxGapInput      = new KIntSpinBox(0, 1000000, 1, 30, settingsBox);
    d->maxGapInput->setWhatsThis(i18n("Sets the maximum difference in "
                    "seconds from a GPS track point to the image time to be matched. "
                    "If the time difference exceeds this setting, no match will be attempted."));

    QLabel *timeZoneLabel = new QLabel(i18n("Camera time zone:"), settingsBox);
    d->timeZoneSystem     = new QRadioButton(i18n("Same as system"), settingsBox);
    d->timeZoneSystem->setWhatsThis(i18n(
                    "Use this option if the timezone of the camera "
                    "is the same as the timezone of this system. "
                    "The conversion to GMT will be done automatically."));
    d->timeZoneManual     = new QRadioButton(i18nc("manual time zone selection for gps syncing", "Manual:"), settingsBox);
    d->timeZoneManual->setWhatsThis(i18n(
                    "Use this option if the timezone of the camera "
                    "is different from this system and you have to "
                    "specify the difference to GMT manually."));
    d->timeZoneGroup = new QButtonGroup(settingsBox);
    d->timeZoneGroup->addButton(d->timeZoneSystem, 1);
    d->timeZoneGroup->addButton(d->timeZoneManual, 2);
    
    d->timeZoneCB         = new KComboBox(settingsBox);

    // See list of time zones over the world :
    // http://en.wikipedia.org/wiki/List_of_time_zones
    // NOTE: Combobox strings are not i18n.
    d->timeZoneCB->addItem("GMT-12:00");
    d->timeZoneCB->addItem("GMT-11:00");
    d->timeZoneCB->addItem("GMT-10:00");
    d->timeZoneCB->addItem("GMT-09:30");
    d->timeZoneCB->addItem("GMT-09:00");
    d->timeZoneCB->addItem("GMT-08:00");
    d->timeZoneCB->addItem("GMT-07:00");
    d->timeZoneCB->addItem("GMT-06:00");
    d->timeZoneCB->addItem("GMT-05:30");
    d->timeZoneCB->addItem("GMT-05:00");
    d->timeZoneCB->addItem("GMT-04:30");
    d->timeZoneCB->addItem("GMT-04:00");
    d->timeZoneCB->addItem("GMT-03:30");
    d->timeZoneCB->addItem("GMT-03:00");
    d->timeZoneCB->addItem("GMT-02:00");
    d->timeZoneCB->addItem("GMT-01:00");
    d->timeZoneCB->addItem("GMT+00:00");
    d->timeZoneCB->addItem("GMT+01:00");
    d->timeZoneCB->addItem("GMT+02:00");
    d->timeZoneCB->addItem("GMT+03:00");
    d->timeZoneCB->addItem("GMT+03:30");
    d->timeZoneCB->addItem("GMT+04:00");
    d->timeZoneCB->addItem("GMT+05:00");
    d->timeZoneCB->addItem("GMT+05:30");    // See B.K.O # 149491
    d->timeZoneCB->addItem("GMT+05:45");
    d->timeZoneCB->addItem("GMT+06:00");
    d->timeZoneCB->addItem("GMT+06:30");
    d->timeZoneCB->addItem("GMT+07:00");
    d->timeZoneCB->addItem("GMT+08:00");
    d->timeZoneCB->addItem("GMT+08:45");
    d->timeZoneCB->addItem("GMT+09:00");
    d->timeZoneCB->addItem("GMT+09:30");
    d->timeZoneCB->addItem("GMT+10:00");
    d->timeZoneCB->addItem("GMT+10:30");
    d->timeZoneCB->addItem("GMT+11:00");
    d->timeZoneCB->addItem("GMT+11:30");
    d->timeZoneCB->addItem("GMT+12:00");
    d->timeZoneCB->addItem("GMT+12:45");
    d->timeZoneCB->addItem("GMT+13:00");
    d->timeZoneCB->addItem("GMT+14:00");
    d->timeZoneCB->setWhatsThis(i18n("<p>Sets the time zone the camera was set to "
                    "during photo shooting, so that the time stamps of the images "
                    "can be converted to GMT to match the GPS time reference.</p>"
                    "<p>Note: positive offsets count eastwards from zero longitude (GMT), "
                    "they are 'ahead of time'.</p>"));

    // additional camera offset to respect
    QLabel *offsetLabel = new QLabel(i18n("Fine offset (mm:ss):"), settingsBox);
    offsetLabel->setWhatsThis(i18n(
                        "Sets an additional offset in minutes and "
                        "seconds that is used to correlate the photos "
                        "to the GPS track. "
                        "This can be used for fine tuning to adjust a "
                        "wrong camera clock."));

    QWidget *offsetWidget = new QWidget(settingsBox);
    d->offsetSign = new KComboBox(offsetWidget);
    d->offsetSign->addItem("+");
    d->offsetSign->addItem("-");
    d->offsetSign->setWhatsThis(i18n("Set whether the camera offset "
        "is negative or positive."));

    d->offsetMin = new KIntSpinBox(0, 59, 1, 0, offsetWidget);
    d->offsetMin->setWhatsThis(i18n("Minutes to fine tune camera offset."));

    d->offsetSec = new KIntSpinBox(0, 59, 1, 0, offsetWidget);
    d->offsetSec->setWhatsThis(i18n("Seconds to fine tune camera offset."));

    QGridLayout *offsetLayout = new QGridLayout(offsetWidget);
    offsetLayout->addWidget(d->offsetSign, 0, 0, 1, 1);
    offsetLayout->addWidget(d->offsetMin, 0, 1, 1, 1);
    offsetLayout->addWidget(d->offsetSec, 0, 2, 1, 1);
    offsetLayout->setSpacing(spacingHint());
    offsetLayout->setMargin(spacingHint());

    // interpolation options
    d->interpolateBox = new QCheckBox(i18n("Interpolate"), settingsBox);
    d->interpolateBox->setWhatsThis(i18n("Set this option to interpolate GPS track points "
                    "which are not closely matched to the GPX data file."));

    d->maxTimeLabel = new QLabel(i18n("Difference in min.:"), settingsBox);
    d->maxTimeInput = new KIntSpinBox(0, 240, 1, 15, settingsBox);
    d->maxTimeInput->setWhatsThis(i18n("Sets the maximum time difference in minutes (240 max.)"
                    " to interpolate GPX file points to image time data."));

    // layout form
    int row = 0;
    settingsBoxLayout->addWidget(loadGPXButton,     row, 0, 1, 2);
    row++;
    settingsBoxLayout->addWidget(gpxFileLabel,      row, 0, 1, 2);
    row++;
    settingsBoxLayout->addWidget(d->gpxFileName,    row, 0, 1, 2);
    row++;
    settingsBoxLayout->addWidget(d->gpxPointsLabel, row, 0, 1, 2);
    row++;
    settingsBoxLayout->addWidget(line,              row, 0, 1, 2);
    row++;
    settingsBoxLayout->addWidget(maxGapLabel,       row, 0, 1, 1);
    settingsBoxLayout->addWidget(d->maxGapInput,    row, 1, 1, 1);
    row++;
    settingsBoxLayout->addWidget(timeZoneLabel,     row, 0, 1, 2);
    row++;
    settingsBoxLayout->addWidget(d->timeZoneSystem, row, 0, 1, 2);
    row++;
    settingsBoxLayout->addWidget(d->timeZoneManual, row, 0, 1, 1);
    settingsBoxLayout->addWidget(d->timeZoneCB,     row, 1, 1, 1);
    row++;
    settingsBoxLayout->addWidget(offsetLabel,       row, 0, 1, 1);
    settingsBoxLayout->addWidget(offsetWidget,      row, 1, 1, 1);
    row++;
    settingsBoxLayout->addWidget(d->interpolateBox, row, 0, 1, 2);
    row++;
    settingsBoxLayout->addWidget(d->maxTimeLabel,   row, 0, 1, 1);
    settingsBoxLayout->addWidget(d->maxTimeInput,   row, 1, 1, 1);
    settingsBoxLayout->setSpacing(spacingHint());
    settingsBoxLayout->setMargin(spacingHint());

    // ---------------------------------------------------------------

    mainLayout->addWidget(d->imagesList, 0, 0, 3, 2);
    mainLayout->addWidget(settingsBox, 0, 2, 2, 1);
    mainLayout->setColumnStretch(1, 10);
    mainLayout->setRowStretch(2, 10);
    mainLayout->setSpacing(spacingHint());
    mainLayout->setMargin(0);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("GPS Sync"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Plugin to synchronize pictures' metadata with a GPS device"),
                   ki18n("(c) 2006-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"),
                        ki18n("Developer and maintainer"),
                              "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ---------------------------------------------------------------

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotApply()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotUser1Correlate()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotUser2EditCoordinates()));

    connect(this, SIGNAL(user3Clicked()),
            this, SLOT(slotUser3RemoveCoordinates()));

    connect(loadGPXButton, SIGNAL(released()),
            this, SLOT(slotLoadGPXFile()));

    connect(d->interpolateBox, SIGNAL(toggled(bool)),
            d->maxTimeLabel, SLOT(setEnabled(bool)));

    connect(d->interpolateBox, SIGNAL(toggled(bool)),
            d->maxTimeInput, SLOT(setEnabled(bool)));

    connect(d->timeZoneGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(slotTimeZoneModeChanged(int)));

    readSettings();
}

GPSSyncDialog::~GPSSyncDialog()
{
    delete d->about;
    delete d;
}

void GPSSyncDialog::slotHelp()
{
    KToolInvocation::invokeHelp("gpssync", "kipi-plugins");
}

void GPSSyncDialog::setImages( const KUrl::List& images )
{
    for( KUrl::List::ConstIterator it = images.begin(); it != images.end(); ++it )
        new GPSListViewItem(d->interface, d->imagesList->listView(), *it);

    d->interface->thumbnails(images, 64);
}

void GPSSyncDialog::slotLoadGPXFile()
{
    KUrl loadGPXFile = KFileDialog::getOpenUrl(d->gpxFileOpenLastDirectory,
                                               i18n("%1|GPS Exchange Format", QString("*.gpx")), this,
                                               i18n("Select GPX File to Load") );
    if( loadGPXFile.isEmpty() )
       return;
    
    d->gpxFileOpenLastDirectory = loadGPXFile.upUrl();

    d->gpxParser.clear();
    bool ret = d->gpxParser.loadGPXFile(loadGPXFile);

    if (!ret)
    {
        KMessageBox::error(this, i18n("Cannot parse %1 GPX file.",
                           loadGPXFile.fileName()), i18n("GPS Sync"));
        enableButton(User1, false);
        return;
    }

    if (d->gpxParser.numPoints() <= 0)
    {
        KMessageBox::sorry(this, i18n("The %1 GPX file does not have a date-time track to use.",
                           loadGPXFile.fileName()), i18n("GPS Sync"));
        enableButton(User1, false);
        return;
    }

    d->gpxFileName->setText(loadGPXFile.fileName());
    d->gpxPointsLabel->setText(i18n("Points parsed: %1", d->gpxParser.numPoints()));
    enableButton(User1, true);
    slotUser1Correlate();
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

bool GPSSyncDialog::promptUserClose()
{
    // Check if one item is dirty in the list.

    int dirty             = 0;
    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = d->imagesList->listView()->topLevelItem(i);
        GPSListViewItem *lvItem = dynamic_cast<GPSListViewItem*>(item);
        if (lvItem)
        {
            if (lvItem->isDirty())
                dirty++;
        }
        i++;
    }
    while (item);

    if (dirty > 0)
    {
        QString msg = i18np("1 image from the list is not updated.",
                           "%1 images from the list are not updated.", dirty);

        if (KMessageBox::No == KMessageBox::warningYesNo(this,
                     i18n("<p>%1\n"
                          "Do you really want to close this window without applying changes?</p>", msg)))
            return false;
    }

    return true;
}

void GPSSyncDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync Settings"));

    d->maxGapInput->setValue(group.readEntry("Max Gap Time", 30));
    const int timeZoneGroupIndex = qMax(1, qMin(2, group.readEntry("Time Zone Mode", 1)));
    d->timeZoneGroup->button(timeZoneGroupIndex)->setChecked(true);
    slotTimeZoneModeChanged(timeZoneGroupIndex);
    d->timeZoneCB->setCurrentIndex(group.readEntry("Time Zone", 16));  // GMT+00:00
    d->interpolateBox->setChecked(group.readEntry("Interpolate", false));
    d->maxTimeInput->setValue(group.readEntry("Max Inter Dist Time", 15));
    d->offsetSign->setCurrentIndex(group.readEntry("Offset Sign", 0));
    d->offsetMin->setValue(group.readEntry("Offset Min", 0));
    d->offsetSec->setValue(group.readEntry("Offset Sec", 0));

    d->maxTimeLabel->setEnabled(d->interpolateBox->isChecked());
    d->maxTimeInput->setEnabled(d->interpolateBox->isChecked());

    KConfigGroup group2 = config.group(QString("GPS Sync Dialog"));
    restoreDialogSize(group2);
}

void GPSSyncDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync Settings"));
    group.writeEntry("Max Gap Time", d->maxGapInput->value() );
    group.writeEntry("Time Zone Mode", d->timeZoneGroup->checkedId() );
    group.writeEntry("Time Zone", d->timeZoneCB->currentIndex() );
    group.writeEntry("Interpolate", d->interpolateBox->isChecked() );
    group.writeEntry("Max Inter Dist Time", d->maxTimeInput->value() );
    group.writeEntry("Offset Sign", d->offsetSign->currentIndex());
    group.writeEntry("Offset Min", d->offsetMin->value());
    group.writeEntry("Offset Sec", d->offsetSec->value());

    KConfigGroup group2 = config.group(QString("GPS Sync Dialog"));
    saveDialogSize(group2);

    config.sync();
}

// Correlate the GPS positions from Pictures using a GPX file data.
void GPSSyncDialog::slotUser1Correlate()
{
    const bool cameraHasSystemTimeZone = (d->timeZoneGroup->checkedId() == 1);
    int itemsUpdated      = 0;
    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = d->imagesList->listView()->topLevelItem(i);
        GPSListViewItem *lvItem = dynamic_cast<GPSListViewItem*>(item);
        if (lvItem)
        {
            GPSDataContainer gpsData;

            int offset = 0;

            if (!cameraHasSystemTimeZone)
            {
                QString tz = d->timeZoneCB->currentText();
                int hh     = QString(QString(tz[4])+QString(tz[5])).toInt();
                int mm     = QString(QString(tz[7])+QString(tz[8])).toInt();
                int timeZoneOffset = hh*3600 + mm*60;
                if (tz[3] == QChar('-')) {
                    timeZoneOffset = (-1) * timeZoneOffset;
                }

                offset+= timeZoneOffset;
            }

            int userOffset = d->offsetMin->value() * 60 + d->offsetSec->value();
            if (d->offsetSign->currentText() == "-") {
                userOffset = (-1) * userOffset;
            }

            offset+= userOffset;

            if (d->gpxParser.matchDate(lvItem->dateTime(),
                                       d->maxGapInput->value(),
                                       offset,
                                       !cameraHasSystemTimeZone,
                                       d->interpolateBox->isChecked(),
                                       d->maxTimeInput->value()*60,
                                       &gpsData))
            {
                lvItem->setGPSInfo(gpsData);
                itemsUpdated++;
            }
        }
        i++;
    }
    while (item);

    if (itemsUpdated == 0)
    {
        KMessageBox::sorry(this, i18n("Cannot find pictures to correlate with GPX file data."),
                           i18n("GPS Sync"));
        return;
    }

    QString msg = i18np("The GPS data of 1 image have been updated using the GPX data file.",    // this is correct - data is plural
                        "The GPS data of %1 images have been updated using the GPX data file.",
                        itemsUpdated);
    msg += '\n';
    msg += i18np("Press the Apply button to update the image's metadata.","Press the Apply button to update the images' metadata.", itemsUpdated);

    KMessageBox::information(this, msg, i18n("GPS Sync"));
}

// Start the GPS coordinates editor dialog.
void GPSSyncDialog::slotUser2EditCoordinates()
{
    const QList<QTreeWidgetItem*> selectedItemsList = d->imagesList->listView()->selectedItems();
    
    if (selectedItemsList.isEmpty())
    {
        KMessageBox::information(this, i18n("Please select at least one image from "
                     "the list to edit GPS coordinates manually."), i18n("GPS Sync"));
        return;
    }

    GPSListViewItem* const item = dynamic_cast<GPSListViewItem*>(d->imagesList->listView()->currentItem());

    QPointer<GPSEditDialog> dlg = new GPSEditDialog(this, item->GPSInfo(),
                                                    item->url().fileName(),
                                                    item->hasGPSInfo());

    if (dlg->exec() == KDialog::Accepted)
    {
        for (QList<QTreeWidgetItem*>::const_iterator it = selectedItemsList.constBegin(); it!=selectedItemsList.constEnd(); ++it)
        {
            GPSListViewItem* const lvItem = dynamic_cast<GPSListViewItem*>(*it);
            lvItem->setGPSInfo(dlg->getGPSInfo(), true, true);
        }
    }

    delete dlg;
}

// Remove GPS coordinates from pictures.
void GPSSyncDialog::slotUser3RemoveCoordinates()
{
    const QList<QTreeWidgetItem*> selectedItemsList = d->imagesList->listView()->selectedItems();
    if (selectedItemsList.isEmpty())
    {
        KMessageBox::information(this, i18n("Please select at least one image from "
                     "which to remove GPS coordinates."), i18n("GPS Sync"));
        return;
    }

    for (QList<QTreeWidgetItem*>::const_iterator it = selectedItemsList.constBegin(); it!=selectedItemsList.constEnd(); ++it)
    {
        GPSListViewItem* const lvItem = dynamic_cast<GPSListViewItem*>(*it);
        lvItem->eraseGPSInfo();
    }
}

void GPSSyncDialog::slotApply()
{
    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = d->imagesList->listView()->topLevelItem(i);
        GPSListViewItem* const lvItem = dynamic_cast<GPSListViewItem*>(item);
        if (lvItem)
        {
            if (lvItem->isDirty())
            {
                d->imagesList->listView()->setCurrentItem(lvItem);
                d->imagesList->listView()->scrollToItem(lvItem);
                lvItem->writeGPSInfoToFile();
            }
        }
        kapp->processEvents();
        i++;
    }
    while (item);
}

void GPSSyncDialog::slotTimeZoneModeChanged(int id)
{
    d->timeZoneCB->setEnabled(id==2);
}

}  // namespace KIPIGPSSyncPlugin
