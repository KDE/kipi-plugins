/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF exposure settings page.
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

// QT includes.

#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qcombobox.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>

// Local includes.

#include "exiv2iface.h"
#include "exifexposure.h"
#include "exifexposure.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFExposurePriv
{
public:

    EXIFExposurePriv()
    {
        exposureTimeCheck    = 0;
        exposureProgramCheck = 0;
        exposureTimeNumEdit  = 0;
        exposureTimeDenEdit  = 0;
    }

    QCheckBox   *exposureTimeCheck;
    QCheckBox   *exposureProgramCheck;

    QComboBox   *exposureProgramCB;

    KIntSpinBox *exposureTimeNumEdit;
    KIntSpinBox *exposureTimeDenEdit;
};

EXIFExposure::EXIFExposure(QWidget* parent, QByteArray& exifData)
            : QWidget(parent)
{
    d = new EXIFExposurePriv;

    QGridLayout* grid = new QGridLayout(parent, 8, 4, KDialog::spacingHint());

    // --------------------------------------------------------

    d->exposureTimeCheck   = new QCheckBox(i18n("Exposure time (seconds):"), parent);
    d->exposureTimeNumEdit = new KIntSpinBox(1, 100000, 1, 1, 10, parent);
    d->exposureTimeDenEdit = new KIntSpinBox(1, 100000, 1, 1, 10, parent);
    QLabel *exposureLabel  = new QLabel("/", parent);
    exposureLabel->setAlignment (Qt::AlignRight|Qt::AlignVCenter);
    grid->addMultiCellWidget(d->exposureTimeCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->exposureTimeNumEdit, 0, 0, 2, 2);
    grid->addMultiCellWidget(exposureLabel, 0, 0, 3, 3);
    grid->addMultiCellWidget(d->exposureTimeDenEdit, 0, 0, 4, 4);
    QWhatsThis::add(d->exposureTimeCheck, i18n("<p>Set on this option to set the exposure time "
                    "of picture, given in seconds."));

    // --------------------------------------------------------

    d->exposureProgramCheck = new QCheckBox(i18n("Exposure Program:"), parent);
    d->exposureProgramCB    = new QComboBox(false, parent);
    d->exposureProgramCB->insertItem(i18n("Not defined"),       0);
    d->exposureProgramCB->insertItem(i18n("Manual"),            1);
    d->exposureProgramCB->insertItem(i18n("Auto"),              2);
    d->exposureProgramCB->insertItem(i18n("Aperture priority"), 3);
    d->exposureProgramCB->insertItem(i18n("Shutter priority"),  4);
    d->exposureProgramCB->insertItem(i18n("Creative program"),  5);
    d->exposureProgramCB->insertItem(i18n("Action program"),    6);
    d->exposureProgramCB->insertItem(i18n("Portrait mode"),     7);
    d->exposureProgramCB->insertItem(i18n("Landscape mode"),    8);
    grid->addMultiCellWidget(d->exposureProgramCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->exposureProgramCB, 2, 2, 2, 4);
    QWhatsThis::add(d->exposureProgramCB, i18n("<p>Select here the program used by the camera "
                                          "to set exposure when the picture have been taken."));

    grid->setColStretch(1, 10);                     
    grid->setRowStretch(6, 10);                     

    // --------------------------------------------------------

    connect(d->exposureTimeCheck, SIGNAL(toggled(bool)),
            d->exposureTimeNumEdit, SLOT(setEnabled(bool)));

    connect(d->exposureTimeCheck, SIGNAL(toggled(bool)),
            d->exposureTimeDenEdit, SLOT(setEnabled(bool)));

    connect(d->exposureProgramCheck, SIGNAL(toggled(bool)),
            d->exposureProgramCB, SLOT(setEnabled(bool)));

    // --------------------------------------------------------
    
    readMetadata(exifData);
}

EXIFExposure::~EXIFExposure()
{
    delete d;
}

void EXIFExposure::readMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    int  num=0, den=0;
    long val=0;
    
    if (exiv2Iface.getExifTagRational("Exif.Photo.ExposureTime", num, den))
    {
        d->exposureTimeNumEdit->setValue(num);
        d->exposureTimeDenEdit->setValue(den);
        d->exposureTimeCheck->setChecked(true);
    }
    d->exposureTimeNumEdit->setEnabled(d->exposureTimeCheck->isChecked());
    d->exposureTimeDenEdit->setEnabled(d->exposureTimeCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.ExposureProgram", val))
    {
        d->exposureProgramCB->setCurrentItem(val);
        d->exposureProgramCheck->setChecked(true);
    }
    d->exposureProgramCB->setEnabled(d->exposureProgramCheck->isChecked());
}

void EXIFExposure::applyMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);

    if (d->exposureTimeCheck->isChecked())
        exiv2Iface.setExifTagRational("Exif.Photo.ExposureTime", d->exposureTimeNumEdit->value(),
                                      d->exposureTimeDenEdit->value());
    else
        exiv2Iface.removeExifTag("Exif.Photo.ExposureTime");

    if (d->exposureProgramCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.ExposureProgram", d->exposureProgramCB->currentItem());
    else
        exiv2Iface.removeExifTag("Exif.Photo.ExposureProgram");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

