/* ============================================================
 * Authors     : Gilles Caulier
 * Date        : 2006-10-12
 * Description : IPTC date and time settings page.
 * 
 * Copyright 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <kdatewidget.h>
#include <ktimewidget.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kseparator.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "iptcdatetime.h"
#include "iptcdatetime.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCDateTimePriv
{
public:

    IPTCDateTimePriv()
    {
        dateCreatedSel       = 0;
        dateReleasedSel      = 0;
        dateExpiredSel       = 0;
        dateDigitalizedSel   = 0;
        timeCreatedSel       = 0;
        timeReleasedSel      = 0;
        timeExpiredSel       = 0;
        timeDigitalizedSel   = 0;

        dateCreatedCheck     = 0;
        dateReleasedCheck    = 0;
        dateExpiredCheck     = 0;
        dateDigitalizedCheck = 0;
        timeCreatedCheck     = 0;
        timeReleasedCheck    = 0;
        timeExpiredCheck     = 0;
        timeDigitalizedCheck = 0;
        syncHOSTDateCheck    = 0;
        syncEXIFDateCheck    = 0;
    }

    QCheckBox   *dateCreatedCheck;
    QCheckBox   *dateReleasedCheck;
    QCheckBox   *dateExpiredCheck;
    QCheckBox   *dateDigitalizedCheck;
    QCheckBox   *timeCreatedCheck;
    QCheckBox   *timeReleasedCheck;
    QCheckBox   *timeExpiredCheck;
    QCheckBox   *timeDigitalizedCheck;
    QCheckBox   *syncHOSTDateCheck;
    QCheckBox   *syncEXIFDateCheck;

    KDateWidget *dateCreatedSel;
    KDateWidget *dateReleasedSel;
    KDateWidget *dateExpiredSel;
    KDateWidget *dateDigitalizedSel;

    KTimeWidget *timeCreatedSel;
    KTimeWidget *timeReleasedSel;
    KTimeWidget *timeExpiredSel;
    KTimeWidget *timeDigitalizedSel;
};

IPTCDateTime::IPTCDateTime(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCDateTimePriv;

    QGridLayout* grid = new QGridLayout(parent, 11, 2, KDialog::spacingHint());

    // --------------------------------------------------------

    d->dateCreatedCheck  = new QCheckBox(i18n("Creation date"), parent);
    d->timeCreatedCheck  = new QCheckBox(i18n("Creation time"), parent);
    d->dateCreatedSel    = new KDateWidget(parent);
    d->timeCreatedSel    = new KTimeWidget(parent);
    d->syncHOSTDateCheck = new QCheckBox(i18n("Sync creation date hosted by %1")
                                              .arg(KApplication::kApplication()->aboutData()->appName()), 
                                              parent);
    d->syncEXIFDateCheck = new QCheckBox(i18n("Sync EXIF creation date"), parent);
    KSeparator *line     = new KSeparator(Horizontal, parent);
    d->dateCreatedSel->setDate(QDate::currentDate());
    d->timeCreatedSel->setTime(QTime::currentTime());
    grid->addMultiCellWidget(d->dateCreatedCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->timeCreatedCheck, 0, 0, 1, 2);
    grid->addMultiCellWidget(d->dateCreatedSel, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->timeCreatedSel, 1, 1, 1, 1);
    grid->addMultiCellWidget(d->syncHOSTDateCheck, 2, 2, 0, 2);
    grid->addMultiCellWidget(d->syncEXIFDateCheck, 3, 3, 0, 2);
    grid->addMultiCellWidget(line, 4, 4, 0, 2);
    QWhatsThis::add(d->dateCreatedSel, i18n("<p>Set here the creation date of "
                    "intellectual content."));
    QWhatsThis::add(d->timeCreatedSel, i18n("<p>Set here the creation time of "
                    "intellectual content."));

    // --------------------------------------------------------

    d->dateReleasedCheck = new QCheckBox(i18n("Release date"), parent);
    d->timeReleasedCheck = new QCheckBox(i18n("Release time"), parent);
    d->dateReleasedSel   = new KDateWidget(parent);
    d->timeReleasedSel   = new KTimeWidget(parent);
    d->dateReleasedSel->setDate(QDate::currentDate());
    d->timeReleasedSel->setTime(QTime::currentTime());
    grid->addMultiCellWidget(d->dateReleasedCheck, 5, 5, 0, 0);
    grid->addMultiCellWidget(d->timeReleasedCheck, 5, 5, 1, 2);
    grid->addMultiCellWidget(d->dateReleasedSel, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->timeReleasedSel, 6, 6, 1, 1);
    QWhatsThis::add(d->dateReleasedSel, i18n("<p>Set here the earliest intended usable date of "
                    "intellectual content."));
    QWhatsThis::add(d->timeReleasedSel, i18n("<p>Set here the earliest intended usable time of "
                    "intellectual content."));

    // --------------------------------------------------------

    d->dateExpiredCheck = new QCheckBox(i18n("Expiration date"), parent);
    d->timeExpiredCheck = new QCheckBox(i18n("Expiration time"), parent);
    d->dateExpiredSel   = new KDateWidget(parent);
    d->timeExpiredSel   = new KTimeWidget(parent);
    d->dateExpiredSel->setDate(QDate::currentDate());
    d->timeExpiredSel->setTime(QTime::currentTime());
    grid->addMultiCellWidget(d->dateExpiredCheck, 7, 7, 0, 0);
    grid->addMultiCellWidget(d->timeExpiredCheck, 7, 7, 1, 2);
    grid->addMultiCellWidget(d->dateExpiredSel, 8, 8, 0, 0);
    grid->addMultiCellWidget(d->timeExpiredSel, 8, 8, 1, 1);
    QWhatsThis::add(d->dateExpiredSel, i18n("<p>Set here the latest intended usable date of "
                    "intellectual content."));
    QWhatsThis::add(d->timeExpiredSel, i18n("<p>Set here the latest intended usable time of "
                    "intellectual content."));

    // --------------------------------------------------------

    d->dateDigitalizedCheck = new QCheckBox(i18n("Digitization date"), parent);
    d->timeDigitalizedCheck = new QCheckBox(i18n("Digitization time"), parent);
    d->dateDigitalizedSel   = new KDateWidget(parent);
    d->timeDigitalizedSel   = new KTimeWidget(parent);
    d->dateDigitalizedSel->setDate(QDate::currentDate());
    d->timeDigitalizedSel->setTime(QTime::currentTime());
    grid->addMultiCellWidget(d->dateDigitalizedCheck, 9, 9, 0, 0);
    grid->addMultiCellWidget(d->timeDigitalizedCheck, 9, 9, 1, 2);
    grid->addMultiCellWidget(d->dateDigitalizedSel, 10, 10, 0, 0);
    grid->addMultiCellWidget(d->timeDigitalizedSel, 10, 10, 1, 1);
    QWhatsThis::add(d->dateDigitalizedSel, i18n("<p>Set here the creation date of "
                    "digital representation."));
    QWhatsThis::add(d->timeDigitalizedSel, i18n("<p>Set here the creation time of "
                    "digital representation."));

    grid->setColStretch(2, 10);                     
    grid->setRowStretch(11, 10);                     

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->dateCreatedSel, SLOT(setEnabled(bool)));

    connect(d->dateReleasedCheck, SIGNAL(toggled(bool)),
            d->dateReleasedSel, SLOT(setEnabled(bool)));

    connect(d->dateExpiredCheck, SIGNAL(toggled(bool)),
            d->dateExpiredSel, SLOT(setEnabled(bool)));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            d->dateDigitalizedSel, SLOT(setEnabled(bool)));

    connect(d->timeCreatedCheck, SIGNAL(toggled(bool)),
            d->timeCreatedSel, SLOT(setEnabled(bool)));

    connect(d->timeReleasedCheck, SIGNAL(toggled(bool)),
            d->timeReleasedSel, SLOT(setEnabled(bool)));

    connect(d->timeExpiredCheck, SIGNAL(toggled(bool)),
            d->timeExpiredSel, SLOT(setEnabled(bool)));

    connect(d->timeDigitalizedCheck, SIGNAL(toggled(bool)),
            d->timeDigitalizedSel, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncHOSTDateCheck, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncEXIFDateCheck, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateReleasedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateExpiredCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeCreatedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeReleasedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeExpiredCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeDigitalizedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->dateCreatedSel, SIGNAL(changed(QDate)),
            this, SIGNAL(signalModified()));

    connect(d->dateReleasedSel, SIGNAL(changed(QDate)),
            this, SIGNAL(signalModified()));

    connect(d->dateExpiredSel, SIGNAL(changed(QDate)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSel, SIGNAL(changed(QDate)),
            this, SIGNAL(signalModified()));

    connect(d->timeCreatedSel, SIGNAL(valueChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    connect(d->timeReleasedSel, SIGNAL(valueChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    connect(d->timeExpiredSel, SIGNAL(valueChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    connect(d->timeDigitalizedSel, SIGNAL(valueChanged(const QTime &)),
            this, SIGNAL(signalModified()));
}

IPTCDateTime::~IPTCDateTime()
{
    delete d;
}

bool IPTCDateTime::syncHOSTDateIsChecked()
{
    return d->syncHOSTDateCheck->isChecked();
}

bool IPTCDateTime::syncEXIFDateIsChecked()
{
    return d->syncEXIFDateCheck->isChecked();
}

void IPTCDateTime::setCheckedSyncHOSTDate(bool c)
{
    d->syncHOSTDateCheck->setChecked(c);
}

void IPTCDateTime::setCheckedSyncEXIFDate(bool c)
{
    d->syncEXIFDateCheck->setChecked(c);
}

QDateTime IPTCDateTime::getIPTCCreationDate()
{
    return QDateTime(d->dateCreatedSel->date(), d->timeCreatedSel->time());
}

void IPTCDateTime::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    QDate date;
    QTime time;
    QString dateStr, timeStr;

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.DateCreated", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.TimeCreated", false);

    d->dateCreatedSel->setDate(QDate::currentDate());
    d->dateCreatedCheck->setChecked(false);
    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateCreatedSel->setDate(date);
            d->dateCreatedCheck->setChecked(true);
        }
    }    
    d->dateCreatedSel->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncHOSTDateCheck->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncEXIFDateCheck->setEnabled(d->dateCreatedCheck->isChecked());

    d->timeCreatedSel->setTime(QTime::currentTime());
    d->timeCreatedCheck->setChecked(false);
    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeCreatedSel->setTime(time);
            d->timeCreatedCheck->setChecked(true);
        }
    }    
    d->timeCreatedSel->setEnabled(d->timeCreatedCheck->isChecked());

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.ReleaseDate", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.ReleaseTime", false);

    d->dateReleasedSel->setDate(QDate::currentDate());
    d->dateReleasedCheck->setChecked(false);
    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateReleasedSel->setDate(date);
            d->dateReleasedCheck->setChecked(true);
        }
    }    
    d->dateReleasedSel->setEnabled(d->dateReleasedCheck->isChecked());

    d->timeReleasedSel->setTime(QTime::currentTime());
    d->timeReleasedCheck->setChecked(false);
    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeReleasedSel->setTime(time);
            d->timeReleasedCheck->setChecked(true);
        }
    }    
    d->timeReleasedSel->setEnabled(d->timeReleasedCheck->isChecked());

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.ExpirationDate", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.ExpirationTime", false);

    d->dateExpiredSel->setDate(QDate::currentDate());
    d->dateExpiredCheck->setChecked(false);
    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateExpiredSel->setDate(date);
            d->dateExpiredCheck->setChecked(true);
        }
    }    
    d->dateExpiredSel->setEnabled(d->dateExpiredCheck->isChecked());

    d->timeExpiredSel->setTime(QTime::currentTime());
    d->timeExpiredCheck->setChecked(false);
    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeExpiredSel->setTime(time);
            d->timeExpiredCheck->setChecked(true);
        }
    }   
    d->timeExpiredSel->setEnabled(d->timeExpiredCheck->isChecked());

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.DigitizationDate", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.DigitizationTime", false);

    d->dateDigitalizedSel->setDate(QDate::currentDate());
    d->dateDigitalizedCheck->setChecked(false);
    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateDigitalizedSel->setDate(date);
            d->dateDigitalizedCheck->setChecked(true);
        }
    }    
    d->dateDigitalizedSel->setEnabled(d->dateDigitalizedCheck->isChecked());

    d->timeDigitalizedSel->setTime(QTime::currentTime());
    d->timeDigitalizedCheck->setChecked(false);
    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeDigitalizedSel->setTime(time);
            d->timeDigitalizedCheck->setChecked(true);
        }
    }   
    d->timeDigitalizedSel->setEnabled(d->timeDigitalizedCheck->isChecked());

    blockSignals(false);
}

void IPTCDateTime::applyMetadata(QByteArray& exifData, QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setIptc(iptcData);

    if (d->dateCreatedCheck->isChecked())
    {
        exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                                    d->dateCreatedSel->date().toString(Qt::ISODate));
        if (syncEXIFDateIsChecked())
        {
            exiv2Iface.setExifTagString("Exif.Image.DateTime",
                    getIPTCCreationDate().toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
        }
    }
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.DateCreated");

    if (d->dateReleasedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ReleaseDate",
                                    d->dateReleasedSel->date().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ReleaseDate");

    if (d->dateExpiredCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ExpirationDate",
                                    d->dateExpiredSel->date().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ExpirationDate");

    if (d->dateDigitalizedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.DigitizationDate",
                                    d->dateDigitalizedSel->date().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.DigitizationDate");

    if (d->timeCreatedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                                    d->timeCreatedSel->time().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.TimeCreated");

    if (d->timeReleasedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ReleaseTime",
                                    d->timeReleasedSel->time().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ReleaseTime");

    if (d->timeExpiredCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ExpirationTime",
                                    d->timeExpiredSel->time().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ExpirationTime");

    if (d->timeDigitalizedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.DigitizationTime",
                                    d->timeDigitalizedSel->time().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.DigitizationTime");

    exifData = exiv2Iface.getExif();
    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

