/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : EXIF date and time settings page.
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

#include "exifdatetime.h"
#include "exifdatetime.moc"

// Qt includes

#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>

// KDE includes

#include <kaboutdata.h>
#include <kcomponentdata.h>
#include <kdatetimewidget.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <kseparator.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

namespace KIPIMetadataEditPlugin
{

class EXIFDateTimePriv
{
public:

    EXIFDateTimePriv()
    {
        dateCreatedSel             = 0;
        dateOriginalSel            = 0;
        dateDigitalizedSel         = 0;
        dateCreatedSubSecEdit      = 0;
        dateOriginalSubSecEdit     = 0;
        dateDigitalizedSubSecEdit  = 0;

        dateCreatedCheck           = 0;
        dateOriginalCheck          = 0;
        dateDigitalizedCheck       = 0;
        dateCreatedSubSecCheck     = 0;
        dateOriginalSubSecCheck    = 0;
        dateDigitalizedSubSecCheck = 0;
        syncHOSTDateCheck          = 0;
        syncXMPDateCheck           = 0;
        syncIPTCDateCheck          = 0;

        setTodayCreatedBtn         = 0;
        setTodayOriginalBtn        = 0;
        setTodayDigitalizedBtn     = 0;
    }

    QCheckBox       *dateCreatedCheck;
    QCheckBox       *dateOriginalCheck;
    QCheckBox       *dateDigitalizedCheck;
    QCheckBox       *dateCreatedSubSecCheck;
    QCheckBox       *dateOriginalSubSecCheck;
    QCheckBox       *dateDigitalizedSubSecCheck;
    QCheckBox       *syncHOSTDateCheck;
    QCheckBox       *syncXMPDateCheck;
    QCheckBox       *syncIPTCDateCheck;

    QPushButton     *setTodayCreatedBtn;
    QPushButton     *setTodayOriginalBtn;
    QPushButton     *setTodayDigitalizedBtn;

    KIntSpinBox     *dateCreatedSubSecEdit;
    KIntSpinBox     *dateOriginalSubSecEdit;
    KIntSpinBox     *dateDigitalizedSubSecEdit;

