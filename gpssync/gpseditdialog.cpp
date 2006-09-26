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
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <klineedit.h>
#include <kmessagebox.h>

// Local includes.

#include "gpseditdialog.h"
#include "gpseditdialog.moc"

namespace KIPIGPSSyncPlugin
{

class GPSEditDialogDialogPrivate
{

public:

    enum WebGPSLocator
    {
        CapeLinks = 0,
        MapKi
        // TODO : Added here others web GPS coordinates locator
    };

    GPSEditDialogDialogPrivate()
    {
        altitudeInput  = 0;
        latitudeInput  = 0;
        longitudeInput = 0;
        gpsButton      = 0;
        gpsCombo       = 0;
    }

    QPushButton *gpsButton;

    QComboBox   *gpsCombo;

    KLineEdit   *altitudeInput;
    KLineEdit   *latitudeInput;
    KLineEdit   *longitudeInput;
};

GPSEditDialog::GPSEditDialog(QWidget* parent, GPSDataContainer gpsData, 
                             const QString& fileName)
             : KDialogBase(Plain, i18n("%1 - Edit GPS coordinates").arg(fileName),
                           Help|User1|Ok|Cancel, Ok,
                           parent, 0, true, true)
{
    d = new GPSEditDialogDialogPrivate;

    setHelp("gpssync", "kipi-plugins");
    setButtonText(User1, i18n("Delete"));

    QGridLayout* grid = new QGridLayout(plainPage(), 3, 2, 0, spacingHint());

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
    d->latitudeInput->setValidator(new QDoubleValidator(-90.0, 90.0, 8, this));
    d->longitudeInput->setValidator(new QDoubleValidator(-180.0, 180.0, 8, this));

    d->altitudeInput->setText(QString::number(gpsData.altitude(),   'g', 12));
    d->latitudeInput->setText(QString::number(gpsData.latitude(),   'g', 12));
    d->longitudeInput->setText(QString::number(gpsData.longitude(), 'g', 12));

    d->gpsCombo  = new QComboBox( false, plainPage() );
    d->gpsButton = new QPushButton(i18n("Get GPS Coordinates..."), plainPage());
    d->gpsCombo->insertItem(QString("Capelinks"), GPSEditDialogDialogPrivate::CapeLinks);
    d->gpsCombo->insertItem(QString("MapKi"), GPSEditDialogDialogPrivate::MapKi);

    // TODO : Added here others web GPS coordinates locator
    
    grid->addMultiCellWidget(altitudeLabel, 0, 0, 0, 0);
    grid->addMultiCellWidget(latitudeLabel, 1, 1, 0, 0);
    grid->addMultiCellWidget(longitudeLabel, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->altitudeInput, 0, 0, 1, 1);
    grid->addMultiCellWidget(d->latitudeInput, 1, 1, 1, 1);
    grid->addMultiCellWidget(d->longitudeInput, 2, 2, 1, 1);
    grid->addMultiCellWidget(altResetButton, 0, 0, 2, 2);
    grid->addMultiCellWidget(latResetButton, 1, 1, 2, 2);
    grid->addMultiCellWidget(lonResetButton, 2, 2, 2, 2);
    grid->addMultiCellWidget(d->gpsCombo, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->gpsButton, 3, 3, 1, 2);

    connect(d->gpsButton, SIGNAL(clicked()),
            this, SLOT(slotGPSLocator()));

    connect(altResetButton, SIGNAL(clicked()),
            d->altitudeInput, SLOT(clear()));

    connect(latResetButton, SIGNAL(clicked()),
            d->latitudeInput, SLOT(clear()));

    connect(lonResetButton, SIGNAL(clicked()),
            d->longitudeInput, SLOT(clear()));

    adjustSize();
}

GPSEditDialog::~GPSEditDialog()
{
    delete d;
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

    accept();
}

void GPSEditDialog::slotUser1()
{
    done(-1);
}

void GPSEditDialog::slotGPSLocator()
{
    QString val, url;

    switch( d->gpsCombo->currentItem() )
    {
        case GPSEditDialogDialogPrivate::CapeLinks:
        {
            url.append("http://www.capelinks.com/cape-cod/maps/gps");
            break;
        }

        case GPSEditDialogDialogPrivate::MapKi:
        {
            url.append("http://mapki.com/getLonLat.php");
            break;
        }

        // TODO : Added here others web GPS coordinates locator
    }
    
    KApplication::kApplication()->invokeBrowser(url);
}

}  // namespace KIPIGPSSyncPlugin
