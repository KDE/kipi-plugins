/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF adjustments settings page.
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

// C++ includes.

#include <cmath>

// QT includes.

#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qpair.h>
#include <qmap.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>

// Local includes.

#include "exiv2iface.h"
#include "exifadjust.h"
#include "exifadjust.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFAdjustPriv
{
public:

    EXIFAdjustPriv()
    {
        brightnessCheck  = 0;
        gainControlCheck = 0;
        brightnessEdit   = 0;
        gainControlCB    = 0;
    }

    QCheckBox      *brightnessCheck;
    QCheckBox      *gainControlCheck;

    QComboBox      *gainControlCB;
   
    KDoubleSpinBox *brightnessEdit;
};

EXIFAdjust::EXIFAdjust(QWidget* parent, QByteArray& exifData)
            : QWidget(parent)
{
    d = new EXIFAdjustPriv;

    QGridLayout* grid = new QGridLayout(parent, 6, 2, KDialog::spacingHint());

    // --------------------------------------------------------

    d->brightnessCheck = new QCheckBox(i18n("Brightness (APEX):"), parent);
    d->brightnessEdit  = new KDoubleSpinBox(-99.99, 99.99, 0.1, 0.0, 2, parent);
    grid->addMultiCellWidget(d->brightnessCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->brightnessEdit, 0, 0, 2, 2);
    QWhatsThis::add(d->brightnessEdit, i18n("<p>Set here the brightness adjustment value in APEX unit "
                                             "used by camera to take the picture."));

    // --------------------------------------------------------

    d->gainControlCheck = new QCheckBox(i18n("Gain Control:"), parent);
    d->gainControlCB    = new QComboBox(false, parent);
    d->gainControlCB->insertItem(i18n("None"),           0);
    d->gainControlCB->insertItem(i18n("Low gain up"),    1);
    d->gainControlCB->insertItem(i18n("High gain up"),   2);
    d->gainControlCB->insertItem(i18n("Low gain down"),  3);
    d->gainControlCB->insertItem(i18n("High gain down"), 4);
    grid->addMultiCellWidget(d->gainControlCheck, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->gainControlCB, 1, 1, 2, 2);
    QWhatsThis::add(d->gainControlCB, i18n("<p>Set here the degree of overall image gain adjustment "
                                             "used by camera to take the picture."));


    grid->setColStretch(1, 10);                     
    grid->setRowStretch(6, 10);                     

    // --------------------------------------------------------

    connect(d->brightnessCheck, SIGNAL(toggled(bool)),
            d->brightnessEdit, SLOT(setEnabled(bool)));

    connect(d->gainControlCheck, SIGNAL(toggled(bool)),
            d->gainControlCB, SLOT(setEnabled(bool)));

    // --------------------------------------------------------
    
    readMetadata(exifData);
}

EXIFAdjust::~EXIFAdjust()
{
    delete d;
}

void EXIFAdjust::readMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;
    long     val=0;
    
    if (exiv2Iface.getExifTagRational("Exif.Photo.BrightnessValue", num, den))
    {
        d->brightnessEdit->setValue((double)(num) / (double)(den));
        d->brightnessCheck->setChecked(true);
    }
    d->brightnessEdit->setEnabled(d->brightnessCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.GainControl", val))
    {
        d->gainControlCB->setCurrentItem(val);
        d->gainControlCheck->setChecked(true);
    }
    d->gainControlCB->setEnabled(d->gainControlCheck->isChecked());
}

void EXIFAdjust::applyMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;

    if (d->brightnessCheck->isChecked())
    {
        exiv2Iface.convertToRational(d->brightnessEdit->value(), &num, &den, 1);
        exiv2Iface.setExifTagRational("Exif.Photo.BrightnessValue", num, den);
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.BrightnessValue");

    if (d->gainControlCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.GainControl", d->gainControlCB->currentItem());
    else
        exiv2Iface.removeExifTag("Exif.Photo.GainControl");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