    KDateTimeWidget *dateCreatedSel;
    KDateTimeWidget *dateOriginalSel;
    KDateTimeWidget *dateDigitalizedSel;
};

EXIFDateTime::EXIFDateTime(QWidget* parent)
            : QWidget(parent), d(new EXIFDateTimePriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->dateCreatedCheck       = new QCheckBox(i18n("Creation date and time"), this);
    d->dateCreatedSubSecCheck = new QCheckBox(i18n("Creation sub-second"), this);
    d->dateCreatedSel         = new KDateTimeWidget(this);
    d->dateCreatedSubSecEdit  = new KIntSpinBox(0, 999, 1, 0, this);
    d->syncHOSTDateCheck      = new QCheckBox(i18n("Sync creation date hosted by %1",
                                              KGlobal::mainComponent().aboutData()->programName()),
                                              this);
    d->syncXMPDateCheck       = new QCheckBox(i18n("Sync XMP creation date"), this);
    d->syncIPTCDateCheck      = new QCheckBox(i18n("Sync IPTC creation date"), this);

    d->setTodayCreatedBtn     = new QPushButton();
    d->setTodayCreatedBtn->setIcon(SmallIcon("go-jump-today"));
    d->setTodayCreatedBtn->setWhatsThis(i18n("Set creation date to today"));

    if (!KExiv2Iface::KExiv2::supportXmp())
        d->syncXMPDateCheck->setEnabled(false);

    d->dateCreatedSel->setWhatsThis(i18n("Set here the date and time of image creation. "
                                       "In this standard it is the date and time the file was changed."));
    d->dateCreatedSubSecEdit->setWhatsThis(i18n("Set here the fractions of seconds for the date "
                                                "and time of image creation."));

    slotSetTodayCreated();

    // --------------------------------------------------------

    d->dateOriginalCheck       = new QCheckBox(i18n("Original date and time"), this);
    d->dateOriginalSubSecCheck = new QCheckBox(i18n("Original sub-second"), this);
    d->dateOriginalSel         = new KDateTimeWidget(this);
    d->dateOriginalSubSecEdit  = new KIntSpinBox(0, 999, 1, 0, this);

    d->setTodayOriginalBtn      = new QPushButton();
    d->setTodayOriginalBtn->setIcon(SmallIcon("go-jump-today"));
    d->setTodayOriginalBtn->setWhatsThis(i18n("Set original date to today"));

    d->dateOriginalSel->setWhatsThis(i18n("Set here the date and time when the original image "
                                          "data was generated. For a digital still camera the date and "
                                          "time the picture was taken are recorded."));
    d->dateOriginalSubSecEdit->setWhatsThis(i18n("Set here the fractions of seconds for the date "
                                                 "and time when the original image data was generated."));

    slotSetTodayOriginal();

    // --------------------------------------------------------

    d->dateDigitalizedCheck       = new QCheckBox(i18n("Digitization date and time"), this);
    d->dateDigitalizedSubSecCheck = new QCheckBox(i18n("Digitization sub-second"), this);
    d->dateDigitalizedSel         = new KDateTimeWidget(this);
    d->dateDigitalizedSubSecEdit  = new KIntSpinBox(0, 999, 1, 0, this);

    d->setTodayDigitalizedBtn     = new QPushButton();
    d->setTodayDigitalizedBtn->setIcon(SmallIcon("go-jump-today"));
    d->setTodayDigitalizedBtn->setWhatsThis(i18n("Set digitization date to today"));

    d->dateDigitalizedSel->setWhatsThis(i18n("Set here the date and time when the image was "
                                             "stored as digital data. If, for example, an image was "
                                             "captured by a digital still camera and at the same "
                                             "time the file was recorded, then Original and Digitization "
                                             "date and time will have the same contents."));
    d->dateDigitalizedSubSecEdit->setWhatsThis(i18n("Set here the fractions of seconds for the date "
                                                    "and time when the image was stored as digital data."));

    slotSetTodayDigitalized();

    // --------------------------------------------------------

    grid->addWidget(d->dateCreatedCheck,                    0, 0, 1, 1);
    grid->addWidget(d->dateCreatedSubSecCheck,              0, 1, 1, 3);
    grid->addWidget(d->dateCreatedSel,                      1, 0, 1, 1);
    grid->addWidget(d->dateCreatedSubSecEdit,               1, 1, 1, 1);
    grid->addWidget(d->setTodayCreatedBtn,                  1, 3, 1, 1);
    grid->addWidget(d->syncHOSTDateCheck,                   2, 0, 1, 4);
    grid->addWidget(d->syncXMPDateCheck,                    3, 0, 1, 4);
    grid->addWidget(d->syncIPTCDateCheck,                   4, 0, 1, 4);
    grid->addWidget(new KSeparator(Qt::Horizontal, this),   5, 0, 1, 4);
    grid->addWidget(d->dateOriginalCheck,                   6, 0, 1, 1);
    grid->addWidget(d->dateOriginalSubSecCheck,             6, 1, 1, 3);
    grid->addWidget(d->dateOriginalSel,                     7, 0, 1, 1);
    grid->addWidget(d->dateOriginalSubSecEdit,              7, 1, 1, 1);
    grid->addWidget(d->setTodayOriginalBtn,                 7, 3, 1, 1);
    grid->addWidget(new KSeparator(Qt::Horizontal, this),   8, 0, 1, 4);
    grid->addWidget(d->dateDigitalizedCheck,                9, 0, 1, 1);
    grid->addWidget(d->dateDigitalizedSubSecCheck,          9, 1, 1, 3);
    grid->addWidget(d->dateDigitalizedSel,                 10, 0, 1, 1);
    grid->addWidget(d->dateDigitalizedSubSecEdit,          10, 1, 1, 1);
    grid->addWidget(d->setTodayDigitalizedBtn,             10, 3, 1, 1);
    grid->setColumnStretch(2, 10);
    grid->setRowStretch(11, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->dateCreatedSel, SLOT(setEnabled(bool)));

    connect(d->dateOriginalCheck, SIGNAL(toggled(bool)),
            d->dateOriginalSel, SLOT(setEnabled(bool)));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            d->dateDigitalizedSel, SLOT(setEnabled(bool)));

    connect(d->dateCreatedSubSecCheck, SIGNAL(toggled(bool)),
            d->dateCreatedSubSecEdit, SLOT(setEnabled(bool)));

