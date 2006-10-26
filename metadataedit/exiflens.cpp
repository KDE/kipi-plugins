/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF lens settings page.
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
#include <qcombobox.h>
#include <qlistbox.h>
#include <qpair.h>
#include <qmap.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>

// Local includes.

#include "metadatacheckbox.h"
#include "exiv2iface.h"
#include "exiflens.h"
#include "exiflens.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFLensPriv
{
public:

    EXIFLensPriv()
    {
        apertureCheck         = 0;
        maxApertureCheck      = 0;
        focalLength35mmCheck  = 0;
        focalLengthCheck      = 0;
        digitalZoomRatioCheck = 0;
        apertureCB            = 0;
        maxApertureCB         = 0;
        focalLength35mmEdit   = 0;
        focalLengthEdit       = 0;
        digitalZoomRatioEdit  = 0;

        apertureValues.append("f/1.0");
        apertureValues.append("f/1.1");
        apertureValues.append("f/1.2");
        apertureValues.append("f/1.3");
        apertureValues.append("f/1.4");
        apertureValues.append("f/1.6");
        apertureValues.append("f/1.8");
        apertureValues.append("f/2.0");
        apertureValues.append("f/2.2");
        apertureValues.append("f/2.5");
        apertureValues.append("f/2.8");
        apertureValues.append("f/3.2");
        apertureValues.append("f/3.5");
        apertureValues.append("f/3.6");
        apertureValues.append("f/4.0");
        apertureValues.append("f/4.5");
        apertureValues.append("f/4.9");
        apertureValues.append("f/5.0");
        apertureValues.append("f/5.6");
        apertureValues.append("f/5.7");
        apertureValues.append("f/6.3");
        apertureValues.append("f/7.0");
        apertureValues.append("f/7.1");
        apertureValues.append("f/8.0");
        apertureValues.append("f/9.0");
        apertureValues.append("f/10.0");
        apertureValues.append("f/10.1");
        apertureValues.append("f/11.0");
        apertureValues.append("f/11.3");
        apertureValues.append("f/12.0");
        apertureValues.append("f/12.7");
        apertureValues.append("f/13.0");
        apertureValues.append("f/14.0");
        apertureValues.append("f/14.3");
        apertureValues.append("f/16.0");
        apertureValues.append("f/18.0");
        apertureValues.append("f/20.0");
        apertureValues.append("f/20.2");
        apertureValues.append("f/22.0");
        apertureValues.append("f/22.6");
        apertureValues.append("f/25.0");
        apertureValues.append("f/25.4");
        apertureValues.append("f/28.5");
        apertureValues.append("f/29.0");
        apertureValues.append("f/32.0");
        apertureValues.append("f/35.9");
        apertureValues.append("f/36.0");
        apertureValues.append("f/40.0");
        apertureValues.append("f/40.3");
        apertureValues.append("f/45.0");
        apertureValues.append("f/45.3");
        apertureValues.append("f/50.8");
        apertureValues.append("f/51.0");
        apertureValues.append("f/57.0");
        apertureValues.append("f/64.0");
        apertureValues.append("f/72.0");
        apertureValues.append("f/81.0");
        apertureValues.append("f/91.0");
    }

    QStringList       apertureValues;

    QCheckBox        *focalLength35mmCheck;
    QCheckBox        *focalLengthCheck;
    QCheckBox        *digitalZoomRatioCheck;
   
    QComboBox        *apertureCB;
    QComboBox        *maxApertureCB;

    KIntSpinBox      *focalLength35mmEdit;

    KDoubleSpinBox   *focalLengthEdit;
    KDoubleSpinBox   *digitalZoomRatioEdit;

    MetadataCheckBox *apertureCheck;
    MetadataCheckBox *maxApertureCheck;
};

