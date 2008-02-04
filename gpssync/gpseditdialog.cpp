/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-22
 * Description : a dialog to edit GPS positions
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

#include <qtimer.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <khelpmenu.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <khtmlview.h>
#include <kpopupmenu.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "gpsmapwidget.h"
#include "gpseditdialog.h"
#include "gpseditdialog.moc"

namespace KIPIGPSSyncPlugin
{

class GPSEditDialogDialogPrivate
{

public:

    GPSEditDialogDialogPrivate()
    {
        altitudeInput  = 0;
        latitudeInput  = 0;
        longitudeInput = 0;
        worldMap       = 0;
        about          = 0;
        goButton       = 0;
    }

    QPushButton              *goButton;

    KLineEdit                *altitudeInput;
    KLineEdit                *latitudeInput;
    KLineEdit                *longitudeInput;

    KIPIPlugins::KPAboutData *about; 

    GPSMapWidget             *worldMap;
};

GPSEditDialog::GPSEditDialog(QWidget* parent, GPSDataContainer gpsData, 
                             const QString& fileName, bool hasGPSInfo)
             : KDialogBase(Plain, i18n("%1 - Edit Geographical Coordinates").arg(fileName),
                           Help|Ok|Cancel, Ok,
                           parent, 0, true, false)
{
    d = new GPSEditDialogDialogPrivate;

    QGridLayout* grid = new QGridLayout(plainPage(), 8, 3, 0, spacingHint());

    QLabel *message   = new QLabel(i18n("<p>Use the map on the right to select the place where "
                                        "the picture have been taken. Click with right mouse button "
                                        "on the map to get the GPS coordinates.<p>"), plainPage());

    QLabel *altitudeLabel  = new QLabel(i18n("Altitude:"), plainPage());
    QLabel *latitudeLabel  = new QLabel(i18n("Latitude:"), plainPage());
    QLabel *longitudeLabel = new QLabel(i18n("Longitude:"), plainPage());

    d->altitudeInput       = new KLineEdit(plainPage());
    d->latitudeInput       = new KLineEdit(plainPage());
    d->longitudeInput      = new KLineEdit(plainPage());

    QPushButton *altResetButton = new QPushButton(SmallIcon("clear_left"), QString::null, plainPage());
    QPushButton *latResetButton = new QPushButton(SmallIcon("clear_left"), QString::null, plainPage());
    QPushButton *lonResetButton = new QPushButton(SmallIcon("clear_left"), QString::null, plainPage());

    d->altitudeInput->setValidator(new QDoubleValidator(-20000.0, 20000.0, 1, this));
    d->latitudeInput->setValidator(new QDoubleValidator(-90.0, 90.0, 12, this));
    d->longitudeInput->setValidator(new QDoubleValidator(-180.0, 180.0, 12, this));

    int zoomLevel = 8;
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");

    if (hasGPSInfo)
    {
        d->altitudeInput->setText(QString::number(gpsData.altitude(),   'g', 12));
        d->latitudeInput->setText(QString::number(gpsData.latitude(),   'g', 12));
        d->longitudeInput->setText(QString::number(gpsData.longitude(), 'g', 12));
        zoomLevel = config.readNumEntry("Zoom Level", 8);
    } 
    else 
    {
        d->altitudeInput->setText(QString::number(config.readDoubleNumEntry("GPS Last Altitude", 0.0),   'g', 12));
        d->latitudeInput->setText(QString::number(config.readDoubleNumEntry("GPS Last Latitude", 0.0),   'g', 12));
        d->longitudeInput->setText(QString::number(config.readDoubleNumEntry("GPS Last Longitude", 0.0), 'g', 12));
        zoomLevel = config.readNumEntry("Zoom Level", 8);
    }

    d->goButton = new QPushButton(i18n("Goto Location"), plainPage());
    d->goButton->setEnabled(false);

    d->worldMap = new GPSMapWidget(plainPage(), d->latitudeInput->text(), 
                                   d->longitudeInput->text(), zoomLevel);
    d->worldMap->show();

    grid->addMultiCellWidget(message,             0, 0, 0, 2);
    grid->addMultiCellWidget(altitudeLabel,       1, 1, 0, 2);
    grid->addMultiCellWidget(d->altitudeInput,    2, 2, 0, 1);
    grid->addMultiCellWidget(altResetButton,      2, 2, 2, 2);
    grid->addMultiCellWidget(latitudeLabel,       3, 3, 0, 2);
    grid->addMultiCellWidget(d->latitudeInput,    4, 4, 0, 1);
    grid->addMultiCellWidget(latResetButton,      4, 4, 2, 2);
    grid->addMultiCellWidget(longitudeLabel,      5, 5, 0, 2);
    grid->addMultiCellWidget(d->longitudeInput,   6, 6, 0, 1);
    grid->addMultiCellWidget(lonResetButton,      6, 6, 2, 2);
    grid->addMultiCellWidget(d->goButton,         7, 7, 0, 1);
    grid->addMultiCellWidget(d->worldMap->view(), 0, 8, 3, 3);
    grid->setColStretch(0, 3);
    grid->setColStretch(3, 10);
    grid->setRowStretch(8, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(I18N_NOOP("GPS Sync"),
                                            NULL,
                                            KAboutData::License_GPL,
                                            I18N_NOOP("A Plugin to synchronize pictures metadata with a GPS device"),
                                            "(c) 2006-2008, Gilles Caulier");

    d->about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                        "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("GPS Sync Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    connect(altResetButton, SIGNAL(released()),
            d->altitudeInput, SLOT(clear()));

    connect(latResetButton, SIGNAL(released()),
            d->latitudeInput, SLOT(clear()));

    connect(lonResetButton, SIGNAL(released()),
            d->longitudeInput, SLOT(clear()));

    connect(d->altitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->latitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->longitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->worldMap, SIGNAL(signalNewGPSLocationFromMap(const QString&, const QString&)),
            this, SLOT(slotNewGPSLocationFromMap(const QString&, const QString&)));

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
    KDialogBase::slotCancel();
}

void GPSEditDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");
    resize(configDialogSize(config, QString("GPS Edit Dialog")));
}

void GPSEditDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");
    saveDialogSize(config, QString("GPS Edit Dialog"));
    config.writeEntry("GPS Last Latitude", d->latitudeInput->text().toDouble());
    config.writeEntry("GPS Last Longitude", d->longitudeInput->text().toDouble());
    config.writeEntry("GPS Last Altitude", d->altitudeInput->text().toDouble());
    config.writeEntry("Zoom Level", d->worldMap->zoomLevel());
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
        KMessageBox::error(this, i18n("Altitude value is not correct!"), 
                           i18n("Edit Geographical Coordinates"));
        return false;
    }

    d->latitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Latitude value is not correct!"), 
                           i18n("Edit Geographical Coordinates"));
        return false;
    }

    d->longitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Longitude value is not correct!"), 
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

void GPSEditDialog::slotNewGPSLocationFromMap(const QString& lat, const QString& lon)
{
    d->latitudeInput->setText(lat);
    d->longitudeInput->setText(lon);
    d->goButton->setEnabled(false);
}

}  // namespace KIPIGPSSyncPlugin