    connect(d->dateOriginalSubSecCheck, SIGNAL(toggled(bool)),
            d->dateOriginalSubSecEdit, SLOT(setEnabled(bool)));

    connect(d->dateDigitalizedSubSecCheck, SIGNAL(toggled(bool)),
            d->dateDigitalizedSubSecEdit, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncHOSTDateCheck, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncXMPDateCheck, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncIPTCDateCheck, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateOriginalCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateCreatedSubSecCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateOriginalSubSecCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSubSecCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->dateCreatedSubSecEdit, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalModified()));

    connect(d->dateOriginalSubSecEdit, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSubSecEdit, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalModified()));

    connect(d->dateCreatedSel, SIGNAL(valueChanged (const QDateTime &)),
            this, SIGNAL(signalModified()));

    connect(d->dateOriginalSel, SIGNAL(valueChanged (const QDateTime &)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSel, SIGNAL(valueChanged (const QDateTime &)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->setTodayCreatedBtn, SIGNAL(clicked()),
            this, SLOT(slotSetTodayCreated()));

    connect(d->setTodayOriginalBtn, SIGNAL(clicked()),
            this, SLOT(slotSetTodayOriginal()));

    connect(d->setTodayDigitalizedBtn, SIGNAL(clicked()),
            this, SLOT(slotSetTodayDigitalized()));
}

EXIFDateTime::~EXIFDateTime()
{
    delete d;
}

void EXIFDateTime::slotSetTodayCreated()
{
    d->dateCreatedSel->setDateTime(QDateTime::currentDateTime());
    d->dateCreatedSubSecEdit->setValue(0);
}

void EXIFDateTime::slotSetTodayOriginal()
{
    d->dateOriginalSel->setDateTime(QDateTime::currentDateTime());
    d->dateOriginalSubSecEdit->setValue(0);
}

void EXIFDateTime::slotSetTodayDigitalized()
{
    d->dateDigitalizedSel->setDateTime(QDateTime::currentDateTime());
    d->dateDigitalizedSubSecEdit->setValue(0);
}

bool EXIFDateTime::syncHOSTDateIsChecked()
{
    return d->syncHOSTDateCheck->isChecked();
}

bool EXIFDateTime::syncXMPDateIsChecked()
{
    return d->syncXMPDateCheck->isChecked();
}

bool EXIFDateTime::syncIPTCDateIsChecked()
{
    return d->syncIPTCDateCheck->isChecked();
}

void EXIFDateTime::setCheckedSyncHOSTDate(bool c)
{
    d->syncHOSTDateCheck->setChecked(c);
}

void EXIFDateTime::setCheckedSyncXMPDate(bool c)
{
    d->syncXMPDateCheck->setChecked(c);
}

void EXIFDateTime::setCheckedSyncIPTCDate(bool c)
{
    d->syncIPTCDateCheck->setChecked(c);
}

QDateTime EXIFDateTime::getEXIFCreationDate()
{
    return d->dateCreatedSel->dateTime();
}

void EXIFDateTime::readMetadata(QByteArray& exifData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);

    QDateTime datetime;
    QString datetimeStr, data;

    d->dateCreatedSel->setDateTime(QDateTime::currentDateTime());
    d->dateCreatedCheck->setChecked(false);
    datetimeStr = exiv2Iface.getExifTagString("Exif.Image.DateTime", false);
    if (!datetimeStr.isEmpty())
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateCreatedSel->setDateTime(datetime);
            d->dateCreatedCheck->setChecked(true);
        }
    }
    d->dateCreatedSel->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncHOSTDateCheck->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncXMPDateCheck->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncIPTCDateCheck->setEnabled(d->dateCreatedCheck->isChecked());

    d->dateCreatedSubSecEdit->setValue(0);
    d->dateCreatedSubSecCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Photo.SubSecTime", false);
    if (!data.isNull())
    {
        bool ok    = false;
        int subsec = data.toInt(&ok);
        if (ok)
        {
            d->dateCreatedSubSecEdit->setValue(subsec);
            d->dateCreatedSubSecCheck->setChecked(true);
        }
    }
    d->dateCreatedSubSecEdit->setEnabled(d->dateCreatedSubSecCheck->isChecked());

    d->dateOriginalSel->setDateTime(QDateTime::currentDateTime());
    d->dateOriginalCheck->setChecked(false);
    datetimeStr = exiv2Iface.getExifTagString("Exif.Photo.DateTimeOriginal", false);
    if (!datetimeStr.isEmpty())
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateOriginalSel->setDateTime(datetime);
            d->dateOriginalCheck->setChecked(true);
        }
    }
    d->dateOriginalSel->setEnabled(d->dateOriginalCheck->isChecked());

    d->dateOriginalSubSecEdit->setValue(0);
    d->dateOriginalSubSecCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Photo.SubSecTimeOriginal", false);
    if (!data.isNull())
    {
        bool ok    = false;
        int subsec = data.toInt(&ok);
        if (ok)
        {
            d->dateOriginalSubSecEdit->setValue(subsec);
            d->dateOriginalSubSecCheck->setChecked(true);
        }
    }
    d->dateOriginalSubSecEdit->setEnabled(d->dateOriginalSubSecCheck->isChecked());

    d->dateDigitalizedSel->setDateTime(QDateTime::currentDateTime());
    d->dateDigitalizedCheck->setChecked(false);
    datetimeStr = exiv2Iface.getExifTagString("Exif.Photo.DateTimeDigitized", false);
    if (!datetimeStr.isEmpty())
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateDigitalizedSel->setDateTime(datetime);
            d->dateDigitalizedCheck->setChecked(true);
        }
    }
    d->dateDigitalizedSel->setEnabled(d->dateDigitalizedCheck->isChecked());

    d->dateDigitalizedSubSecEdit->setValue(0);
    d->dateDigitalizedSubSecCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Photo.SubSecTimeDigitized", false);
    if (!data.isNull())
    {
        bool ok    = false;
        int subsec = data.toInt(&ok);
        if (ok)
        {
            d->dateDigitalizedSubSecEdit->setValue(subsec);
            d->dateDigitalizedSubSecCheck->setChecked(true);
        }
    }
    d->dateDigitalizedSubSecEdit->setEnabled(d->dateDigitalizedSubSecCheck->isChecked());

    blockSignals(false);
}

