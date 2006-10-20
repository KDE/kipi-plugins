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
#include <qlistbox.h>
#include <qpair.h>
#include <qmap.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>

// Local includes.

#include "exiv2iface.h"
#include "exifphoto.h"
#include "exifphoto.moc"

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

class EXIFPhotoPriv
{
public:

    EXIFPhotoPriv()
    {
        apertureCheck        = 0;
        exposureTimeCheck    = 0;
        exposureProgramCheck = 0;
        exposureModeCheck    = 0;
        ISOSpeedCheck        = 0;
        meteringModeCheck    = 0;
        lightSourceCheck     = 0;
        focalLengthCheck     = 0;
        flashModeCheck       = 0;
        apertureCB           = 0;
        exposureProgramCB    = 0;
        exposureModeCB       = 0;
        ISOSpeedCB           = 0;
        meteringModeCB       = 0;
        lightSourceCB        = 0;
        exposureTimeNumEdit  = 0;
        exposureTimeDenEdit  = 0;
        focalLengthEdit      = 0;
        flashModeCB          = 0;

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

    FlashModeMap   flashModeMap;

    QCheckBox      *apertureCheck;
    QCheckBox      *exposureTimeCheck;
    QCheckBox      *exposureProgramCheck;
    QCheckBox      *exposureModeCheck;
    QCheckBox      *ISOSpeedCheck;
    QCheckBox      *meteringModeCheck;
    QCheckBox      *lightSourceCheck;
    QCheckBox      *focalLengthCheck;
    QCheckBox      *flashModeCheck;
   
    QComboBox      *apertureCB;
    QComboBox      *exposureProgramCB;
    QComboBox      *exposureModeCB;
    QComboBox      *ISOSpeedCB;
    QComboBox      *meteringModeCB;
    QComboBox      *lightSourceCB;
    QComboBox      *flashModeCB;

    KIntSpinBox    *exposureTimeNumEdit;
    KIntSpinBox    *exposureTimeDenEdit;

    KDoubleSpinBox *focalLengthEdit;
};

EXIFPhoto::EXIFPhoto(QWidget* parent, QByteArray& exifData)
            : QWidget(parent)
{
    d = new EXIFPhotoPriv;

    QGridLayout* grid = new QGridLayout(parent, 9, 5, KDialog::spacingHint());

    // --------------------------------------------------------

    d->focalLengthCheck = new QCheckBox(i18n("Focal Length (mm):"), parent);
    d->focalLengthEdit  = new KDoubleSpinBox(1, 10000, 1, 50, 1, parent);
    grid->addMultiCellWidget(d->focalLengthCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->focalLengthEdit, 0, 0, 2, 2);
    QWhatsThis::add(d->focalLengthEdit, i18n("<p>Set here the lens focal lenght in milimeters "
                                             "used by camera to take the picture."));

    // --------------------------------------------------------

    d->apertureCheck = new QCheckBox(i18n("Aperture (f-number):"), parent);
    d->apertureCB    = new QComboBox(false, parent);
    d->apertureCB->insertItem("1.0");
    d->apertureCB->insertItem("1.1");
    d->apertureCB->insertItem("1.2");
    d->apertureCB->insertItem("1.3");
    d->apertureCB->insertItem("1.4");
    d->apertureCB->insertItem("1.6");
    d->apertureCB->insertItem("1.8");
    d->apertureCB->insertItem("2.0");
    d->apertureCB->insertItem("2.2");
    d->apertureCB->insertItem("2.5");
    d->apertureCB->insertItem("2.8");
    d->apertureCB->insertItem("3.2");
    d->apertureCB->insertItem("3.5");
    d->apertureCB->insertItem("3.6");
    d->apertureCB->insertItem("4.0");
    d->apertureCB->insertItem("4.5");
    d->apertureCB->insertItem("4.9");
    d->apertureCB->insertItem("5.0");
    d->apertureCB->insertItem("5.6");
    d->apertureCB->insertItem("5.7");
    d->apertureCB->insertItem("6.3");
    d->apertureCB->insertItem("7.0");
    d->apertureCB->insertItem("7.1");
    d->apertureCB->insertItem("8.0");
    d->apertureCB->insertItem("9.0");
    d->apertureCB->insertItem("10.0");
    d->apertureCB->insertItem("10.1");
    d->apertureCB->insertItem("11.0");
    d->apertureCB->insertItem("11.3");
    d->apertureCB->insertItem("12.0");
    d->apertureCB->insertItem("12.7");
    d->apertureCB->insertItem("13.0");
    d->apertureCB->insertItem("14.0");
    d->apertureCB->insertItem("14.3");
    d->apertureCB->insertItem("16.0");
    d->apertureCB->insertItem("18.0");
    d->apertureCB->insertItem("20.0");
    d->apertureCB->insertItem("20.2");
    d->apertureCB->insertItem("22.0");
    d->apertureCB->insertItem("22.6");
    d->apertureCB->insertItem("25.0");
    d->apertureCB->insertItem("25.4");
    d->apertureCB->insertItem("28.5");
    d->apertureCB->insertItem("29.0");
    d->apertureCB->insertItem("32.0");
    d->apertureCB->insertItem("35.9");
    d->apertureCB->insertItem("36.0");
    d->apertureCB->insertItem("40.0");
    d->apertureCB->insertItem("40.3");
    d->apertureCB->insertItem("45.0");
    d->apertureCB->insertItem("45.3");
    d->apertureCB->insertItem("50.8");
    d->apertureCB->insertItem("51.0");
    d->apertureCB->insertItem("57.0");
    d->apertureCB->insertItem("64.0");
    d->apertureCB->insertItem("72.0");
    d->apertureCB->insertItem("81.0");
    d->apertureCB->insertItem("91.0");
    grid->addMultiCellWidget(d->apertureCheck, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->apertureCB, 1, 1, 2, 2);
    QWhatsThis::add(d->apertureCB, i18n("<p>Select here the lens aperture used by camera "
                                        "to take the picture."));

    // --------------------------------------------------------

    d->exposureTimeCheck   = new QCheckBox(i18n("Exposure time (seconds):"), parent);
    d->exposureTimeNumEdit = new KIntSpinBox(1, 100000, 1, 1, 10, parent);
    d->exposureTimeDenEdit = new KIntSpinBox(1, 100000, 1, 1, 10, parent);
    QLabel *exposureLabel  = new QLabel("/", parent);
    exposureLabel->setAlignment (Qt::AlignRight|Qt::AlignVCenter);
    grid->addMultiCellWidget(d->exposureTimeCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->exposureTimeNumEdit, 2, 2, 2, 2);
    grid->addMultiCellWidget(exposureLabel, 2, 2, 3, 3);
    grid->addMultiCellWidget(d->exposureTimeDenEdit, 2, 2, 4, 4);
    QWhatsThis::add(d->exposureTimeCheck, i18n("<p>Set on this option to set the exposure time "
                    "of picture, given in seconds."));

    // --------------------------------------------------------

    d->exposureProgramCheck = new QCheckBox(i18n("Exposure program:"), parent);
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
    grid->addMultiCellWidget(d->exposureProgramCheck, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->exposureProgramCB, 3, 3, 2, 5);
    QWhatsThis::add(d->exposureProgramCB, i18n("<p>Select here the program used by the camera "
                                          "to set exposure when the picture have been taken."));

    // --------------------------------------------------------

    d->exposureModeCheck = new QCheckBox(i18n("Exposure mode:"), parent);
    d->exposureModeCB    = new QComboBox(false, parent);
    d->exposureModeCB->insertItem(i18n("Auto"),         0);
    d->exposureModeCB->insertItem(i18n("Manual"),       1);
    d->exposureModeCB->insertItem(i18n("Auto bracket"), 2);
    grid->addMultiCellWidget(d->exposureModeCheck, 4, 4, 0, 0);
    grid->addMultiCellWidget(d->exposureModeCB, 4, 4, 2, 5);
    QWhatsThis::add(d->exposureModeCB, i18n("<p>Select here the mode used by the camera "
                                       "to set exposure when the picture have been shot. "
                                       "In auto-bracketing mode, the camera shoots a "
                                       "series of frames of the same scene at different "
                                       "exposure settings."));

    // --------------------------------------------------------

    d->meteringModeCheck = new QCheckBox(i18n("Metering mode:"), parent);
    d->meteringModeCB    = new QComboBox(false, parent);
    d->meteringModeCB->insertItem(i18n("Unknown"),                 0);
    d->meteringModeCB->insertItem(i18n("Average"),                 1);
    d->meteringModeCB->insertItem(i18n("Center weighted average"), 2);
    d->meteringModeCB->insertItem(i18n("Spot"),                    3);
    d->meteringModeCB->insertItem(i18n("Multi-spot"),              4);
    d->meteringModeCB->insertItem(i18n("Multi-segment"),           5);
    d->meteringModeCB->insertItem(i18n("Partial"),                 6);
    d->meteringModeCB->insertItem(i18n("Other"),                   7);
    grid->addMultiCellWidget(d->meteringModeCheck, 5, 5, 0, 0);
    grid->addMultiCellWidget(d->meteringModeCB, 5, 5, 2, 5);
    QWhatsThis::add(d->meteringModeCB, i18n("<p>Select here the metering mode used by the camera "
                                       "to set exposure when the picture have been shot."));

    // --------------------------------------------------------

    d->ISOSpeedCheck = new QCheckBox(i18n("Sensitivity (ISO):"), parent);
    d->ISOSpeedCB    = new QComboBox(false, parent);
    d->ISOSpeedCB->insertItem("10",    0);
    d->ISOSpeedCB->insertItem("12",    1);
    d->ISOSpeedCB->insertItem("16",    2);
    d->ISOSpeedCB->insertItem("20",    3);
    d->ISOSpeedCB->insertItem("25",    4);
    d->ISOSpeedCB->insertItem("32",    5);
    d->ISOSpeedCB->insertItem("40",    6);
    d->ISOSpeedCB->insertItem("50",    7);
    d->ISOSpeedCB->insertItem("64",    8);
    d->ISOSpeedCB->insertItem("80",    9);
    d->ISOSpeedCB->insertItem("100",   10);
    d->ISOSpeedCB->insertItem("125",   11);
    d->ISOSpeedCB->insertItem("160",   12);
    d->ISOSpeedCB->insertItem("200",   13);
    d->ISOSpeedCB->insertItem("250",   14);
    d->ISOSpeedCB->insertItem("320",   15);
    d->ISOSpeedCB->insertItem("400",   16);
    d->ISOSpeedCB->insertItem("500",   17);
    d->ISOSpeedCB->insertItem("640",   18);
    d->ISOSpeedCB->insertItem("800",   19);
    d->ISOSpeedCB->insertItem("1000",  20);
    d->ISOSpeedCB->insertItem("1250",  21);
    d->ISOSpeedCB->insertItem("1600",  22);
    d->ISOSpeedCB->insertItem("2000",  23);
    d->ISOSpeedCB->insertItem("2500",  24);
    d->ISOSpeedCB->insertItem("3200",  25);
    d->ISOSpeedCB->insertItem("4000",  26);
    d->ISOSpeedCB->insertItem("5000",  27);
    d->ISOSpeedCB->insertItem("6400",  28);
    d->ISOSpeedCB->insertItem("8000",  29);
    d->ISOSpeedCB->insertItem("10000", 30);
    d->ISOSpeedCB->insertItem("12500", 31);
    d->ISOSpeedCB->insertItem("16000", 32);
    d->ISOSpeedCB->insertItem("20000", 33);
    d->ISOSpeedCB->insertItem("25000", 34);
    d->ISOSpeedCB->insertItem("32000", 35);
    grid->addMultiCellWidget(d->ISOSpeedCheck, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->ISOSpeedCB, 6, 6, 2, 5);
    QWhatsThis::add(d->ISOSpeedCB, i18n("<p>Select here the ISO Speed of the digital still camera "
                    "witch have taken the picture."));

    // --------------------------------------------------------

    d->lightSourceCheck = new QCheckBox(i18n("Light source:"), parent);
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
    grid->addMultiCellWidget(d->lightSourceCheck, 7, 7, 0, 0);
    grid->addMultiCellWidget(d->lightSourceCB, 7, 7, 2, 5);
    QWhatsThis::add(d->lightSourceCB, i18n("<p>Select here the kind of light source used "
                                           "to take the picture."));

    // --------------------------------------------------------

    d->flashModeCheck = new QCheckBox(i18n("Flash mode:"), parent);
    d->flashModeCB    = new QComboBox(false, parent);

    for (EXIFPhotoPriv::FlashModeMap::Iterator it = d->flashModeMap.begin();
        it != d->flashModeMap.end(); ++it )
       d->flashModeCB->insertItem(it.data().desc());

    grid->addMultiCellWidget(d->flashModeCheck, 8, 8, 0, 0);
    grid->addMultiCellWidget(d->flashModeCB, 8, 8, 2, 5);
    QWhatsThis::add(d->flashModeCB, i18n("<p>Select here the flash program mode used by camera "
                                         "to take the picture."));

    grid->setColStretch(1, 10);                     
    grid->setColStretch(5, 10);                     
    grid->setRowStretch(9, 10);                     

    // --------------------------------------------------------

    connect(d->focalLengthCheck, SIGNAL(toggled(bool)),
            d->focalLengthEdit, SLOT(setEnabled(bool)));

    connect(d->apertureCheck, SIGNAL(toggled(bool)),
            d->apertureCB, SLOT(setEnabled(bool)));

    connect(d->exposureTimeCheck, SIGNAL(toggled(bool)),
            d->exposureTimeNumEdit, SLOT(setEnabled(bool)));

    connect(d->exposureTimeCheck, SIGNAL(toggled(bool)),
            d->exposureTimeDenEdit, SLOT(setEnabled(bool)));

    connect(d->exposureProgramCheck, SIGNAL(toggled(bool)),
            d->exposureProgramCB, SLOT(setEnabled(bool)));

    connect(d->exposureModeCheck, SIGNAL(toggled(bool)),
            d->exposureModeCB, SLOT(setEnabled(bool)));

    connect(d->meteringModeCheck, SIGNAL(toggled(bool)),
            d->meteringModeCB, SLOT(setEnabled(bool)));

    connect(d->ISOSpeedCheck, SIGNAL(toggled(bool)),
            d->ISOSpeedCB, SLOT(setEnabled(bool)));

    connect(d->lightSourceCheck, SIGNAL(toggled(bool)),
            d->lightSourceCB, SLOT(setEnabled(bool)));

    connect(d->flashModeCheck, SIGNAL(toggled(bool)),
            d->flashModeCB, SLOT(setEnabled(bool)));

    // --------------------------------------------------------
    
    readMetadata(exifData);
}

EXIFPhoto::~EXIFPhoto()
{
    delete d;
}

void EXIFPhoto::readMetadata(QByteArray& exifData)
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
    d->exposureTimeNumEdit->setEnabled(d->focalLengthCheck->isChecked());

