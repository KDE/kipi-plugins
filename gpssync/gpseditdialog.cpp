/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-22
 * Description : a dialog to edit GPS positions
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

#include "gpseditdialog.h"
#include "gpseditdialog.moc"

// Qt includes

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTimer>
#include <QValidator>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <khtmlview.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Local includes

#include "gpsmapwidget.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"

namespace KIPIGPSSyncPlugin
{

class GPSEditDialogPrivate
{

public:

    GPSEditDialogPrivate()
    {
        altitudeInput  = 0;
        latitudeInput  = 0;
        longitudeInput = 0;
        worldMap       = 0;
        about          = 0;
        goButton       = 0;
        hasGPSInfo     = false;
    }

    bool                      hasGPSInfo;

    QPushButton              *goButton;

    KLineEdit                *altitudeInput;
    KLineEdit                *latitudeInput;
    KLineEdit                *longitudeInput;

    KIPIPlugins::KPAboutData *about;

    GPSDataContainer          gpsData;

    GPSMapWidget             *worldMap;
};

GPSEditDialog::GPSEditDialog(QWidget* parent, const GPSDataContainer& gpsData,
                             const QString& fileName, bool hasGPSInfo)
             : KDialog(parent), d(new GPSEditDialogPrivate)
{
    d->hasGPSInfo = hasGPSInfo;
    d->gpsData    = gpsData;

    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("%1 - Edit Geographical Coordinates", fileName));
    setModal(true);

    QWidget *page = new QWidget( this );
    setMainWidget( page );

    QGridLayout* grid = new QGridLayout(page);

    QLabel *message   = new QLabel(i18n("<p>Use the map on the right to select the location where "
                                        "the picture was taken. Click with left mouse button or move the marker "
                                        "on the map to get the GPS coordinates.</p>"), page);
    message->setWordWrap(true);

    QLabel *altitudeLabel  = new QLabel(i18n("Altitude:"), page);
    QLabel *latitudeLabel  = new QLabel(i18n("Latitude:"), page);
    QLabel *longitudeLabel = new QLabel(i18n("Longitude:"), page);

    d->altitudeInput       = new KLineEdit(page);
    d->latitudeInput       = new KLineEdit(page);
    d->longitudeInput      = new KLineEdit(page);

    d->altitudeInput->setClearButtonShown(true);
    d->latitudeInput->setClearButtonShown(true);
    d->longitudeInput->setClearButtonShown(true);

    d->altitudeInput->setValidator(new QDoubleValidator(-20000.0, 20000.0, 1, this));
    d->latitudeInput->setValidator(new QDoubleValidator(-90.0, 90.0, 12, this));
    d->longitudeInput->setValidator(new QDoubleValidator(-180.0, 180.0, 12, this));

    d->goButton = new QPushButton(i18n("Go to Location"), page);
    d->goButton->setEnabled(false);

    d->worldMap = new GPSMapWidget(page);
    d->worldMap->setFileName(fileName);
    d->worldMap->show();

    grid->addWidget(message,             0, 0, 1, 3);
    grid->addWidget(altitudeLabel,       1, 0, 1, 3);
    grid->addWidget(d->altitudeInput,    2, 0, 1, 3);
    grid->addWidget(latitudeLabel,       3, 0, 1, 3);
    grid->addWidget(d->latitudeInput,    4, 0, 1, 3);
    grid->addWidget(longitudeLabel,      5, 0, 1, 3);
    grid->addWidget(d->longitudeInput,   6, 0, 1, 3);
    grid->addWidget(d->goButton,         7, 0, 1, 3);
    grid->addWidget(d->worldMap->view(), 0, 3, 9, 1);
    grid->setColumnStretch(0, 3);
    grid->setColumnStretch(3, 10);
    grid->setRowStretch(8, 10);
    grid->setSpacing(spacingHint());
    grid->setMargin(0);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("GPS Sync"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Plugin to edit geographical coordinates"),
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

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    connect(d->altitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->latitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->longitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->worldMap, SIGNAL(signalNewGPSLocationFromMap(const QString&, const QString&, const QString&)),
            this, SLOT(slotNewGPSLocationFromMap(const QString&, const QString&, const QString&)));

    connect(d->goButton, SIGNAL(released()),
            this, SLOT(slotGotoLocation()));

    // ---------------------------------------------------------------

    readSettings();
    QTimer::singleShot(0, this, SLOT(slotUpdateWorldMap()));
}

GPSEditDialog::~GPSEditDialog()
{
    delete d->about;
    delete d;
}

void GPSEditDialog::slotHelp()
{
    KToolInvocation::invokeHelp("gpssync", "kipi-plugins");
}

void GPSEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void GPSEditDialog::slotGPSPositionChanged()
{
    d->goButton->setEnabled(true);
}

void GPSEditDialog::slotGotoLocation()
{
    if (!checkGPSLocation()) return;
    d->worldMap->setGPSPosition(d->latitudeInput->text(), d->longitudeInput->text());
    slotUpdateWorldMap();
}

void GPSEditDialog::slotUpdateWorldMap()
{
    d->worldMap->resized();
}

void GPSEditDialog::resizeEvent(QResizeEvent *e)
{
    if (!e) return;
    slotUpdateWorldMap();
}

void GPSEditDialog::slotCancel()
{
    saveSettings();
    done(Cancel);
}

void GPSEditDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync Settings"));

