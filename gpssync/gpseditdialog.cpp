/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-22
 * Description : a dialog to edit GPS positions
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

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <khtmlview.h>

// Local includes.

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
    }

    KLineEdit    *altitudeInput;
    KLineEdit    *latitudeInput;
    KLineEdit    *longitudeInput;

    GPSMapWidget *worldMap;
};

GPSEditDialog::GPSEditDialog(QWidget* parent, GPSDataContainer gpsData, const QString& fileName)
             : KDialogBase(Plain, i18n("%1 - Edit GPS coordinates").arg(fileName),
                           Help|Ok|Cancel, Ok,
                           parent, 0, true, true)
{
    d = new GPSEditDialogDialogPrivate;

    setHelp("gpssync", "kipi-plugins");

    QGridLayout* grid = new QGridLayout(plainPage(), 7, 3, 0, spacingHint());

    QLabel *message = new QLabel(i18n("<p>Use the map on the left to select the right place where "
                                      "have been taken the picture. Click with right mouse button "
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

    d->altitudeInput->setText(QString::number(gpsData.altitude(),   'g', 12));
    d->latitudeInput->setText(QString::number(gpsData.latitude(),   'g', 12));
    d->longitudeInput->setText(QString::number(gpsData.longitude(), 'g', 12));

    d->worldMap = new GPSMapWidget(plainPage(), d->latitudeInput->text(), d->longitudeInput->text());
    d->worldMap->show();

    grid->addMultiCellWidget(message, 0, 0, 0, 2);
    grid->addMultiCellWidget(altitudeLabel, 1, 1, 0, 2);
    grid->addMultiCellWidget(d->altitudeInput, 2, 2, 0, 1);
    grid->addMultiCellWidget(altResetButton, 2, 2, 2, 2);
    grid->addMultiCellWidget(latitudeLabel, 3, 3, 0, 2);
    grid->addMultiCellWidget(d->latitudeInput, 4, 4, 0, 1);
    grid->addMultiCellWidget(latResetButton, 4, 4, 2, 2);
    grid->addMultiCellWidget(longitudeLabel, 5, 5, 0, 2);
    grid->addMultiCellWidget(d->longitudeInput, 6, 6, 0, 1);
    grid->addMultiCellWidget(lonResetButton, 6, 6, 2, 2);
    grid->addMultiCellWidget(d->worldMap->view(), 0, 7, 3, 3);
    grid->setColStretch(0, 2);
    grid->setColStretch(3, 10);
    grid->setRowStretch(7, 10);

    connect(altResetButton, SIGNAL(released()),
            d->altitudeInput, SLOT(clear()));

    connect(latResetButton, SIGNAL(released()),
            d->latitudeInput, SLOT(clear()));

    connect(lonResetButton, SIGNAL(released()),
            d->longitudeInput, SLOT(clear()));

    connect(d->worldMap, SIGNAL(signalMouseReleased()),
            this, SLOT(slotGetGPSLocationFromMap()));

    readSettings();
}

GPSEditDialog::~GPSEditDialog()
{
    delete d;
}

void GPSEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
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
    config.sync();
}

GPSDataContainer GPSEditDialog::getGPSInfo()
{
    return GPSDataContainer(d->altitudeInput->text().toDouble(), 
                            d->latitudeInput->text().toDouble(),
                            d->longitudeInput->text().toDouble(),
                            false);
}

void GPSEditDialog::slotOk()
{
    bool ok;

    d->altitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Altitude value is not correct!"), i18n("GPS Sync"));    
        return;
    }        

    d->latitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Latitude value is not correct!"), i18n("GPS Sync"));    
        return;
    }        

    d->longitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Longitude value is not correct!"), i18n("GPS Sync"));    
        return;
    }        

    saveSettings();
    accept();
}

void GPSEditDialog::slotGetGPSLocationFromMap()
{
    QString status = d->worldMap->jsStatusBarText();
    
    if (status.startsWith(QString("(lat:")))
    {
        status.remove(0, 5);
        status.truncate(status.length()-1);
        QString lat = status.section(",", 0, 0);
        QString lon = status.section(",", 1, 1);
        lon.remove(0, 5);
        d->latitudeInput->setText(lat);
        d->longitudeInput->setText(lon);
    }
}

}  // namespace KIPIGPSSyncPlugin
