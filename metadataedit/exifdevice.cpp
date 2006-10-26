/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF device settings page.
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
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>
#include <klineedit.h>

// Local includes.

#include "metadatacheckbox.h"
#include "exiv2iface.h"
#include "exifdevice.h"
#include "exifdevice.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFDevicePriv
{
public:

    EXIFDevicePriv()
    {
        makeCheck                = 0;
        modelCheck               = 0;
        deviceTypeCheck          = 0;
        exposureTimeCheck        = 0;
        exposureProgramCheck     = 0;
        exposureModeCheck        = 0;
        exposureBiasCheck        = 0;
        ISOSpeedCheck            = 0;
        meteringModeCheck        = 0;
        sensingMethodCheck       = 0;
        sceneTypeCheck           = 0;
        subjectDistanceTypeCheck = 0;
        exposureProgramCB        = 0;
        exposureModeCB           = 0;
        ISOSpeedCB               = 0;
        meteringModeCB           = 0;
        sensingMethodCB          = 0;
        sceneTypeCB              = 0;
        subjectDistanceTypeCB    = 0;
        exposureTimeNumEdit      = 0;
        exposureTimeDenEdit      = 0;
        exposureBiasEdit         = 0;
        makeEdit                 = 0;
        modelEdit                = 0;
    }

    QCheckBox        *makeCheck;
    QCheckBox        *modelCheck;
    QCheckBox        *exposureTimeCheck;
    QCheckBox        *exposureBiasCheck;
   
    QComboBox        *deviceTypeCB;
    QComboBox        *exposureProgramCB;
    QComboBox        *exposureModeCB;
    QComboBox        *ISOSpeedCB;
    QComboBox        *meteringModeCB;
    QComboBox        *sensingMethodCB;
    QComboBox        *sceneTypeCB;
    QComboBox        *subjectDistanceTypeCB;

    KLineEdit        *makeEdit;
    KLineEdit        *modelEdit;

    KIntSpinBox      *exposureTimeNumEdit;
    KIntSpinBox      *exposureTimeDenEdit;

    KDoubleSpinBox   *exposureBiasEdit;

    MetadataCheckBox *deviceTypeCheck;
    MetadataCheckBox *exposureProgramCheck;
    MetadataCheckBox *exposureModeCheck;
    MetadataCheckBox *meteringModeCheck;
    MetadataCheckBox *ISOSpeedCheck;
    MetadataCheckBox *sensingMethodCheck;
    MetadataCheckBox *sceneTypeCheck;
    MetadataCheckBox *subjectDistanceTypeCheck;
};