EXIFLens::EXIFLens(QWidget* parent, QByteArray& exifData)
        : QWidget(parent)
{
    d = new EXIFLensPriv;

    QGridLayout* grid = new QGridLayout(parent, 5, 2, KDialog::spacingHint());

    // --------------------------------------------------------

    d->focalLengthCheck = new QCheckBox(i18n("Focal length (mm):"), parent);
    d->focalLengthEdit  = new KDoubleSpinBox(1.0, 10000.0, 1.0, 50.0, 1, parent);
    grid->addMultiCellWidget(d->focalLengthCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->focalLengthEdit, 0, 0, 2, 2);
    QWhatsThis::add(d->focalLengthEdit, i18n("<p>Set here the lens focal lenght in milimeters "
                                             "used by camera to take the picture."));

    // --------------------------------------------------------

    d->focalLength35mmCheck = new QCheckBox(i18n("Focal length in 35mm film (mm):"), parent);
    d->focalLength35mmEdit  = new KIntSpinBox(1, 10000, 1, 1, 10, parent);
    grid->addMultiCellWidget(d->focalLength35mmCheck, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->focalLength35mmEdit, 1, 1, 2, 2);
    QWhatsThis::add(d->focalLength35mmEdit, i18n("<p>Set here equivalent focal length assuming "
                                            "a 35mm film camera, in mm. A value of 0 means the focal "
                                            "length is unknown."));

    // --------------------------------------------------------

    d->digitalZoomRatioCheck = new QCheckBox(i18n("Digital zoom ratio:"), parent);
    d->digitalZoomRatioEdit  = new KDoubleSpinBox(0.0, 100.0, 0.1, 1.0, 1, parent);
    grid->addMultiCellWidget(d->digitalZoomRatioCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->digitalZoomRatioEdit, 2, 2, 2, 2);
    QWhatsThis::add(d->digitalZoomRatioEdit, i18n("<p>Set here the digital zoom ratio "
                                             "used by camera to take the picture."));

    // --------------------------------------------------------

    d->apertureCheck = new MetadataCheckBox(i18n("Lens aperture (f-number):"), parent);
    d->apertureCB    = new QComboBox(false, parent);
    d->apertureCB->insertStringList(d->apertureValues);
    grid->addMultiCellWidget(d->apertureCheck, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->apertureCB, 3, 3, 2, 2);
    QWhatsThis::add(d->apertureCB, i18n("<p>Select here the lens aperture used by camera "
                                        "to take the picture."));

    // --------------------------------------------------------

    d->maxApertureCheck = new MetadataCheckBox(i18n("Max. lens aperture (f-number):"), parent);
    d->maxApertureCB    = new QComboBox(false, parent);
    d->maxApertureCB->insertStringList(d->apertureValues);
    grid->addMultiCellWidget(d->maxApertureCheck, 4, 4, 0, 0);
    grid->addMultiCellWidget(d->maxApertureCB, 4, 4, 2, 2);
    QWhatsThis::add(d->maxApertureCB, i18n("<p>Select here the smallest aperture of the lens used by camera "
                                           "to take the picture."));

    grid->setColStretch(1, 10);                     
    grid->setRowStretch(5, 10);                     

    // --------------------------------------------------------

    connect(d->focalLengthCheck, SIGNAL(toggled(bool)),
            d->focalLengthEdit, SLOT(setEnabled(bool)));

    connect(d->focalLength35mmCheck, SIGNAL(toggled(bool)),
            d->focalLength35mmEdit, SLOT(setEnabled(bool)));

    connect(d->digitalZoomRatioCheck, SIGNAL(toggled(bool)),
            d->digitalZoomRatioEdit, SLOT(setEnabled(bool)));

    connect(d->apertureCheck, SIGNAL(toggled(bool)),
            d->apertureCB, SLOT(setEnabled(bool)));

    connect(d->maxApertureCheck, SIGNAL(toggled(bool)),
            d->maxApertureCB, SLOT(setEnabled(bool)));

    // --------------------------------------------------------
    
    readMetadata(exifData);
}

EXIFLens::~EXIFLens()
{
    delete d;
}