    KConfigGroup group2 = config.group(QString("GPS Edit Dialog"));
    restoreDialogSize(group2);

    QString mapType = group.readEntry("Map Type", QString("G_NORMAL_MAP"));

    if (mapType == QString("G_SATELLITE_TYPE"))   mapType = QString("G_SATELLITE_MAP");
    else if (mapType == QString("G_MAP_TYPE"))    mapType = QString("G_NORMAL_MAP");
    else if (mapType == QString("G_HYBRID_TYPE")) mapType = QString("G_HYBRID_MAP");

    d->worldMap->setMapType(mapType);
    d->worldMap->setZoomLevel(group.readEntry("Zoom Level", 8));

    d->altitudeInput->blockSignals(true);
    d->latitudeInput->blockSignals(true);
    d->longitudeInput->blockSignals(true);

    if (d->hasGPSInfo)
    {
        d->altitudeInput->setText(QString::number(d->gpsData.altitude(),   'g', 12));
        d->latitudeInput->setText(QString::number(d->gpsData.latitude(),   'g', 12));
        d->longitudeInput->setText(QString::number(d->gpsData.longitude(), 'g', 12));
    }
    else
    {
        d->altitudeInput->setText(QString::number(group.readEntry("GPS Last Altitude", 0.0),   'g', 12));
        d->latitudeInput->setText(QString::number(group.readEntry("GPS Last Latitude", 0.0),   'g', 12));
        d->longitudeInput->setText(QString::number(group.readEntry("GPS Last Longitude", 0.0), 'g', 12));
    }

    d->altitudeInput->blockSignals(false);
    d->latitudeInput->blockSignals(false);
    d->longitudeInput->blockSignals(false);

    d->worldMap->setGPSPosition(d->latitudeInput->text(), d->longitudeInput->text());
    d->worldMap->resized();
}

void GPSEditDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync Settings"));

    KConfigGroup group2 = config.group(QString("GPS Edit Dialog"));
    saveDialogSize(group2);

    group.writeEntry("GPS Last Latitude", d->latitudeInput->text().toDouble());
    group.writeEntry("GPS Last Longitude", d->longitudeInput->text().toDouble());
    group.writeEntry("GPS Last Altitude", d->altitudeInput->text().toDouble());
    group.writeEntry("Zoom Level", d->worldMap->zoomLevel());
    group.writeEntry("Map Type", d->worldMap->mapType());
    config.sync();
}

GPSDataContainer GPSEditDialog::getGPSInfo()
{
    return GPSDataContainer(d->altitudeInput->text().toDouble(),
                            d->latitudeInput->text().toDouble(),
                            d->longitudeInput->text().toDouble(),
                            false);
}

bool GPSEditDialog::checkGPSLocation()
{
    bool ok;

    d->altitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Altitude value is invalid."),
                           i18n("Edit Geographical Coordinates"));
        return false;
    }

    d->latitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Latitude value is invalid."),
                           i18n("Edit Geographical Coordinates"));
        return false;
    }

    d->longitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Longitude value is invalid."),
                           i18n("Edit Geographical Coordinates"));
        return false;
    }

    return true;
}

void GPSEditDialog::slotOk()
{
    if (!checkGPSLocation()) return;
    saveSettings();
    accept();
}

void GPSEditDialog::slotNewGPSLocationFromMap(const QString& lat, const QString& lon, const QString& alt)
{
    d->latitudeInput->setText(lat);
    d->longitudeInput->setText(lon);
    d->altitudeInput->setText(alt);
    d->goButton->setEnabled(false);
}

}  // namespace KIPIGPSSyncPlugin
