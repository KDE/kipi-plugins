/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with
 *               a GPS device.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QGroupBox>
#include <QComboBox>
#include <QPainter>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QCloseEvent>
#include <QHeaderView>
#include <QTreeWidget>

// KDE includes.

#include <ktoolinvocation.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <ksqueezedtextlabel.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>
#include <knuminput.h>
#include <kseparator.h>

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

    QTreeWidget              *listView;

    KIntSpinBox              *maxGapInput;
    KIntSpinBox              *maxTimeInput;

    KSqueezedTextLabel       *gpxFileName;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;

    GPSDataParser             gpxParser;
};

GPSSyncDialog::GPSSyncDialog(KIPI::Interface* interface, QWidget* parent)
             : KDialog(parent)
{
    d = new GPSSyncDialogPriv;
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

    d->listView = new QTreeWidget(page);
    d->listView->setColumnCount(7);
    d->listView->setIconSize(QSize(64, 64));
    d->listView->setSortingEnabled(false);
    d->listView->setRootIsDecorated(false);
    d->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setMinimumWidth(450);
    d->listView->header()->setResizeMode(QHeaderView::Stretch);

    QStringList labels;
    labels.append( i18n("Thumbnail") );
    labels.append( i18n("File Name") );
    labels.append( i18n("Camera time stamp") );
    labels.append( i18n("Latitude") );
    labels.append( i18n("Longitude") );
    labels.append( i18n("Altitude") );
    labels.append( i18n("Status") );
    d->listView->setHeaderLabels(labels);

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
    d->maxGapInput->setWhatsThis(i18n("<p>Sets the maximum difference in "
                    "seconds from a GPS track point to the image time to be matched. "
                    "If the time difference exceeds this setting, no match will be attempted."));

    QLabel *timeZoneLabel = new QLabel(i18n("Time zone:"), settingsBox);
    d->timeZoneCB         = new QComboBox(settingsBox);

    // See list of time zomes over the world :
    // http://en.wikipedia.org/wiki/List_of_time_zones
    // NOTE: Combo box strings are not i18n.
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
                    "can be converted to GMT to match the GPS time reference.\n"
                    "Note: positive offsets count eastwards from zero longitude (GMT), "
                    "they are 'ahead of time'."));

    d->interpolateBox = new QCheckBox(i18n("Interpolate"), settingsBox);
    d->interpolateBox->setWhatsThis(i18n("<p>Set this option to interpolate GPS track points "
                    "which are not closely matched to the GPX data file."));

    d->maxTimeLabel = new QLabel(i18n("Difference in min.:"), settingsBox);
    d->maxTimeInput = new KIntSpinBox(0, 240, 1, 15, settingsBox);
    d->maxTimeInput->setWhatsThis(i18n("<p>Sets the maximum time difference in minutes (240 max.)"
                    " to interpolate GPX file points to image time data."));

    settingsBoxLayout->addWidget(loadGPXButton, 0, 0, 1, 2 );
    settingsBoxLayout->addWidget(gpxFileLabel, 1, 0, 1, 2 );
    settingsBoxLayout->addWidget(d->gpxFileName, 2, 0, 1, 2 );
    settingsBoxLayout->addWidget(d->gpxPointsLabel, 3, 0, 1, 2 );
    settingsBoxLayout->addWidget(line, 4, 0, 1, 2 );
    settingsBoxLayout->addWidget(maxGapLabel, 5, 0, 1, 1);
    settingsBoxLayout->addWidget(d->maxGapInput, 5, 1, 1, 1);
    settingsBoxLayout->addWidget(timeZoneLabel, 6, 0, 1, 1);
    settingsBoxLayout->addWidget(d->timeZoneCB, 6, 1, 1, 1);
    settingsBoxLayout->addWidget(d->interpolateBox, 7, 0, 1, 2 );
    settingsBoxLayout->addWidget(d->maxTimeLabel, 8, 0, 1, 1);
    settingsBoxLayout->addWidget(d->maxTimeInput, 8, 1, 1, 1);
    settingsBoxLayout->setSpacing(spacingHint());
    settingsBoxLayout->setMargin(0);

    // ---------------------------------------------------------------

    mainLayout->addWidget(d->listView, 0, 0, 3, 2 );
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
                   ki18n("A Plugin to synchronize pictures metadata with a GPS device"),
                   ki18n("(c) 2006-2008, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"), 
                        ki18n("Developer and maintainer"),
                              "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu* helpMenu     = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook       = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu( helpMenu->menu() );

    // ---------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotApply()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotUser1()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotUser2()));

    connect(this, SIGNAL(user3Clicked()),
            this, SLOT(slotUser3()));

    connect(d->interface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
            this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));

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
    KToolInvocation::invokeHelp("gpssync", "kipi-plugins");
}

