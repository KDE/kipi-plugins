/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : IPTC date and time settings page.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT includes.

#include <QDateTime>
#include <QLayout>
#include <QLabel>
#include <QValidator>
#include <QCheckBox>
#include <QTimeEdit>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <kdatewidget.h>
#include <kcomponentdata.h>
#include <kglobal.h>
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

    QTimeEdit   *timeCreatedSel;
    QTimeEdit   *timeReleasedSel;
    QTimeEdit   *timeExpiredSel;
    QTimeEdit   *timeDigitalizedSel;

    KDateWidget *dateCreatedSel;
    KDateWidget *dateReleasedSel;
    KDateWidget *dateExpiredSel;
    KDateWidget *dateDigitalizedSel;
};

IPTCDateTime::IPTCDateTime(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCDateTimePriv;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->dateCreatedCheck  = new QCheckBox(i18n("Creation date"), this);
    d->timeCreatedCheck  = new QCheckBox(i18n("Creation time"), this);
    d->dateCreatedSel    = new KDateWidget(this);
    d->timeCreatedSel    = new QTimeEdit(this);
    d->syncHOSTDateCheck = new QCheckBox(i18n("Sync creation date hosted by %1",
                                              KGlobal::mainComponent().aboutData()->programName()), 
                                              this);
    d->syncEXIFDateCheck = new QCheckBox(i18n("Sync EXIF creation date"), this);
    KSeparator *line     = new KSeparator(Qt::Horizontal, this);
    d->dateCreatedSel->setDate(QDate::currentDate());
    d->timeCreatedSel->setTime(QTime::currentTime());
    d->dateCreatedSel->setWhatsThis(i18n("<p>Set here the creation date of "
                                         "intellectual content."));
    d->timeCreatedSel->setWhatsThis(i18n("<p>Set here the creation time of "
                                         "intellectual content."));

    // --------------------------------------------------------

    d->dateReleasedCheck = new QCheckBox(i18n("Release date"), this);
    d->timeReleasedCheck = new QCheckBox(i18n("Release time"), this);
    d->dateReleasedSel   = new KDateWidget(this);
    d->timeReleasedSel   = new QTimeEdit(this);
    d->dateReleasedSel->setDate(QDate::currentDate());
    d->timeReleasedSel->setTime(QTime::currentTime());
    d->dateReleasedSel->setWhatsThis(i18n("<p>Set here the earliest intended usable date of "
                                          "intellectual content."));
    d->timeReleasedSel->setWhatsThis(i18n("<p>Set here the earliest intended usable time of "
                                          "intellectual content."));

    // --------------------------------------------------------

    d->dateExpiredCheck = new QCheckBox(i18n("Expiration date"), this);
    d->timeExpiredCheck = new QCheckBox(i18n("Expiration time"), this);
    d->dateExpiredSel   = new KDateWidget(this);
    d->timeExpiredSel   = new QTimeEdit(this);
    d->dateExpiredSel->setDate(QDate::currentDate());
    d->timeExpiredSel->setTime(QTime::currentTime());
    d->dateExpiredSel->setWhatsThis(i18n("<p>Set here the latest intended usable date of "
                                         "intellectual content."));
    d->timeExpiredSel->setWhatsThis(i18n("<p>Set here the latest intended usable time of "
                                         "intellectual content."));

    // --------------------------------------------------------

    d->dateDigitalizedCheck = new QCheckBox(i18n("Digitization date"), this);
    d->timeDigitalizedCheck = new QCheckBox(i18n("Digitization time"), this);
    d->dateDigitalizedSel   = new KDateWidget(this);
    d->timeDigitalizedSel   = new QTimeEdit(this);
    d->dateDigitalizedSel->setDate(QDate::currentDate());
    d->timeDigitalizedSel->setTime(QTime::currentTime());
    d->dateDigitalizedSel->setWhatsThis(i18n("<p>Set here the creation date of "
                                             "digital representation."));
    d->timeDigitalizedSel->setWhatsThis(i18n("<p>Set here the creation time of "
                                             "digital representation."));

    // --------------------------------------------------------

    grid->addWidget(d->dateCreatedCheck, 0, 0, 1, 1);
    grid->addWidget(d->timeCreatedCheck, 0, 1, 1, 2);
    grid->addWidget(d->dateCreatedSel, 1, 0, 1, 1);
    grid->addWidget(d->timeCreatedSel, 1, 1, 1, 1);
    grid->addWidget(d->syncHOSTDateCheck, 2, 0, 1, 3 );
    grid->addWidget(d->syncEXIFDateCheck, 3, 0, 1, 3 );
    grid->addWidget(line, 4, 0, 1, 3 ); 
    grid->addWidget(d->dateReleasedCheck, 5, 0, 1, 1);
    grid->addWidget(d->timeReleasedCheck, 5, 1, 1, 2);
    grid->addWidget(d->dateReleasedSel, 6, 0, 1, 1);
    grid->addWidget(d->timeReleasedSel, 6, 1, 1, 1);
    grid->addWidget(d->dateExpiredCheck, 7, 0, 1, 1);
    grid->addWidget(d->timeExpiredCheck, 7, 1, 1, 2);
    grid->addWidget(d->dateExpiredSel, 8, 0, 1, 1);
    grid->addWidget(d->timeExpiredSel, 8, 1, 1, 1);
    grid->addWidget(d->dateDigitalizedCheck, 9, 0, 1, 1);
    grid->addWidget(d->timeDigitalizedCheck, 9, 1, 1, 2);
    grid->addWidget(d->dateDigitalizedSel, 10, 0, 1, 1);
    grid->addWidget(d->timeDigitalizedSel, 10, 1, 1, 1);
    grid->setColumnStretch(2, 10);                     
    grid->setRowStretch(11, 10);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

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

    connect(d->dateCreatedSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->dateReleasedSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->dateExpiredSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->timeCreatedSel, SIGNAL(timeChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    connect(d->timeReleasedSel, SIGNAL(timeChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    connect(d->timeExpiredSel, SIGNAL(timeChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    connect(d->timeDigitalizedSel, SIGNAL(timeChanged(const QTime &)),
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
                    getIPTCCreationDate().toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
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
