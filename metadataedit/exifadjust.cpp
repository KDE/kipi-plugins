/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF adjustments settings page.
 * 
 * Copyright 2006-2007 by Gilles Caulier
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
#include <qcombobox.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>

// LibKExiv2 includes. 

#include <libkexiv2/libkexiv2.h>

// Local includes.

#include "metadatacheckbox.h"
#include "exifadjust.h"
#include "exifadjust.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFAdjustPriv
{
public:

    EXIFAdjustPriv()
    {
        brightnessCheck     = 0;
        gainControlCheck    = 0;
        contrastCheck       = 0;
        saturationCheck     = 0;
        sharpnessCheck      = 0;
        customRenderedCheck = 0;
        brightnessEdit      = 0;
        gainControlCB       = 0;
        contrastCB          = 0;
        saturationCB        = 0;
        sharpnessCB         = 0;
        customRenderedCB    = 0;
    }

    QCheckBox        *brightnessCheck;

    QComboBox        *gainControlCB;
    QComboBox        *contrastCB;
    QComboBox        *saturationCB;
    QComboBox        *sharpnessCB;
    QComboBox        *customRenderedCB;
   
    KDoubleSpinBox   *brightnessEdit;

    MetadataCheckBox *gainControlCheck;
    MetadataCheckBox *contrastCheck;
    MetadataCheckBox *saturationCheck;
    MetadataCheckBox *sharpnessCheck;
    MetadataCheckBox *customRenderedCheck;
};

EXIFAdjust::EXIFAdjust(QWidget* parent)
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

    d->gainControlCheck = new MetadataCheckBox(i18n("Gain Control:"), parent);
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

    // --------------------------------------------------------

    d->contrastCheck = new MetadataCheckBox(i18n("Contrast:"), parent);
    d->contrastCB    = new QComboBox(false, parent);
    d->contrastCB->insertItem(i18n("Normal"), 0);
    d->contrastCB->insertItem(i18n("Soft"),   1);
    d->contrastCB->insertItem(i18n("Hard"),   2);
    grid->addMultiCellWidget(d->contrastCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->contrastCB, 2, 2, 2, 2);
    QWhatsThis::add(d->contrastCB, i18n("<p>Set here the direction of contrast processing "
                                        "applied by the camera to take the picture."));

    // --------------------------------------------------------

    d->saturationCheck = new MetadataCheckBox(i18n("Saturation:"), parent);
    d->saturationCB    = new QComboBox(false, parent);
    d->saturationCB->insertItem(i18n("Normal"), 0);
    d->saturationCB->insertItem(i18n("Low"),    1);
    d->saturationCB->insertItem(i18n("High"),   2);
    grid->addMultiCellWidget(d->saturationCheck, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->saturationCB, 3, 3, 2, 2);
    QWhatsThis::add(d->saturationCB, i18n("<p>Set here the direction of saturation processing "
                                          "applied by the camera to take the picture."));

    // --------------------------------------------------------

    d->sharpnessCheck = new MetadataCheckBox(i18n("Sharpness:"), parent);
    d->sharpnessCB    = new QComboBox(false, parent);
    d->sharpnessCB->insertItem(i18n("Normal"), 0);
    d->sharpnessCB->insertItem(i18n("Soft"),   1);
    d->sharpnessCB->insertItem(i18n("Hard"),   2);
    grid->addMultiCellWidget(d->sharpnessCheck, 4, 4, 0, 0);
    grid->addMultiCellWidget(d->sharpnessCB, 4, 4, 2, 2);
    QWhatsThis::add(d->sharpnessCB, i18n("<p>Set here the direction of sharpness processing "
                                         "applied by the camera to take the picture."));

    // --------------------------------------------------------

    d->customRenderedCheck = new MetadataCheckBox(i18n("Custom rendered:"), parent);
    d->customRenderedCB    = new QComboBox(false, parent);
    d->customRenderedCB->insertItem(i18n("Normal process"), 0);
    d->customRenderedCB->insertItem(i18n("Custom process"), 1);
    grid->addMultiCellWidget(d->customRenderedCheck, 5, 5, 0, 0);
    grid->addMultiCellWidget(d->customRenderedCB, 5, 5, 2, 2);
    QWhatsThis::add(d->customRenderedCB, i18n("<p>Set here the use of special processing on "
                                              "image data, such as rendering geared to output."));

    grid->setColStretch(1, 10);                     
    grid->setRowStretch(6, 10);                     

    // --------------------------------------------------------

    connect(d->brightnessCheck, SIGNAL(toggled(bool)),
            d->brightnessEdit, SLOT(setEnabled(bool)));

    connect(d->gainControlCheck, SIGNAL(toggled(bool)),
            d->gainControlCB, SLOT(setEnabled(bool)));

    connect(d->contrastCheck, SIGNAL(toggled(bool)),
            d->contrastCB, SLOT(setEnabled(bool)));

    connect(d->saturationCheck, SIGNAL(toggled(bool)),
            d->saturationCB, SLOT(setEnabled(bool)));

    connect(d->sharpnessCheck, SIGNAL(toggled(bool)),
            d->sharpnessCB, SLOT(setEnabled(bool)));

    connect(d->customRenderedCheck, SIGNAL(toggled(bool)),
            d->customRenderedCB, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->brightnessCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));
    
    connect(d->gainControlCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->contrastCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->saturationCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->sharpnessCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->customRenderedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->gainControlCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->contrastCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->saturationCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->sharpnessCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->customRenderedCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->brightnessEdit, SIGNAL(valueChanged(double)),
            this, SIGNAL(signalModified()));
}