EXIFDevice::EXIFDevice(QWidget* parent, QByteArray& exifData)
          : QWidget(parent)
{
    d = new EXIFDevicePriv;

    QGridLayout* grid = new QGridLayout(parent, 13, 5, KDialog::spacingHint());

    // EXIF only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->makeCheck = new QCheckBox(i18n("Device manufacturer (*):"), parent);
    d->makeEdit  = new KLineEdit(parent);
    d->makeEdit->setValidator(asciiValidator);
    grid->addMultiCellWidget(d->makeCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->makeEdit, 0, 0, 2, 5);
    QWhatsThis::add(d->makeEdit, i18n("<p>Set here the manufacturer of image input equipment used to "
                                 "take the picture. This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->modelCheck = new QCheckBox(i18n("Device model (*):"), parent);
    d->modelEdit  = new KLineEdit(parent);
    d->modelEdit->setValidator(asciiValidator);
    grid->addMultiCellWidget(d->modelCheck, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->modelEdit, 1, 1, 2, 5);
    QWhatsThis::add(d->modelEdit, i18n("<p>Set here the model of image input equipment used to "
                                  "take the picture. This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->deviceTypeCheck = new MetadataCheckBox(i18n("Device type:"), parent);
    d->deviceTypeCB    = new QComboBox(false, parent);
    d->deviceTypeCB->insertItem(i18n("Film scanner"),             0);
    d->deviceTypeCB->insertItem(i18n("Reflection print scanner"), 1);
    d->deviceTypeCB->insertItem(i18n("Digital still camera"),     2);
    grid->addMultiCellWidget(d->deviceTypeCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->deviceTypeCB, 2, 2, 2, 5);
    QWhatsThis::add(d->deviceTypeCB, i18n("<p>Select here the image input equipment type used to "
                                     "take the picture."));

    // --------------------------------------------------------

    d->exposureTimeCheck   = new QCheckBox(i18n("Exposure time (seconds):"), parent);
    d->exposureTimeNumEdit = new KIntSpinBox(1, 100000, 1, 1, 10, parent);
    d->exposureTimeDenEdit = new KIntSpinBox(1, 100000, 1, 1, 10, parent);
    QLabel *exposureLabel  = new QLabel("/", parent);
    exposureLabel->setAlignment (Qt::AlignRight|Qt::AlignVCenter);
    grid->addMultiCellWidget(d->exposureTimeCheck, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->exposureTimeNumEdit, 3, 3, 2, 2);
    grid->addMultiCellWidget(exposureLabel, 3, 3, 3, 3);
    grid->addMultiCellWidget(d->exposureTimeDenEdit, 3, 3, 4, 4);
    QWhatsThis::add(d->exposureTimeCheck, i18n("<p>Set on this option to set the exposure time "
                                          "of picture, given in seconds."));

    // --------------------------------------------------------

    d->exposureProgramCheck = new MetadataCheckBox(i18n("Exposure program:"), parent);
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
    grid->addMultiCellWidget(d->exposureProgramCheck, 4, 4, 0, 0);
    grid->addMultiCellWidget(d->exposureProgramCB, 4, 4, 2, 5);
    QWhatsThis::add(d->exposureProgramCB, i18n("<p>Select here the program used by the camera "
                                          "to set exposure when the picture have been taken."));

    // --------------------------------------------------------

    d->exposureModeCheck = new MetadataCheckBox(i18n("Exposure mode:"), parent);
    d->exposureModeCB    = new QComboBox(false, parent);
    d->exposureModeCB->insertItem(i18n("Auto"),         0);
    d->exposureModeCB->insertItem(i18n("Manual"),       1);
    d->exposureModeCB->insertItem(i18n("Auto bracket"), 2);
    grid->addMultiCellWidget(d->exposureModeCheck, 5, 5, 0, 0);
    grid->addMultiCellWidget(d->exposureModeCB, 5, 5, 2, 5);
    QWhatsThis::add(d->exposureModeCB, i18n("<p>Select here the mode used by the camera "
                                       "to set exposure when the picture have been shot. "
                                       "In auto-bracketing mode, the camera shoots a "
                                       "series of frames of the same scene at different "
                                       "exposure settings."));

    // --------------------------------------------------------

    d->exposureBiasCheck = new QCheckBox(i18n("Exposure bias (APEX):"), parent);
    d->exposureBiasEdit  = new KDoubleSpinBox(-99.99, 99.99, 0.1, 0.0, 2, parent);
    grid->addMultiCellWidget(d->exposureBiasCheck, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->exposureBiasEdit, 6, 6, 2, 2);
    QWhatsThis::add(d->exposureBiasEdit, i18n("<p>Set here the exposure bias value in APEX unit "
                                         "used by camera to take the picture."));

    // --------------------------------------------------------

    d->meteringModeCheck = new MetadataCheckBox(i18n("Metering mode:"), parent);
    d->meteringModeCB    = new QComboBox(false, parent);
    d->meteringModeCB->insertItem(i18n("Unknown"),                 0);
    d->meteringModeCB->insertItem(i18n("Average"),                 1);
    d->meteringModeCB->insertItem(i18n("Center weighted average"), 2);
    d->meteringModeCB->insertItem(i18n("Spot"),                    3);
    d->meteringModeCB->insertItem(i18n("Multi-spot"),              4);
    d->meteringModeCB->insertItem(i18n("Multi-segment"),           5);
    d->meteringModeCB->insertItem(i18n("Partial"),                 6);
    d->meteringModeCB->insertItem(i18n("Other"),                   7);
    grid->addMultiCellWidget(d->meteringModeCheck, 7, 7, 0, 0);
    grid->addMultiCellWidget(d->meteringModeCB, 7, 7, 2, 5);
    QWhatsThis::add(d->meteringModeCB, i18n("<p>Select here the metering mode used by the camera "
                                       "to set exposure when the picture have been shot."));

    // --------------------------------------------------------

    d->ISOSpeedCheck = new MetadataCheckBox(i18n("Sensitivity (ISO):"), parent);
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
    grid->addMultiCellWidget(d->ISOSpeedCheck, 8, 8, 0, 0);
    grid->addMultiCellWidget(d->ISOSpeedCB, 8, 8, 2, 5);
    QWhatsThis::add(d->ISOSpeedCB, i18n("<p>Select here the ISO Speed of the camera "
                                   "witch have taken the picture."));

    // --------------------------------------------------------

    d->sensingMethodCheck = new MetadataCheckBox(i18n("Sensing method:"), parent);
    d->sensingMethodCB    = new QComboBox(false, parent);
    d->sensingMethodCB->insertItem(i18n("Not defined"),             0);
    d->sensingMethodCB->insertItem(i18n("One-chip color area"),     1);
    d->sensingMethodCB->insertItem(i18n("Two-chip color area"),     2);
    d->sensingMethodCB->insertItem(i18n("Three-chip color area"),   3);
    d->sensingMethodCB->insertItem(i18n("Color sequential area"),   4);
    d->sensingMethodCB->insertItem(i18n("Trilinear sensor"),        5);
    d->sensingMethodCB->insertItem(i18n("Color sequential linear"), 6);
    grid->addMultiCellWidget(d->sensingMethodCheck, 9, 9, 0, 0);
    grid->addMultiCellWidget(d->sensingMethodCB, 9, 9, 2, 5);
    QWhatsThis::add(d->sensingMethodCB, i18n("<p>Select here the image sensor type used by the camera "
                                       "to take the picture."));

    // --------------------------------------------------------

    d->sceneTypeCheck = new MetadataCheckBox(i18n("Scene capture type:"), parent);
    d->sceneTypeCB    = new QComboBox(false, parent);
    d->sceneTypeCB->insertItem(i18n("Standard"),    0);
    d->sceneTypeCB->insertItem(i18n("Landscape"),   1);
    d->sceneTypeCB->insertItem(i18n("Portrait"),    2);
    d->sceneTypeCB->insertItem(i18n("Night scene"), 3);
    grid->addMultiCellWidget(d->sceneTypeCheck, 10, 10, 0, 0);
    grid->addMultiCellWidget(d->sceneTypeCB, 10, 10, 2, 5);
    QWhatsThis::add(d->sceneTypeCB, i18n("<p>Select here the type of scene used by the camera "
                                    "to take the picture."));

    // --------------------------------------------------------

    d->subjectDistanceTypeCheck = new MetadataCheckBox(i18n("Subject distance type:"), parent);
    d->subjectDistanceTypeCB    = new QComboBox(false, parent);
    d->subjectDistanceTypeCB->insertItem(i18n("Unknow"),       0);
    d->subjectDistanceTypeCB->insertItem(i18n("Macro"),        1);
    d->subjectDistanceTypeCB->insertItem(i18n("Close view"),   2);
    d->subjectDistanceTypeCB->insertItem(i18n("Distant view"), 3);
    grid->addMultiCellWidget(d->subjectDistanceTypeCheck, 11, 11, 0, 0);
    grid->addMultiCellWidget(d->subjectDistanceTypeCB, 11, 11, 2, 5);
    QWhatsThis::add(d->subjectDistanceTypeCB, i18n("<p>Select here the type of distance between "
                                              "the subject and the image input equipment."));

    // --------------------------------------------------------

    QLabel *exifNote = new QLabel(i18n("<b>Note: EXIF text tags annoted by (*) only support printable "
                                       "ASCII characters set.</b>"), parent);
    grid->addMultiCellWidget(exifNote, 12, 12, 0, 5);

    grid->setColStretch(1, 10);                     
    grid->setColStretch(5, 10);                     
    grid->setRowStretch(13, 10);                     

    // --------------------------------------------------------

    connect(d->makeCheck, SIGNAL(toggled(bool)),
            d->makeEdit, SLOT(setEnabled(bool)));

    connect(d->modelCheck, SIGNAL(toggled(bool)),
            d->modelEdit, SLOT(setEnabled(bool)));

    connect(d->deviceTypeCheck, SIGNAL(toggled(bool)),
            d->deviceTypeCB, SLOT(setEnabled(bool)));

    connect(d->exposureTimeCheck, SIGNAL(toggled(bool)),
            d->exposureTimeNumEdit, SLOT(setEnabled(bool)));

    connect(d->exposureTimeCheck, SIGNAL(toggled(bool)),
            d->exposureTimeDenEdit, SLOT(setEnabled(bool)));

    connect(d->exposureProgramCheck, SIGNAL(toggled(bool)),
            d->exposureProgramCB, SLOT(setEnabled(bool)));

    connect(d->exposureModeCheck, SIGNAL(toggled(bool)),
            d->exposureModeCB, SLOT(setEnabled(bool)));

    connect(d->exposureBiasCheck, SIGNAL(toggled(bool)),
            d->exposureBiasEdit, SLOT(setEnabled(bool)));

    connect(d->meteringModeCheck, SIGNAL(toggled(bool)),
            d->meteringModeCB, SLOT(setEnabled(bool)));

    connect(d->ISOSpeedCheck, SIGNAL(toggled(bool)),
            d->ISOSpeedCB, SLOT(setEnabled(bool)));

    connect(d->sensingMethodCheck, SIGNAL(toggled(bool)),
            d->sensingMethodCB, SLOT(setEnabled(bool)));

    connect(d->sceneTypeCheck, SIGNAL(toggled(bool)),
            d->sceneTypeCB, SLOT(setEnabled(bool)));

    connect(d->subjectDistanceTypeCheck, SIGNAL(toggled(bool)),
            d->subjectDistanceTypeCB, SLOT(setEnabled(bool)));

    // --------------------------------------------------------
    
    readMetadata(exifData);
}

EXIFDevice::~EXIFDevice()
{
    delete d;
}

void EXIFDevice::readMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;
    long     val=0;
    QString  data;

    data = exiv2Iface.getExifTagString("Exif.Image.Make", false);       
    if (!data.isNull())
    {
        d->makeEdit->setText(data);
        d->makeCheck->setChecked(true);
    }
    d->makeEdit->setEnabled(d->makeCheck->isChecked());

    data = exiv2Iface.getExifTagString("Exif.Image.Model", false);     
    if (!data.isNull())
    {
        d->modelEdit->setText(data);
        d->modelCheck->setChecked(true);
    }
    d->modelEdit->setEnabled(d->modelCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.FileSource", val))
    {
        if (val>0 && val<4)
        {
            d->deviceTypeCB->setCurrentItem(val-1);
            d->deviceTypeCheck->setChecked(true);
        }
        else
            d->deviceTypeCheck->setValid(false);
    }
    d->deviceTypeCB->setEnabled(d->deviceTypeCheck->isChecked());

    if (exiv2Iface.getExifTagRational("Exif.Photo.ExposureTime", num, den))
    {
        d->exposureTimeNumEdit->setValue(num);
        d->exposureTimeDenEdit->setValue(den);
        d->exposureTimeCheck->setChecked(true);
    }
    else if (exiv2Iface.getExifTagRational("Exif.Photo.ShutterSpeedValue", num, den))
    {
        double tmp = exp(log(2.0) * (double)(num)/(double)(den));
        if (tmp > 1.0) 
            num = (long int)(tmp + 0.5);
        else 
            den = (long int)(1.0/tmp + 0.5);

        d->exposureTimeNumEdit->setValue(num);
        d->exposureTimeDenEdit->setValue(den);
        d->exposureTimeCheck->setChecked(true);        
    }
    d->exposureTimeNumEdit->setEnabled(d->exposureTimeCheck->isChecked());
    d->exposureTimeDenEdit->setEnabled(d->exposureTimeCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.ExposureProgram", val))
    {
        if (val>=0 && val <=8)
        {
            d->exposureProgramCB->setCurrentItem(val);
            d->exposureProgramCheck->setChecked(true);
        }
        else 
            d->exposureProgramCheck->setValid(false);
    }
    d->exposureProgramCB->setEnabled(d->exposureProgramCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.ExposureMode", val))
    {
        if (val>=0 && val <=2)
        {
            d->exposureModeCB->setCurrentItem(val);
            d->exposureModeCheck->setChecked(true);
        }
        else
            d->exposureModeCheck->setValid(false);
    }
    d->exposureModeCB->setEnabled(d->exposureModeCheck->isChecked());

    if (exiv2Iface.getExifTagRational("Exif.Photo.ExposureBiasValue", num, den))
    {
        d->exposureBiasEdit->setValue((double)(num) / (double)(den));
        d->exposureBiasCheck->setChecked(true);
    }
    d->exposureBiasEdit->setEnabled(d->exposureBiasCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.MeteringMode", val))
    {
        if ((val>= 0 && val <=6) || val == 255) 
        {
            d->meteringModeCB->setCurrentItem(val == 255 ? 7 : val);
            d->meteringModeCheck->setChecked(true);
        }
        else
            d->meteringModeCheck->setValid(false);
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
        else
            d->ISOSpeedCheck->setValid(false);
    }
    else if (exiv2Iface.getExifTagRational("Exif.Photo.ExposureIndex", num, den))
    {
        val = num / den;    
        int item = -1;
        for (int i = 0 ; i < d->ISOSpeedCB->count() ; i++)
            if (d->ISOSpeedCB->text(i) == QString::number(val))
                item = i;

        if (item != -1)
        {
            d->ISOSpeedCB->setCurrentItem(item);
            d->ISOSpeedCheck->setChecked(true);
        }
        else
            d->ISOSpeedCheck->setValid(false);
    }
    d->ISOSpeedCB->setEnabled(d->ISOSpeedCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.SensingMethod", val))
    {
        if (val>=1 && val<=8 && val!=6)
        {
            d->sensingMethodCB->setCurrentItem(val > 6 ? val-2 : val-1);
            d->sensingMethodCheck->setChecked(true);
        }
        else
            d->sensingMethodCheck->setValid(false);            
    }
    d->sensingMethodCB->setEnabled(d->sensingMethodCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.SceneCaptureType", val))
    {
        if (val>=0 && val<=3)
        {
            d->sceneTypeCB->setCurrentItem(val);
            d->sceneTypeCheck->setChecked(true);
        }
        else
            d->sceneTypeCheck->setValid(false);            
    }
    d->sceneTypeCB->setEnabled(d->sceneTypeCheck->isChecked());

    if (exiv2Iface.getExifTagLong("Exif.Photo.SubjectDistanceRange", val))
    {
        if (val>=0 && val<=3)
        {
            d->subjectDistanceTypeCB->setCurrentItem(val);
            d->subjectDistanceTypeCheck->setChecked(true);
        }
        else
            d->subjectDistanceTypeCheck->setValid(false);            
    }
    d->subjectDistanceTypeCB->setEnabled(d->subjectDistanceTypeCheck->isChecked());
}

void EXIFDevice::applyMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);
    long int num=1, den=1;

    if (d->makeCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Image.Make", d->makeEdit->text());
    else
        exiv2Iface.removeExifTag("Exif.Image.Make");

    if (d->modelCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Image.Model", d->modelEdit->text());
    else
        exiv2Iface.removeExifTag("Exif.Image.Model");

    if (d->deviceTypeCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.FileSource", d->deviceTypeCB->currentItem()+1);
    else if (d->deviceTypeCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.FileSource");

    if (d->exposureTimeCheck->isChecked())
    {
        exiv2Iface.setExifTagRational("Exif.Photo.ExposureTime", d->exposureTimeNumEdit->value(),
                                      d->exposureTimeDenEdit->value());

        double exposureTime = (double)(d->exposureTimeNumEdit->value())/
                              (double)(d->exposureTimeDenEdit->value());
        double shutterSpeed = (-1.0)*(log(exposureTime)/log(2.0));
        exiv2Iface.convertToRational(shutterSpeed, &num, &den, 8);
        exiv2Iface.setExifTagRational("Exif.Photo.ShutterSpeedValue", num, den);
    }
    else
    {
        exiv2Iface.removeExifTag("Exif.Photo.ExposureTime");
        exiv2Iface.removeExifTag("Exif.Photo.ShutterSpeedValue");
    }

    if (d->exposureProgramCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.ExposureProgram", d->exposureProgramCB->currentItem());
    else if (d->exposureProgramCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.ExposureProgram");

    if (d->exposureModeCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.ExposureMode", d->exposureModeCB->currentItem());
    else if (d->exposureModeCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.ExposureMode");

    if (d->exposureBiasCheck->isChecked())
    {
        exiv2Iface.convertToRational(d->exposureBiasEdit->value(), &num, &den, 1);
        exiv2Iface.setExifTagRational("Exif.Photo.ExposureBiasValue", num, den);
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.ExposureBiasValue");

    if (d->meteringModeCheck->isChecked())
    {
        long met = d->meteringModeCB->currentItem();
        exiv2Iface.setExifTagLong("Exif.Photo.MeteringMode", met > 6 ? 255 : met);
    }
    else if (d->meteringModeCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.MeteringMode");

    if (d->ISOSpeedCheck->isChecked())
    {
        exiv2Iface.setExifTagLong("Exif.Photo.ISOSpeedRatings", d->ISOSpeedCB->currentText().toLong());

        exiv2Iface.convertToRational(d->ISOSpeedCB->currentText().toDouble(), &num, &den, 1);
        exiv2Iface.setExifTagRational("Exif.Photo.ExposureIndex", num, den);
    }
    else if (d->ISOSpeedCheck->isValid())
    {
        exiv2Iface.removeExifTag("Exif.Photo.ISOSpeedRatings");
        exiv2Iface.removeExifTag("Exif.Photo.ExposureIndex");
    }

    if (d->sensingMethodCheck->isChecked())
    {
        long sem = d->sensingMethodCB->currentItem();
        exiv2Iface.setExifTagLong("Exif.Photo.SensingMethod", sem > 4 ? sem+2 : sem+1);
    }
    else if (d->sensingMethodCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.SensingMethod");

    if (d->sceneTypeCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.SceneCaptureType", d->sceneTypeCB->currentItem());
    else if (d->sceneTypeCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.SceneCaptureType");

    if (d->subjectDistanceTypeCheck->isChecked())
        exiv2Iface.setExifTagLong("Exif.Photo.SubjectDistanceRange", d->subjectDistanceTypeCB->currentItem());
    else if (d->subjectDistanceTypeCheck->isValid())
        exiv2Iface.removeExifTag("Exif.Photo.SubjectDistanceRange");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

