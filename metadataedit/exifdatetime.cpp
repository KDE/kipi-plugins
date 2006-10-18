/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF date and time settings page.
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
#include <qdatetime.h>
#include <qwhatsthis.h>
#include <qvalidator.h>
#include <qcheckbox.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <kdatetimewidget.h>

// Local includes.

#include "exiv2iface.h"
#include "exifdatetime.h"
#include "exifdatetime.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFDateTimePriv
{
public:

    EXIFDateTimePriv()
    {
        dateCreatedSel       = 0;
        dateOriginalSel      = 0;
        dateDigitalizedSel   = 0;

        dateCreatedCheck     = 0;
        dateOriginalCheck    = 0;
        dateDigitalizedCheck = 0;
    }

    QCheckBox       *dateCreatedCheck;
    QCheckBox       *dateOriginalCheck;
    QCheckBox       *dateDigitalizedCheck;

    KDateTimeWidget *dateCreatedSel;
    KDateTimeWidget *dateOriginalSel;
    KDateTimeWidget *dateDigitalizedSel;
};

EXIFDateTime::EXIFDateTime(QWidget* parent, QByteArray& exifData)
            : QWidget(parent)
{
    d = new EXIFDateTimePriv;

    QGridLayout* grid = new QGridLayout(parent, 8, 3, KDialog::spacingHint());

    // --------------------------------------------------------

    d->dateCreatedCheck = new QCheckBox(i18n("Creation date and time"), parent);
    d->dateCreatedSel   = new KDateTimeWidget(parent);
    d->dateCreatedSel->setDateTime(QDateTime::currentDateTime());
    grid->addMultiCellWidget(d->dateCreatedCheck, 0, 0, 0, 1);
    grid->addMultiCellWidget(d->dateCreatedSel, 1, 1, 0, 1);
    QWhatsThis::add(d->dateCreatedSel, i18n("<p>Set here the date and time of image creation. "
                                       "In this standard it is the date and time the file was changed."));

    // --------------------------------------------------------

    d->dateOriginalCheck = new QCheckBox(i18n("Original date and time"), parent);
    d->dateOriginalSel   = new KDateTimeWidget(parent);
    d->dateOriginalSel->setDateTime(QDateTime::currentDateTime());
    grid->addMultiCellWidget(d->dateOriginalCheck, 2, 2, 0, 1);
    grid->addMultiCellWidget(d->dateOriginalSel, 3, 3, 0, 1);
    QWhatsThis::add(d->dateOriginalSel, i18n("<p>Set here the date and time when the original image "
                                        "data was generated. For a digital still camera the date and "
                                        "time the picture was taken are recorded."));

    // --------------------------------------------------------

    d->dateDigitalizedCheck = new QCheckBox(i18n("Digitization date and time"), parent);
    d->dateDigitalizedSel   = new KDateTimeWidget(parent);
    d->dateDigitalizedSel->setDateTime(QDateTime::currentDateTime());
    grid->addMultiCellWidget(d->dateDigitalizedCheck, 4, 4, 0, 1);
    grid->addMultiCellWidget(d->dateDigitalizedSel, 5, 5, 0, 1);
    QWhatsThis::add(d->dateDigitalizedSel, i18n("<p>Set here the date and time when the image was "
                                           "stored as digital data. If, for example, an image was "
                                           "captured by a digital still camera and at the same "
                                           "time the file was recorded, then Original and Digitization "
                                           "date and time will have the same contents."));

    grid->setColStretch(2, 10);                     
    grid->setRowStretch(6, 10);                     

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->dateCreatedSel, SLOT(setEnabled(bool)));

    connect(d->dateOriginalCheck, SIGNAL(toggled(bool)),
            d->dateOriginalSel, SLOT(setEnabled(bool)));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            d->dateDigitalizedSel, SLOT(setEnabled(bool)));

    // --------------------------------------------------------
    
    readMetadata(exifData);
}

EXIFDateTime::~EXIFDateTime()
{
    delete d;
}

void EXIFDateTime::readMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);

    QDateTime datetime;
    QString datetimeStr;

    datetimeStr = exiv2Iface.getExifTagString("Exif.Image.DateTime", false);

    if (!datetimeStr.isEmpty()) 
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateCreatedCheck->setChecked(true);
            d->dateCreatedSel->setDateTime(datetime);
        }
    }    
    d->dateCreatedSel->setEnabled(d->dateCreatedCheck->isChecked());

    datetimeStr = exiv2Iface.getExifTagString("Exif.Photo.DateTimeOriginal", false);

    if (!datetimeStr.isEmpty()) 
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateOriginalCheck->setChecked(true);
            d->dateOriginalSel->setDateTime(datetime);
        }
    }    
    d->dateOriginalSel->setEnabled(d->dateOriginalCheck->isChecked());

    datetimeStr = exiv2Iface.getExifTagString("Exif.Photo.DateTimeDigitized", false);

    if (!datetimeStr.isEmpty()) 
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateDigitalizedCheck->setChecked(true);
            d->dateDigitalizedSel->setDateTime(datetime);
        }
    }    
    d->dateDigitalizedSel->setEnabled(d->dateDigitalizedCheck->isChecked());
}

void EXIFDateTime::applyMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);

    if (d->dateCreatedCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Image.DateTime",
                   d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
    else
        exiv2Iface.removeExifTag("Exif.Image.DateTime");

    if (d->dateOriginalCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.DateTimeOriginal",
                   d->dateOriginalSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.DateTimeOriginal");

    if (d->dateDigitalizedCheck->isChecked())
        exiv2Iface.setIptcTagString("Exif.Photo.DateTimeDigitized",
                   d->dateDigitalizedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.DateTimeDigitized");

    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

