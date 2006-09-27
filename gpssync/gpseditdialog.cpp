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
#include <qframe.h>
#include <qlayout.h>
#include <qcombobox.h>
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
#include <khtml_part.h>
#include <khtmlview.h>

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
        gpsCombo       = 0;
        worldMap       = 0;
    }

    QComboBox   *gpsCombo;

    KLineEdit   *altitudeInput;
    KLineEdit   *latitudeInput;
    KLineEdit   *longitudeInput;

    KHTMLPart   *worldMap;
};

GPSEditDialog::GPSEditDialog(QWidget* parent, GPSDataContainer gpsData, 
                             const QString& fileName)
             : KDialogBase(Plain, i18n("%1 - Edit GPS coordinates").arg(fileName),
                           Help|User1|Ok|Cancel, Ok,
                           parent, 0, true, true)
{
    d = new GPSEditDialogDialogPrivate;

    setHelp("gpssync", "kipi-plugins");
    setButtonText(User1, i18n("Delete Coordinates"));

    QGridLayout* grid = new QGridLayout(plainPage(), 4, 3, 0, spacingHint());

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

    d->worldMap = new KHTMLPart(plainPage());
    /*d->worldMap->openURL(KURL("/home/gilles/Documents/Devel/SVN/trunk/extragear/libs/kipi-plugins/gpssync/getlonlat.html"));*/
    d->worldMap->view()->resize(640, 480);
    d->worldMap->setJScriptEnabled(true);
    d->worldMap->show();

    d->gpsCombo  = new QComboBox( false, plainPage() );
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
    grid->addMultiCellWidget(d->worldMap->view(), 0, 4, 3, 3);
    grid->setColStretch(3, 10);
    grid->setRowStretch(4, 10);

    connect(d->gpsCombo, SIGNAL(activated(int)),
            this, SLOT(slotGPSLocator(int)));

    connect(altResetButton, SIGNAL(clicked()),
            d->altitudeInput, SLOT(clear()));

    connect(latResetButton, SIGNAL(clicked()),
            d->latitudeInput, SLOT(clear()));

    connect(lonResetButton, SIGNAL(clicked()),
            d->longitudeInput, SLOT(clear()));

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

void GPSEditDialog::slotClose()
{
    saveSettings();
    KDialogBase::slotClose();
}

void GPSEditDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");
    d->gpsCombo->setCurrentItem(config.readNumEntry("GPS Locator", 
                                GPSEditDialogDialogPrivate::CapeLinks));
    resize(configDialogSize(config, QString("GPS Edit Dialog")));
    slotGPSLocator(d->gpsCombo->currentItem());
}

void GPSEditDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("GPS Sync Settings");
    config.writeEntry("GPS Locator", d->gpsCombo->currentItem());
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

    accept();
}

void GPSEditDialog::slotUser1()
{
    done(-1);
}

void GPSEditDialog::slotGPSLocator(int i)
{
    switch(i)
    {
        case GPSEditDialogDialogPrivate::CapeLinks:
        {
            d->worldMap->openURL(KURL("http://www.capelinks.com/cape-cod/maps/gps"));
            break;
        }

        case GPSEditDialogDialogPrivate::MapKi:
        {
            d->worldMap->openURL(KURL("http://mapki.com/getLonLat.php"));
            break;
        }

        // TODO : Added here others web GPS coordinates locator
    }
}

}  // namespace KIPIGPSSyncPlugin
