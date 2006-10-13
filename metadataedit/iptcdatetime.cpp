/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : IPTC date and time settings page.
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
#include <kdatewidget.h>
#include <ktimewidget.h>

// Local includes.

#include "exiv2iface.h"
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
    }

    QCheckBox   *dateCreatedCheck;
    QCheckBox   *dateReleasedCheck;
    QCheckBox   *dateExpiredCheck;
    QCheckBox   *dateDigitalizedCheck;
    QCheckBox   *timeCreatedCheck;
    QCheckBox   *timeReleasedCheck;
    QCheckBox   *timeExpiredCheck;
    QCheckBox   *timeDigitalizedCheck;

    KDateWidget *dateCreatedSel;
    KDateWidget *dateReleasedSel;
    KDateWidget *dateExpiredSel;
    KDateWidget *dateDigitalizedSel;

    KTimeWidget *timeCreatedSel;
    KTimeWidget *timeReleasedSel;
    KTimeWidget *timeExpiredSel;
    KTimeWidget *timeDigitalizedSel;
};

IPTCDateTime::IPTCDateTime(QWidget* parent, QByteArray& iptcData)
          : QWidget(parent)
{
    d = new IPTCDateTimePriv;

    QGridLayout* grid = new QGridLayout(parent, 8, 3, KDialog::spacingHint());

    // --------------------------------------------------------

    d->dateCreatedCheck = new QCheckBox(i18n("Date created"), parent);
    d->timeCreatedCheck = new QCheckBox(i18n("Time created"), parent);
    d->dateCreatedSel   = new KDateWidget(parent);
    d->timeCreatedSel   = new KTimeWidget(parent);
    d->dateCreatedSel->setDate(QDate::currentDate());
    d->timeCreatedSel->setTime(QTime::currentTime());
    grid->addMultiCellWidget(d->dateCreatedCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->timeCreatedCheck, 0, 0, 1, 1);
    grid->addMultiCellWidget(d->dateCreatedSel, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->timeCreatedSel, 1, 1, 1, 1);
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
    grid->addMultiCellWidget(d->dateReleasedCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->timeReleasedCheck, 2, 2, 1, 1);
    grid->addMultiCellWidget(d->dateReleasedSel, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->timeReleasedSel, 3, 3, 1, 1);
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
    grid->addMultiCellWidget(d->dateExpiredCheck, 4, 4, 0, 0);
    grid->addMultiCellWidget(d->timeExpiredCheck, 4, 4, 1, 1);
    grid->addMultiCellWidget(d->dateExpiredSel, 5, 5, 0, 0);
    grid->addMultiCellWidget(d->timeExpiredSel, 5, 5, 1, 1);
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
    grid->addMultiCellWidget(d->dateDigitalizedCheck, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->timeDigitalizedCheck, 6, 6, 1, 1);
    grid->addMultiCellWidget(d->dateDigitalizedSel, 7, 7, 0, 0);
    grid->addMultiCellWidget(d->timeDigitalizedSel, 7, 7, 1, 1);
    QWhatsThis::add(d->dateDigitalizedSel, i18n("<p>Set here the creation date of "
                    "digital representation."));
    QWhatsThis::add(d->timeDigitalizedSel, i18n("<p>Set here the creation time of "
                    "digital representation."));

    grid->setColStretch(2, 10);                     
    grid->setRowStretch(8, 10);                     

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

    // --------------------------------------------------------
    
    readMetadata(iptcData);
}

IPTCDateTime::~IPTCDateTime()
{
    delete d;
}

void IPTCDateTime::readMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    QDate date;
    QTime time;
    QString dateStr, timeStr;

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.DateCreated", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.TimeCreated", false);

    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateCreatedCheck->setChecked(true);
            d->dateCreatedSel->setDate(date);
        }
    }    
    d->dateCreatedSel->setEnabled(d->dateCreatedCheck->isChecked());

    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeCreatedCheck->setChecked(true);
            d->timeCreatedSel->setTime(time);
        }
    }    
    d->timeCreatedSel->setEnabled(d->timeCreatedCheck->isChecked());

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.ReleaseDate", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.ReleaseTime", false);

    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateReleasedCheck->setChecked(true);
            d->dateReleasedSel->setDate(date);
        }
    }    
    d->dateReleasedSel->setEnabled(d->dateReleasedCheck->isChecked());

    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeReleasedCheck->setChecked(true);
            d->timeReleasedSel->setTime(time);
        }
    }    
    d->timeReleasedSel->setEnabled(d->timeReleasedCheck->isChecked());

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.ExpirationDate", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.ExpirationTime", false);

    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateExpiredCheck->setChecked(true);
            d->dateExpiredSel->setDate(date);
        }
    }    
    d->dateExpiredSel->setEnabled(d->dateExpiredCheck->isChecked());

    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeExpiredCheck->setChecked(true);
            d->timeExpiredSel->setTime(time);
        }
    }   
    d->timeExpiredSel->setEnabled(d->timeExpiredCheck->isChecked());

    dateStr = exiv2Iface.getIptcTagString("Iptc.Application2.DigitizationDate", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Application2.DigitizationTime", false);

    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateDigitalizedCheck->setChecked(true);
            d->dateDigitalizedSel->setDate(date);
        }
    }    
    d->dateDigitalizedSel->setEnabled(d->dateDigitalizedCheck->isChecked());

    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeDigitalizedCheck->setChecked(true);
            d->timeDigitalizedSel->setTime(time);
        }
    }   
    d->timeDigitalizedSel->setEnabled(d->timeDigitalizedCheck->isChecked());
}

void IPTCDateTime::applyMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->dateCreatedCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                                    d->dateCreatedSel->date().toString(Qt::ISODate));
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

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

