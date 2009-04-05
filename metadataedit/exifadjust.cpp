/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : EXIF adjustments settings page.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "exifadjust.h"
#include "exifadjust.moc"

// C++ includes

#include <cmath>

// Qt includes

#include <QGridLayout>

// KDE includes

#include <kcombobox.h>
#include <kdialog.h>
#include <klocale.h>
#include <knuminput.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "metadatacheckbox.h"

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

    KComboBox        *gainControlCB;
    KComboBox        *contrastCB;
    KComboBox        *saturationCB;
    KComboBox        *sharpnessCB;
    KComboBox        *customRenderedCB;

    QDoubleSpinBox   *brightnessEdit;

    MetadataCheckBox *gainControlCheck;
    MetadataCheckBox *contrastCheck;
    MetadataCheckBox *saturationCheck;
    MetadataCheckBox *sharpnessCheck;
    MetadataCheckBox *customRenderedCheck;
};

EXIFAdjust::EXIFAdjust(QWidget* parent)
          : QWidget(parent), d(new EXIFAdjustPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->brightnessCheck = new QCheckBox(i18n("Brightness (APEX):"), this);
    d->brightnessEdit  = new QDoubleSpinBox(this);
    d->brightnessEdit->setRange(-99.99, 99.99);
    d->brightnessEdit->setSingleStep(0.1);
    d->brightnessEdit->setValue(0.0);
    d->brightnessEdit->setWhatsThis(i18n("Set here the brightness adjustment value in APEX unit "
                                         "used by camera to take the picture."));

    // --------------------------------------------------------

    d->gainControlCheck = new MetadataCheckBox(i18n("Gain Control:"), this);
    d->gainControlCB    = new KComboBox(this);
    d->gainControlCB->insertItem(0, i18n("None"));
    d->gainControlCB->insertItem(1, i18n("Low gain up"));
    d->gainControlCB->insertItem(2, i18n("High gain up"));
    d->gainControlCB->insertItem(3, i18n("Low gain down"));
    d->gainControlCB->insertItem(4, i18n("High gain down"));
    d->gainControlCB->setWhatsThis(i18n("Set here the degree of overall image gain adjustment "
                                        "used by camera to take the picture."));

    // --------------------------------------------------------

    d->contrastCheck = new MetadataCheckBox(i18n("Contrast:"), this);
    d->contrastCB    = new KComboBox(this);
    d->contrastCB->insertItem(0, i18n("Normal"));
    d->contrastCB->insertItem(1, i18n("Soft"));
    d->contrastCB->insertItem(2, i18n("Hard"));
    d->contrastCB->setWhatsThis(i18n("Set here the direction of contrast processing "
                                     "applied by the camera to take the picture."));

    // --------------------------------------------------------

    d->saturationCheck = new MetadataCheckBox(i18n("Saturation:"), this);
    d->saturationCB    = new KComboBox(this);
    d->saturationCB->insertItem(0, i18n("Normal"));
    d->saturationCB->insertItem(1, i18n("Low"));
    d->saturationCB->insertItem(2, i18n("High"));
    d->saturationCB->setWhatsThis(i18n("Set here the direction of saturation processing "
                                       "applied by the camera to take the picture."));

    // --------------------------------------------------------

    d->sharpnessCheck = new MetadataCheckBox(i18n("Sharpness:"), this);
    d->sharpnessCB    = new KComboBox(this);
    d->sharpnessCB->insertItem(0, i18n("Normal"));
    d->sharpnessCB->insertItem(1, i18n("Soft"));
    d->sharpnessCB->insertItem(2, i18n("Hard"));
    d->sharpnessCB->setWhatsThis(i18n("Set here the direction of sharpness processing "
                                      "applied by the camera to take the picture."));

    // --------------------------------------------------------

    d->customRenderedCheck = new MetadataCheckBox(i18n("Custom rendered:"), this);
    d->customRenderedCB    = new KComboBox(this);
    d->customRenderedCB->insertItem(0, i18n("Normal process"));
    d->customRenderedCB->insertItem(1, i18n("Custom process"));
    d->customRenderedCB->setWhatsThis(i18n("Set here the use of special processing on "
                                           "image data, such as rendering geared to output."));

    grid->addWidget(d->brightnessCheck,     0, 0, 1, 1);
    grid->addWidget(d->brightnessEdit,      0, 2, 1, 1);
    grid->addWidget(d->gainControlCheck,    1, 0, 1, 1);
    grid->addWidget(d->gainControlCB,       1, 2, 1, 1);
    grid->addWidget(d->contrastCheck,       2, 0, 1, 1);
    grid->addWidget(d->contrastCB,          2, 2, 1, 1);
    grid->addWidget(d->saturationCheck,     3, 0, 1, 1);
    grid->addWidget(d->saturationCB,        3, 2, 1, 1);
    grid->addWidget(d->sharpnessCheck,      4, 0, 1, 1);
    grid->addWidget(d->sharpnessCB,         4, 2, 1, 1);
    grid->addWidget(d->customRenderedCheck, 5, 0, 1, 1);
    grid->addWidget(d->customRenderedCB,    5, 2, 1, 1);
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(6, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

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

    d->gainControlCB->setCurrentIndex(0);
    d->gainControlCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.GainControl", val))
    {
        if (val >= 0 && val <= 4)
        {
            d->gainControlCB->setCurrentIndex(val);
            d->gainControlCheck->setChecked(true);
        }
        else
            d->gainControlCheck->setValid(false);
    }
    d->gainControlCB->setEnabled(d->gainControlCheck->isChecked());

    d->contrastCB->setCurrentIndex(0);
    d->contrastCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.Contrast", val))
    {
        if (val >= 0 && val <= 2)
        {
            d->contrastCB->setCurrentIndex(val);
            d->contrastCheck->setChecked(true);
        }
        else
            d->contrastCheck->setValid(false);
    }
    d->contrastCB->setEnabled(d->contrastCheck->isChecked());

    d->saturationCB->setCurrentIndex(0);
    d->saturationCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.Saturation", val))
    {
        if (val >= 0 && val <= 2)
        {
            d->saturationCB->setCurrentIndex(val);
            d->saturationCheck->setChecked(true);
        }
        else
            d->saturationCheck->setValid(false);
    }
    d->saturationCB->setEnabled(d->saturationCheck->isChecked());

    d->sharpnessCB->setCurrentIndex(0);
    d->sharpnessCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.Sharpness", val))
    {
        if (val >= 0 && val <= 2)
        {
            d->sharpnessCB->setCurrentIndex(val);
            d->sharpnessCheck->setChecked(true);
        }
        else
            d->sharpnessCheck->setValid(false);
    }
    d->sharpnessCB->setEnabled(d->sharpnessCheck->isChecked());

    d->customRenderedCB->setCurrentIndex(0);
    d->customRenderedCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.CustomRendered", val))
    {
        if (val >= 0 && val <= 1)
        {
            d->customRenderedCB->setCurrentIndex(val);
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
        exiv2Iface.setExifTagLong("Exif.Photo.GainControl", d->gainControlCB->currentIndex());
    else if (d->gainControlCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.GainControl");

    if (d->contrastCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.Contrast", d->contrastCB->currentIndex());
    else if (d->contrastCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.Contrast");

    if (d->saturationCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.Saturation", d->saturationCB->currentIndex());
    else if (d->saturationCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.Saturation");

    if (d->sharpnessCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.Sharpness", d->sharpnessCB->currentIndex());
    else if (d->sharpnessCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.Sharpness");

    if (d->customRenderedCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.CustomRendered", d->customRenderedCB->currentIndex());
    else if (d->customRenderedCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.CustomRendered");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin
