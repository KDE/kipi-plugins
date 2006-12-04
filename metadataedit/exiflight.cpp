/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF light settings page.
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
#include <qmap.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>

// Local includes.

#include "metadatacheckbox.h"
#include "exiv2iface.h"
#include "exiflight.h"
#include "exiflight.moc"

namespace KIPIMetadataEditPlugin
{

    class FlashMode
    {
    public:

        FlashMode() {}
        FlashMode(int id, const QString& desc) : m_id(id), m_desc(desc) {}

        int     id()   const { return m_id;   }
        QString desc() const { return m_desc; }

    private:

        int     m_id;
        QString m_desc;
    };

class EXIFLightPriv
{
public:

    EXIFLightPriv()
    {
        lightSourceCheck     = 0;
        flashModeCheck       = 0;
        flashEnergyCheck     = 0;
        whiteBalanceCheck    = 0;
        lightSourceCB        = 0;
        flashEnergyEdit      = 0;
        flashModeCB          = 0;
        whiteBalanceCB       = 0;

        flashModeMap.insert(0,  FlashMode( 0x00, i18n("No flash") ));
        flashModeMap.insert(1,  FlashMode( 0x01, i18n("Fired") ));
        flashModeMap.insert(2,  FlashMode( 0x05, i18n("Fired, no strobe return light") ));
        flashModeMap.insert(3,  FlashMode( 0x07, i18n("Fired, strobe return light") ));
        flashModeMap.insert(4,  FlashMode( 0x09, i18n("Yes, compulsory") ));
        flashModeMap.insert(5,  FlashMode( 0x0d, i18n("Yes, compulsory, no return light") ));
        flashModeMap.insert(6,  FlashMode( 0x0f, i18n("Yes, compulsory, return light") ));
        flashModeMap.insert(7,  FlashMode( 0x10, i18n("No, compulsory") ));
        flashModeMap.insert(8,  FlashMode( 0x18, i18n("No, auto") ));
        flashModeMap.insert(9,  FlashMode( 0x19, i18n("Yes, auto") ));
        flashModeMap.insert(10, FlashMode( 0x1d, i18n("Yes, auto, no return light") ));
        flashModeMap.insert(11, FlashMode( 0x1f, i18n("Yes, auto, return light") ));
        flashModeMap.insert(12, FlashMode( 0x20, i18n("No flash function") ));
        flashModeMap.insert(13, FlashMode( 0x41, i18n("Yes, red-eye") ));
        flashModeMap.insert(14, FlashMode( 0x45, i18n("Yes, red-eye, no return light") ));
        flashModeMap.insert(15, FlashMode( 0x47, i18n("Yes, red-eye, return light") ));
        flashModeMap.insert(16, FlashMode( 0x49, i18n("Yes, compulsory, red-eye") ));
        flashModeMap.insert(17, FlashMode( 0x4d, i18n("Yes, compulsory, red-eye, no return light") ));
        flashModeMap.insert(18, FlashMode( 0x4f, i18n("Yes, compulsory, red-eye, return light") ));
        flashModeMap.insert(19, FlashMode( 0x59, i18n("Yes, auto, red-eye") ));
        flashModeMap.insert(20, FlashMode( 0x5d, i18n("Yes, auto, red-eye, no return light") ));
        flashModeMap.insert(21, FlashMode( 0x5f, i18n("Yes, auto, red-eye, return light") ));
    }

    typedef QMap<int, FlashMode> FlashModeMap; 

    FlashModeMap      flashModeMap;

    QCheckBox        *flashEnergyCheck;
   
    QComboBox        *lightSourceCB;
    QComboBox        *flashModeCB;
    QComboBox        *whiteBalanceCB;

    KDoubleSpinBox   *flashEnergyEdit;

    MetadataCheckBox *lightSourceCheck;
    MetadataCheckBox *flashModeCheck;
    MetadataCheckBox *whiteBalanceCheck;
};

EXIFLight::EXIFLight(QWidget* parent)
         : QWidget(parent)
{
    d = new EXIFLightPriv;

    QGridLayout* grid = new QGridLayout(parent, 4, 3, KDialog::spacingHint());

    // --------------------------------------------------------

    d->lightSourceCheck = new MetadataCheckBox(i18n("Light source:"), parent);
    d->lightSourceCB    = new QComboBox(false, parent);
    d->lightSourceCB->insertItem(i18n("Unknown"),                                 0);
    d->lightSourceCB->insertItem(i18n("Daylight"),                                1);
    d->lightSourceCB->insertItem(i18n("Fluorescent"),                             2);
    d->lightSourceCB->insertItem(i18n("Tungsten (incandescent light)"),           3);
    d->lightSourceCB->insertItem(i18n("Flash"),                                   4);
    d->lightSourceCB->insertItem(i18n("Fine weather"),                            5);
    d->lightSourceCB->insertItem(i18n("Cloudy weather"),                          6);
    d->lightSourceCB->insertItem(i18n("Shade"),                                   7);
    d->lightSourceCB->insertItem(i18n("Daylight fluorescent (D 5700 - 7100K)"),   8);
    d->lightSourceCB->insertItem(i18n("Day white fluorescent (N 4600 - 5400K)"),  9);
    d->lightSourceCB->insertItem(i18n("Cool white fluorescent (W 3900 - 4500K)"), 10);
    d->lightSourceCB->insertItem(i18n("White fluorescent (WW 3200 - 3700K)"),     11);
    d->lightSourceCB->insertItem(i18n("Standard light A"),                        12);
    d->lightSourceCB->insertItem(i18n("Standard light B"),                        13);
    d->lightSourceCB->insertItem(i18n("Standard light C"),                        14);
    d->lightSourceCB->insertItem(i18n("D55"),                                     15);
    d->lightSourceCB->insertItem(i18n("D65"),                                     16);
    d->lightSourceCB->insertItem(i18n("D75"),                                     17);
    d->lightSourceCB->insertItem(i18n("D50"),                                     18);
    d->lightSourceCB->insertItem(i18n("ISO studio tungsten"),                     19);
    d->lightSourceCB->insertItem(i18n("Other light source"),                      20);
    grid->addMultiCellWidget(d->lightSourceCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->lightSourceCB, 0, 0, 2, 3);
    QWhatsThis::add(d->lightSourceCB, i18n("<p>Select here the kind of light source used "
                                           "to take the picture."));

    // --------------------------------------------------------

    d->flashModeCheck = new MetadataCheckBox(i18n("Flash mode:"), parent);
    d->flashModeCB    = new QComboBox(false, parent);

    for (EXIFLightPriv::FlashModeMap::Iterator it = d->flashModeMap.begin();
        it != d->flashModeMap.end(); ++it )
       d->flashModeCB->insertItem(it.data().desc());

    grid->addMultiCellWidget(d->flashModeCheck, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->flashModeCB, 1, 1, 2, 3);
    QWhatsThis::add(d->flashModeCB, i18n("<p>Select here the flash program mode used by camera "
                                         "to take the picture."));

    // --------------------------------------------------------

    d->flashEnergyCheck = new QCheckBox(i18n("Flash energy (BCPS):"), parent);
    d->flashEnergyEdit  = new KDoubleSpinBox(1, 10000, 1, 1, 1, parent);
    grid->addMultiCellWidget(d->flashEnergyCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->flashEnergyEdit, 2, 2, 2, 2);
    QWhatsThis::add(d->flashEnergyEdit, i18n("<p>Set here the flash energy used to take the picture "
                                             "in BCPS unit. Beam Candle Power Seconds is the measure "
                                             "of effective intensity of a light source when it is "
                                             "focused into a beam by a reflector or lens. This value "
                                             "is the effective intensity for a period of one second."));

    // --------------------------------------------------------

    d->whiteBalanceCheck = new MetadataCheckBox(i18n("White balance:"), parent);
    d->whiteBalanceCB    = new QComboBox(false, parent);
    d->whiteBalanceCB->insertItem(i18n("Auto"),   0);
    d->whiteBalanceCB->insertItem(i18n("Manual"), 1);
    grid->addMultiCellWidget(d->whiteBalanceCheck, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->whiteBalanceCB, 3, 3, 2, 2);
    QWhatsThis::add(d->whiteBalanceCB, i18n("<p>Select here the white balance mode set by camera when "
                                           "the picture have been shot."));


    grid->setColStretch(1, 10);                     
    grid->setRowStretch(4, 10);                     

    // --------------------------------------------------------

    connect(d->lightSourceCheck, SIGNAL(toggled(bool)),
            d->lightSourceCB, SLOT(setEnabled(bool)));

    connect(d->flashModeCheck, SIGNAL(toggled(bool)),
            d->flashModeCB, SLOT(setEnabled(bool)));

    connect(d->flashEnergyCheck, SIGNAL(toggled(bool)),
            d->flashEnergyEdit, SLOT(setEnabled(bool)));

    connect(d->whiteBalanceCheck, SIGNAL(toggled(bool)),
            d->whiteBalanceCB, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->flashEnergyCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->lightSourceCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->flashModeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->whiteBalanceCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->lightSourceCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->flashModeCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->whiteBalanceCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->flashEnergyEdit, SIGNAL(valueChanged(double)),
            this, SIGNAL(signalModified()));
}

EXIFLight::~EXIFLight()
{
    delete d;
}

void EXIFLight::readMetadata(QByteArray& exifData)
{
    blockSignals(true);
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;
    long     val=0;

    d->lightSourceCB->setCurrentItem(0);
    d->lightSourceCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.LightSource", val))
    {
        if ((val>=0 && val <=4) || (val> 8 && val <16) || (val> 16 && val <25) || val == 255) 
        {    
            if (val > 8 && val < 16)
                val = val - 4;
            else if (val > 16 && val < 25)
                val = val - 5;
            else if (val == 255)    
                val = 20;
    
            d->lightSourceCB->setCurrentItem(val);
            d->lightSourceCheck->setChecked(true);
        }
        else 
            d->lightSourceCheck->setValid(false);
    }
    d->lightSourceCB->setEnabled(d->lightSourceCheck->isChecked());

    d->flashModeCB->setCurrentItem(0);
    d->flashModeCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.Flash", val))
    {
        int item = -1;    
        for (EXIFLightPriv::FlashModeMap::Iterator it = d->flashModeMap.begin();
            it != d->flashModeMap.end(); ++it )
        {
            if (it.data().id() == val)
                item = it.key();
        }
        
        if (item != -1)
        {
            d->flashModeCB->setCurrentItem(item);
            d->flashModeCheck->setChecked(true);
        }
        else
            d->flashModeCheck->setValid(false);
    }
    d->flashModeCB->setEnabled(d->flashModeCheck->isChecked());

    d->flashEnergyEdit->setValue(1.0);
    d->flashEnergyCheck->setChecked(false);
    if (exiv2Iface.getExifTagRational("Exif.Photo.FlashEnergy", num, den))
    {
        d->flashEnergyEdit->setValue((double)(num) / (double)(den));
        d->flashEnergyCheck->setChecked(true);
    }
    d->flashEnergyEdit->setEnabled(d->flashEnergyCheck->isChecked());

    d->whiteBalanceCB->setCurrentItem(0);
    d->whiteBalanceCheck->setChecked(false);
    if (exiv2Iface.getExifTagLong("Exif.Photo.WhiteBalance", val))
    {
        if (val>=0 && val<=1)
        {
            d->whiteBalanceCB->setCurrentItem(val);
            d->whiteBalanceCheck->setChecked(true);
        }
        else
            d->whiteBalanceCheck->setValid(false);
    }
    d->whiteBalanceCB->setEnabled(d->whiteBalanceCheck->isChecked());

    blockSignals(false);
}

void EXIFLight::applyMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;

    if (d->lightSourceCheck->isChecked())
    {
        long val = d->lightSourceCB->currentItem();
        if (val > 4 && val < 12)
            val = val + 4;
        else if (val > 11 && val < 20)
            val = val + 5;
        else if (val == 20)    
            val = 255;

        exiv2Iface.setExifTagLong("Exif.Photo.LightSource", val);
    }
    else if (d->lightSourceCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.LightSource");

    if (d->flashModeCheck->isChecked())
    {
        long val = d->flashModeCB->currentItem();
        exiv2Iface.setExifTagLong("Exif.Photo.Flash", d->flashModeMap[val].id());
    }
    else if (d->flashModeCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.Flash");

    if (d->flashEnergyCheck->isChecked())
    {
        exiv2Iface.convertToRational(d->flashEnergyEdit->value(), &num, &den, 1);
        exiv2Iface.setExifTagRational("Exif.Photo.FlashEnergy", num, den);
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.FlashEnergy");

    if (d->whiteBalanceCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.WhiteBalance", d->whiteBalanceCB->currentItem());
    else if (d->whiteBalanceCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.WhiteBalance");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