void EXIFDateTime::applyMetadata(QByteArray& exifData, QByteArray& iptcData, QByteArray& xmpData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setIptc(iptcData);
    exiv2Iface.setXmp(xmpData);

    if (d->dateCreatedCheck->isChecked())
    {
        exiv2Iface.setExifTagString("Exif.Image.DateTime",
                   d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());

        if (exiv2Iface.supportXmp() && d->syncXMPDateCheck->isChecked())
        {
            exiv2Iface.setXmpTagString("Xmp.exif.DateTimeOriginal",
                d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
            exiv2Iface.setXmpTagString("Xmp.photoshop.DateCreated",
                d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
            exiv2Iface.setXmpTagString("Xmp.tiff.DateTime",
                d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
            exiv2Iface.setXmpTagString("Xmp.xmp.CreateDate",
                d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
            exiv2Iface.setXmpTagString("Xmp.xmp.MetadataDate",
                d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
            exiv2Iface.setXmpTagString("Xmp.xmp.ModifyDate",
                d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
        }

        if (syncIPTCDateIsChecked())
        {
            exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                       d->dateCreatedSel->dateTime().date().toString(Qt::ISODate));
            exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                       d->dateCreatedSel->dateTime().time().toString(Qt::ISODate));
        }
    }
    else
        exiv2Iface.removeExifTag("Exif.Image.DateTime");

    if (d->dateCreatedSubSecCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.SubSecTime",
                   QString::number(d->dateCreatedSubSecEdit->value()).toAscii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.SubSecTime");

    if (d->dateOriginalCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.DateTimeOriginal",
                   d->dateOriginalSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.DateTimeOriginal");

    if (d->dateOriginalSubSecCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.SubSecTimeOriginal",
                   QString::number(d->dateOriginalSubSecEdit->value()).toAscii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.SubSecTimeOriginal");

    if (d->dateDigitalizedCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.DateTimeDigitized",
                   d->dateDigitalizedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.DateTimeDigitized");

    if (d->dateDigitalizedSubSecCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.SubSecTimeDigitized",
                   QString::number(d->dateDigitalizedSubSecEdit->value()).toAscii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.SubSecTimeDigitized");

    exifData = exiv2Iface.getExif();
    iptcData = exiv2Iface.getIptc();
    xmpData  = exiv2Iface.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