    if (exiv2Iface.getExifTagRational("Exif.Photo.FNumber", num, den))
    {
        QString aperture = QString::number((double)(num)/(double)(den), 'f', 1);

        int item = -1;
        for (int i = 0 ; i < d->apertureCB->count() ; i++)
            if (d->apertureCB->text(i) == aperture)
                item = i;

        if (item != -1)
        {
            d->apertureCB->setCurrentItem(item);
            d->apertureCheck->setChecked(true);
        }
    }
    d->apertureCB->setEnabled(d->apertureCheck->isChecked());
    
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

    if (exiv2Iface.getExifTagLong("Exif.Photo.ExposureMode", val))
    {
        d->exposureModeCB->setCurrentItem(val);
        d->exposureModeCheck->setChecked(true);
    }
    d->exposureModeCB->setEnabled(d->exposureModeCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.MeteringMode", val))
    {
        d->meteringModeCB->setCurrentItem(val > 6 ? 7 : val);
        d->meteringModeCheck->setChecked(true);
    }
    d->meteringModeCB->setEnabled(d->meteringModeCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.ISOSpeedRatings", val))
    {
        int item = -1;
        for (int i = 0 ; i < d->ISOSpeedCB->count() ; i++)
            if (d->ISOSpeedCB->text(i) == QString::number(val))
                item = i;

        if (item != -1)
        {
            d->ISOSpeedCB->setCurrentItem(item);
            d->ISOSpeedCheck->setChecked(true);
        }
    }
    d->ISOSpeedCB->setEnabled(d->ISOSpeedCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.LightSource", val))
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
    d->lightSourceCB->setEnabled(d->lightSourceCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.Flash", val))
    {
        int item = -1;    
        for (EXIFPhotoPriv::FlashModeMap::Iterator it = d->flashModeMap.begin();
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
    }
    d->flashModeCB->setEnabled(d->flashModeCheck->isChecked());
}

void EXIFPhoto::applyMetadata(QByteArray& exifData)
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

    if (d->exposureTimeCheck->isChecked())
    {
        exiv2Iface.convertToRational(d->apertureCB->currentText().toDouble(), &num, &den, 1);
        exiv2Iface.setExifTagRational("Exif.Photo.FNumber", num, den);
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.FNumber");

    if (d->exposureTimeCheck->isChecked())
        exiv2Iface.setExifTagRational("Exif.Photo.ExposureTime", d->exposureTimeNumEdit->value(),
                                      d->exposureTimeDenEdit->value());
    else
        exiv2Iface.removeExifTag("Exif.Photo.ExposureTime");

    if (d->exposureProgramCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.ExposureProgram", d->exposureProgramCB->currentItem());
    else
        exiv2Iface.removeExifTag("Exif.Photo.ExposureProgram");

    if (d->exposureModeCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.ExposureMode", d->exposureModeCB->currentItem());
    else
        exiv2Iface.removeExifTag("Exif.Photo.ExposureMode");

    if (d->meteringModeCheck->isChecked())
    {
        long met = d->meteringModeCB->currentItem();
        exiv2Iface.setExifTagLong("Exif.Photo.MeteringMode", met > 6 ? 255 : met);
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.MeteringMode");

    if (d->ISOSpeedCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.ISOSpeedRatings", d->ISOSpeedCB->currentText().toLong());
    else
        exiv2Iface.removeExifTag("Exif.Photo.ISOSpeedRatings");

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
    else
        exiv2Iface.removeExifTag("Exif.Photo.LightSource");

    if (d->flashModeCheck->isChecked())
    {
        long val = d->flashModeCB->currentItem();
        exiv2Iface.setExifTagLong("Exif.Photo.Flash", d->flashModeMap[val].id());
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.Flash");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

