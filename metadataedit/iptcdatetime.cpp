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
#include <QPushButton>

// KDE includes.

#include <kiconloader.h>
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
        dateReleasedSel        = 0;
        dateExpiredSel         = 0;
        timeReleasedSel        = 0;
        timeExpiredSel         = 0;
        dateReleasedCheck      = 0;
        dateExpiredCheck       = 0;
        timeReleasedCheck      = 0;
        timeExpiredCheck       = 0;
        setTodayReleasedBtn    = 0;
        setTodayExpiredBtn     = 0;
    }

    QCheckBox   *dateReleasedCheck;
    QCheckBox   *dateExpiredCheck;
    QCheckBox   *timeReleasedCheck;
    QCheckBox   *timeExpiredCheck;

    QTimeEdit   *timeReleasedSel;
    QTimeEdit   *timeExpiredSel;

    QPushButton *setTodayReleasedBtn;
    QPushButton *setTodayExpiredBtn;

    KDateWidget *dateReleasedSel;
    KDateWidget *dateExpiredSel;
};

IPTCDateTime::IPTCDateTime(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCDateTimePriv;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->dateReleasedCheck   = new QCheckBox(i18n("Release date"), this);
    d->timeReleasedCheck   = new QCheckBox(i18n("Release time"), this);
    d->dateReleasedSel     = new KDateWidget(this);
    d->timeReleasedSel     = new QTimeEdit(this);

    d->setTodayReleasedBtn = new QPushButton();
    d->setTodayReleasedBtn->setIcon(SmallIcon("calendar-today"));
    d->setTodayReleasedBtn->setWhatsThis(i18n("Set release date to today"));

    d->dateReleasedSel->setWhatsThis(i18n("<p>Set here the earliest intended usable date of "
                                          "intellectual content."));
    d->timeReleasedSel->setWhatsThis(i18n("<p>Set here the earliest intended usable time of "
                                          "intellectual content."));
    slotSetTodayReleased();

    // --------------------------------------------------------

    d->dateExpiredCheck   = new QCheckBox(i18n("Expiration date"), this);
    d->timeExpiredCheck   = new QCheckBox(i18n("Expiration time"), this);
    d->dateExpiredSel     = new KDateWidget(this);
    d->timeExpiredSel     = new QTimeEdit(this);

    d->setTodayExpiredBtn = new QPushButton();
    d->setTodayExpiredBtn->setIcon(SmallIcon("calendar-today"));
    d->setTodayExpiredBtn->setWhatsThis(i18n("Set expiration date to today"));

    d->dateExpiredSel->setWhatsThis(i18n("<p>Set here the latest intended usable date of "
                                         "intellectual content."));
    d->timeExpiredSel->setWhatsThis(i18n("<p>Set here the latest intended usable time of "
                                         "intellectual content."));

    slotSetTodayExpired();

    // --------------------------------------------------------

    grid->addWidget(d->dateReleasedCheck, 7, 0, 1, 1);
    grid->addWidget(d->timeReleasedCheck, 7, 1, 1, 3);
    grid->addWidget(d->dateReleasedSel, 8, 0, 1, 1);
    grid->addWidget(d->timeReleasedSel, 8, 1, 1, 1);
    grid->addWidget(d->setTodayReleasedBtn, 8, 3, 1, 1);
    grid->addWidget(d->dateExpiredCheck, 9, 0, 1, 1);
    grid->addWidget(d->timeExpiredCheck, 9, 1, 1, 3);
    grid->addWidget(d->dateExpiredSel, 10, 0, 1, 1);
    grid->addWidget(d->timeExpiredSel, 10, 1, 1, 1);
    grid->addWidget(d->setTodayExpiredBtn, 10, 3, 1, 1);
    grid->setColumnStretch(2, 10);                     
    grid->setRowStretch(11, 10);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->dateReleasedCheck, SIGNAL(toggled(bool)),
            d->dateReleasedSel, SLOT(setEnabled(bool)));

    connect(d->dateExpiredCheck, SIGNAL(toggled(bool)),
            d->dateExpiredSel, SLOT(setEnabled(bool)));

    connect(d->timeReleasedCheck, SIGNAL(toggled(bool)),
            d->timeReleasedSel, SLOT(setEnabled(bool)));

    connect(d->timeExpiredCheck, SIGNAL(toggled(bool)),
            d->timeExpiredSel, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->dateReleasedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateExpiredCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeReleasedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeExpiredCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->dateReleasedSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->dateExpiredSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->timeReleasedSel, SIGNAL(timeChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    connect(d->timeExpiredSel, SIGNAL(timeChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->setTodayReleasedBtn, SIGNAL(clicked()),
            this, SLOT(slotSetTodayReleased()));

    connect(d->setTodayExpiredBtn, SIGNAL(clicked()),
            this, SLOT(slotSetTodayExpired()));
}

IPTCDateTime::~IPTCDateTime()
{
    delete d;
}

void IPTCDateTime::slotSetTodayReleased()
{
    d->dateReleasedSel->setDate(QDate::currentDate());
    d->timeReleasedSel->setTime(QTime::currentTime());
}

void IPTCDateTime::slotSetTodayExpired()
{
    d->dateExpiredSel->setDate(QDate::currentDate());
    d->timeExpiredSel->setTime(QTime::currentTime());
}

void IPTCDateTime::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    QDate date;
    QTime time;
    QString dateStr, timeStr;

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
    
    blockSignals(false);
}

void IPTCDateTime::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

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

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