void EXIFLens::readMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;
    long     val=0;

    if (exiv2Iface.getExifTagRational("Exif.Photo.FocalLength", num, den))
    {
        d->focalLengthEdit->setValue((double)(num) / (double)(den));
        d->focalLengthCheck->setChecked(true);
    }
    d->focalLengthEdit->setEnabled(d->focalLengthCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.FocalLengthIn35mmFilm", val))
    {
        d->focalLength35mmEdit->setValue(val);
        d->focalLength35mmCheck->setChecked(true);
    }
    d->focalLength35mmEdit->setEnabled(d->focalLength35mmCheck->isChecked());

    if (exiv2Iface.getExifTagRational("Exif.Photo.DigitalZoomRatio", num, den))
    {
        d->digitalZoomRatioEdit->setValue((num == 0) ? 0.0 : (double)(num) / (double)(den));
        d->digitalZoomRatioCheck->setChecked(true);
    }
    d->digitalZoomRatioEdit->setEnabled(d->digitalZoomRatioCheck->isChecked());

    if (exiv2Iface.getExifTagRational("Exif.Photo.FNumber", num, den))
    {
        QString fnumber = QString::number((double)(num)/(double)(den), 'f', 1);

        int item = -1;
        for (int i = 0 ; i < d->apertureCB->count() ; i++)
        {
            if (d->apertureCB->text(i).remove(0, 2) == fnumber)
                item = i;
        }

        if (item != -1)
        {
            d->apertureCB->setCurrentItem(item);
            d->apertureCheck->setChecked(true);
        }
    }
    else if (exiv2Iface.getExifTagRational("Exif.Photo.ApertureValue", num, den))
    {
        double aperture = pow(2.0, ((double)(num)/(double)(den))/2.0);

        QString fnumber = QString::number(aperture, 'f', 1);

        int item = -1;
        for (int i = 0 ; i < d->apertureCB->count() ; i++)
        {
            if (d->apertureCB->text(i).remove(0, 2) == fnumber)
                item = i;
        }

        if (item != -1)
        {
            d->apertureCB->setCurrentItem(item);
            d->apertureCheck->setChecked(true);
        }
        else
            d->apertureCheck->setValid(false);
    }
    d->apertureCB->setEnabled(d->apertureCheck->isChecked());
    
    if (exiv2Iface.getExifTagRational("Exif.Photo.MaxApertureValue", num, den))
    {
        double maxAperture = pow(2.0, ((double)(num)/(double)(den))/2.0);

        QString fnumber = QString::number(maxAperture, 'f', 1);

        int item = -1;
        for (int i = 0 ; i < d->apertureCB->count() ; i++)
        {
            if (d->maxApertureCB->text(i).remove(0, 2) == fnumber)
                item = i;
        }

        if (item != -1)
        {
            d->maxApertureCB->setCurrentItem(item);
            d->maxApertureCheck->setChecked(true);
        }
        else
            d->maxApertureCheck->setValid(false);
    }
    d->maxApertureCB->setEnabled(d->maxApertureCheck->isChecked());
}

void EXIFLens::applyMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;

    if (d->focalLengthCheck->isChecked())
    {
        exiv2Iface.convertToRational(d->focalLengthEdit->value(), &num, &den, 1);
        exiv2Iface.setExifTagRational("Exif.Photo.FocalLength", num, den);
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.FocalLength");

    if (d->focalLength35mmCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.FocalLengthIn35mmFilm", d->focalLength35mmEdit->value());
    else
        exiv2Iface.removeExifTag("Exif.Photo.FocalLengthIn35mmFilm");

    if (d->digitalZoomRatioCheck->isChecked())
    {
        exiv2Iface.convertToRational(d->digitalZoomRatioEdit->value(), &num, &den, 1);
        exiv2Iface.setExifTagRational("Exif.Photo.DigitalZoomRatio", num, den);
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.DigitalZoomRatio");

    if (d->apertureCheck->isChecked())
    {
        exiv2Iface.convertToRational(d->apertureCB->currentText().remove(0, 2).toDouble(), &num, &den, 1);
        exiv2Iface.setExifTagRational("Exif.Photo.FNumber", num, den);

        double fnumber  = d->apertureCB->currentText().remove(0, 2).toDouble();
        double aperture = 2.0*(log(fnumber)/log(2.0));
        exiv2Iface.convertToRational(aperture, &num, &den, 8);
        exiv2Iface.setExifTagRational("Exif.Photo.ApertureValue", num, den);
    }
    else if (d->apertureCheck->isValid())
    {
        exiv2Iface.removeExifTag("Exif.Photo.FNumber");
        exiv2Iface.removeExifTag("Exif.Photo.ApertureValue");
    }

    if (d->maxApertureCheck->isChecked())
    {
        double fnumber  = d->maxApertureCB->currentText().remove(0, 2).toDouble();
        double aperture = 2.0*(log(fnumber)/log(2.0));
        exiv2Iface.convertToRational(aperture, &num, &den, 8);
        exiv2Iface.setExifTagRational("Exif.Photo.MaxApertureValue", num, den);
    }
    else if (d->maxApertureCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.MaxApertureValue");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