void GPSSyncDialog::setImages( const KUrl::List& images )
{
    for( KUrl::List::ConstIterator it = images.begin(); it != images.end(); ++it )
        new GPSListViewItem(d->interface, d->listView, *it);

    d->interface->thumbnails(images, 64);
}

void GPSSyncDialog::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = d->listView->topLevelItem(i);
        GPSListViewItem *lvItem = dynamic_cast<GPSListViewItem*>(item);
        if (lvItem)
        {
            if (lvItem->url() == url)
            {
                lvItem->setThumbnail(pix);
                return;
            }
        }
        i++;
    }
    while (item);
}

void GPSSyncDialog::slotLoadGPXFile()
{
    KUrl loadGPXFile = KFileDialog::getOpenUrl(KGlobalSettings::documentPath(),
                                               i18n("%1|GPS Exchange Format", QString("*.gpx")), this,
                                               i18n("Select GPX File to Load") );
    if( loadGPXFile.isEmpty() )
       return;

    d->gpxParser.clear();
    bool ret = d->gpxParser.loadGPXFile(loadGPXFile);

    if (!ret)
    {
        KMessageBox::error(this, i18n("Cannot parse %1 GPX file!",
                           loadGPXFile.fileName()), i18n("GPS Sync"));
        enableButton(User1, false);
        return;
    }

    if (d->gpxParser.numPoints() <= 0)
    {
        KMessageBox::sorry(this, i18n("The %1 GPX file do not have a date-time track to use!",
                           loadGPXFile.fileName()), i18n("GPS Sync"));
        enableButton(User1, false);
        return;
    }

    d->gpxFileName->setText(loadGPXFile.fileName());
    d->gpxPointsLabel->setText(i18n("Points parsed: %1", d->gpxParser.numPoints()));
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
    done(Close);
}

bool GPSSyncDialog::promptUserClose()
{
    // Check if one item is dirty in the list.

    int dirty             = 0;
    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = d->listView->topLevelItem(i);
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
                           "%n images from the list are not updated.", dirty);

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
    d->timeZoneCB->setCurrentIndex(group.readEntry("Time Zone", 16));  // GMT+00:00
    d->interpolateBox->setChecked(group.readEntry("Interpolate", false));
    d->maxTimeInput->setValue(group.readEntry("Max Inter Dist Time", 15));

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
    group.writeEntry("Time Zone", d->timeZoneCB->currentIndex() );
    group.writeEntry("Interpolate", d->interpolateBox->isChecked() );
    group.writeEntry("Max Inter Dist Time", d->maxTimeInput->value() );

    KConfigGroup group2 = config.group(QString("GPS Sync Dialog"));
    saveDialogSize(group2);

    config.sync();
}

// Correlate the GPS positions from Pictures using a GPX file data.
void GPSSyncDialog::slotUser1()
{
    int itemsUpdated      = 0;
    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = d->listView->topLevelItem(i);
        GPSListViewItem *lvItem = dynamic_cast<GPSListViewItem*>(item);
        if (lvItem)
        {
            GPSDataContainer gpsData;
            QString tz = d->timeZoneCB->currentText();
            int hh     = QString(QString(tz[4])+QString(tz[5])).toInt();
            int mm     = QString(QString(tz[7])+QString(tz[8])).toInt();
            int offset = hh*3600 + mm*60;

            if (tz[3] == QChar('-'))
                offset = (-1)*offset;

            if (d->gpxParser.matchDate(lvItem->dateTime(),
                                       d->maxGapInput->value(),
                                       offset,
                                       d->interpolateBox->isChecked(),
                                       d->maxTimeInput->value()*60,
                                       gpsData))
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

    QString msg = i18np("GPS data of 1 image have been updated on the list using the GPX data file.",
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

    if (dlg.exec() == KDialog::Accepted)
    {
        int i                 = 0;
        QTreeWidgetItem *item = 0;
        do
        {
            item = d->listView->topLevelItem(i);
            GPSListViewItem *lvItem = dynamic_cast<GPSListViewItem*>(item);
            if (lvItem)
            {
                if (lvItem->isSelected())
                    lvItem->setGPSInfo(dlg.getGPSInfo(), true, true);
            }
            i++;
        }
        while (item);
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

    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = d->listView->topLevelItem(i);
        GPSListViewItem *lvItem = dynamic_cast<GPSListViewItem*>(item);
        if (lvItem)
            lvItem->eraseGPSInfo();
        i++;
    }
    while (item);
}

void GPSSyncDialog::slotApply()
{
    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = d->listView->topLevelItem(i);
        GPSListViewItem *lvItem = dynamic_cast<GPSListViewItem*>(item);
        if (lvItem)
        {
            d->listView->setCurrentItem(lvItem);
            d->listView->scrollToItem(lvItem);
            lvItem->writeGPSInfoToFile();
        }
        kapp->processEvents();
        i++;
    }
    while (item);
}

}  // NameSpace KIPIGPSSyncPlugin