EXIFAdjust::~EXIFAdjust()
{
    delete d;
}

void EXIFAdjust::readMetadata(QByteArray& exifData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;
    long     val=0;
    
    d->brightnessEdit->setValue(0.0);
    d->brightnessCheck->setChecked(false);
    if (exiv2Iface.getExifTagRational("Exif.Photo.BrightnessValue", num, den))
    {
        d->brightnessEdit->setValue((double)(num) / (double)(den));
        d->brightnessCheck->setChecked(true);
    }
    d->brightnessEdit->setEnabled(d->brightnessCheck->isChecked());

    d->gainControlCB->setCurrentItem(0);
    d->gainControlCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.GainControl", val))
    {
        if (val >= 0 && val <= 4)
        {
            d->gainControlCB->setCurrentItem(val);
            d->gainControlCheck->setChecked(true);
        }
        else
            d->gainControlCheck->setValid(false);
    }
    d->gainControlCB->setEnabled(d->gainControlCheck->isChecked());
    
    d->contrastCB->setCurrentItem(0);
    d->contrastCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.Contrast", val))
    {
        if (val >= 0 && val <= 2)
        {
            d->contrastCB->setCurrentItem(val);
            d->contrastCheck->setChecked(true);
        }
        else
            d->contrastCheck->setValid(false);
    }
    d->contrastCB->setEnabled(d->contrastCheck->isChecked());

    d->saturationCB->setCurrentItem(0);
    d->saturationCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.Saturation", val))
    {
        if (val >= 0 && val <= 2)
        {
            d->saturationCB->setCurrentItem(val);
            d->saturationCheck->setChecked(true);
        }
        else
            d->saturationCheck->setValid(false);
    }
    d->saturationCB->setEnabled(d->saturationCheck->isChecked());

    d->sharpnessCB->setCurrentItem(0);
    d->sharpnessCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.Sharpness", val))
    {
        if (val >= 0 && val <= 2)
        {
            d->sharpnessCB->setCurrentItem(val);
            d->sharpnessCheck->setChecked(true);
        }
        else
            d->sharpnessCheck->setValid(false);
    }
    d->sharpnessCB->setEnabled(d->sharpnessCheck->isChecked());

    d->customRenderedCB->setCurrentItem(0);
    d->customRenderedCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.CustomRendered", val))
    {
        if (val >= 0 && val <= 1)
        {
            d->customRenderedCB->setCurrentItem(val);
            d->customRenderedCheck->setChecked(true);
        }
        else
            d->customRenderedCheck->setValid(false);
    }
    d->customRenderedCB->setEnabled(d->customRenderedCheck->isChecked());

    blockSignals(false);
}

void EXIFAdjust::applyMetadata(QByteArray& exifData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
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
    else if (d->gainControlCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.GainControl");

    if (d->contrastCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.Contrast", d->contrastCB->currentItem());
    else if (d->contrastCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.Contrast");

    if (d->saturationCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.Saturation", d->saturationCB->currentItem());
    else if (d->saturationCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.Saturation");

    if (d->sharpnessCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.Sharpness", d->sharpnessCB->currentItem());
    else if (d->sharpnessCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.Sharpness");

    if (d->customRenderedCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.CustomRendered", d->customRenderedCB->currentItem());
    else if (d->customRenderedCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.CustomRendered");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

