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
#include <qvalidator.h>
#include <qcheckbox.h>

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
        exposureTimeCheck   = 0;
        exposureTimeNumEdit = 0;
        exposureTimeDenEdit = 0;
    }

    QCheckBox   *exposureTimeCheck;

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
/*
    d->dateOriginalCheck       = new QCheckBox(i18n("Original date and time"), parent);
    d->dateOriginalSubSecCheck = new QCheckBox(i18n("Original sub-second"), parent);
    d->dateOriginalSel         = new KExposureWidget(parent);
    d->dateOriginalSubSecEdit  = new KIntSpinBox(0, 999, 1, 0, 10, parent);
    d->dateOriginalSel->setExposure(QExposure::currentExposure());
    grid->addMultiCellWidget(d->dateOriginalCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->dateOriginalSubSecCheck, 2, 2, 1, 2);
    grid->addMultiCellWidget(d->dateOriginalSel, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->dateOriginalSubSecEdit, 3, 3, 1, 1);
    QWhatsThis::add(d->dateOriginalSel, i18n("<p>Set here the date and time when the original image "
                                        "data was generated. For a digital still camera the date and "
                                        "time the picture was taken are recorded."));
    QWhatsThis::add(d->dateOriginalSubSecEdit, i18n("<p>Set here the fractions of seconds for the date "
                                               "and time when the original image data was generated."));

    // --------------------------------------------------------

    d->dateDigitalizedCheck       = new QCheckBox(i18n("Digitization date and time"), parent);
    d->dateDigitalizedSubSecCheck = new QCheckBox(i18n("Digitization sub-second"), parent);
    d->dateDigitalizedSel         = new KExposureWidget(parent);
    d->dateDigitalizedSubSecEdit  = new KIntSpinBox(0, 999, 1, 0, 10, parent);
    d->dateDigitalizedSel->setExposure(QExposure::currentExposure());
    grid->addMultiCellWidget(d->dateDigitalizedCheck, 4, 4, 0, 0);
    grid->addMultiCellWidget(d->dateDigitalizedSubSecCheck, 4, 4, 1, 2);
    grid->addMultiCellWidget(d->dateDigitalizedSel, 5, 5, 0, 0);
    grid->addMultiCellWidget(d->dateDigitalizedSubSecEdit, 5, 5, 1, 1);
    QWhatsThis::add(d->dateDigitalizedSel, i18n("<p>Set here the date and time when the image was "
                                           "stored as digital data. If, for example, an image was "
                                           "captured by a digital still camera and at the same "
                                           "time the file was recorded, then Original and Digitization "
                                           "date and time will have the same contents."));
    QWhatsThis::add(d->dateDigitalizedSubSecEdit, i18n("<p>Set here the fractions of seconds for the date "
                                                  "and time when the image was stored as digital data."));
*/

    grid->setColStretch(1, 10);                     
    grid->setRowStretch(6, 10);                     

    // --------------------------------------------------------

    connect(d->exposureTimeCheck, SIGNAL(toggled(bool)),
            d->exposureTimeNumEdit, SLOT(setEnabled(bool)));

    connect(d->exposureTimeCheck, SIGNAL(toggled(bool)),
            d->exposureTimeDenEdit, SLOT(setEnabled(bool)));

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
    int num=0, den=0;
    
    if (exiv2Iface.getExifTagRational("Exif.Photo.ExposureTime", num, den))
    {
        d->exposureTimeNumEdit->setValue(num);
        d->exposureTimeDenEdit->setValue(den);
        d->exposureTimeCheck->setChecked(true);
    }
    d->exposureTimeNumEdit->setEnabled(d->exposureTimeCheck->isChecked());
    d->exposureTimeDenEdit->setEnabled(d->exposureTimeCheck->isChecked());

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

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

