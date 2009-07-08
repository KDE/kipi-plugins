/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-10
 * Description : IPTC workflow status properties settings page.
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "iptcproperties.h"
#include "iptcproperties.moc"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QTimeEdit>
#include <QValidator>
#include <QtCore/QtDebug>
#include <QGridLayout>

// KDE includes

#include <kcombobox.h>
#include <kdatewidget.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klanguagebutton.h>
#include <klineedit.h>
#include <klocale.h>
#include <kseparator.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "metadatacheckbox.h"
#include "objectattributesedit.h"
#include "pluginsversion.h"

namespace KIPIMetadataEditPlugin
{

class IPTCPropertiesPriv
{
public:

    IPTCPropertiesPriv()
    {
        dateReleasedSel     = 0;
        dateExpiredSel      = 0;
        timeReleasedSel     = 0;
        timeExpiredSel      = 0;
        dateReleasedCheck   = 0;
        dateExpiredCheck    = 0;
        timeReleasedCheck   = 0;
        timeExpiredCheck    = 0;
        setTodayReleasedBtn = 0;
        setTodayExpiredBtn  = 0;
        priorityCB          = 0;
        priorityCheck       = 0;
        objectCycleCB       = 0;
        objectTypeCB        = 0;
        objectCycleCheck    = 0;
        objectTypeCheck     = 0;
        objectAttribute     = 0;
        languageBtn         = 0;
        languageCheck       = 0;
        originalTransCheck  = 0;
        originalTransEdit   = 0;

    }

    QCheckBox                     *dateReleasedCheck;
    QCheckBox                     *timeReleasedCheck;
    QCheckBox                     *dateExpiredCheck;
    QCheckBox                     *timeExpiredCheck;
    QCheckBox                     *originalTransCheck;

    QTimeEdit                     *timeReleasedSel;
    QTimeEdit                     *timeExpiredSel;

    QPushButton                   *setTodayReleasedBtn;
    QPushButton                   *setTodayExpiredBtn;

    KComboBox                     *priorityCB;
    KComboBox                     *objectCycleCB;
    KComboBox                     *objectTypeCB;

    KLineEdit                     *objectTypeDescEdit;
    KLineEdit                     *originalTransEdit;

    KLanguageButton               *languageBtn;

    KDateWidget                   *dateReleasedSel;
    KDateWidget                   *dateExpiredSel;

    MetadataCheckBox              *priorityCheck;
    MetadataCheckBox              *objectCycleCheck;
    MetadataCheckBox              *objectTypeCheck;
    MetadataCheckBox              *languageCheck;

    ObjectAttributesEdit          *objectAttribute;
};

IPTCProperties::IPTCProperties(QWidget* parent)
              : QWidget(parent), d(new IPTCPropertiesPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->dateReleasedCheck   = new QCheckBox(i18n("Release date"), this);
    d->timeReleasedCheck   = new QCheckBox(i18n("Release time"), this);
    d->dateReleasedSel     = new KDateWidget(this);
    d->timeReleasedSel     = new QTimeEdit(this);

    d->setTodayReleasedBtn = new QPushButton();
    d->setTodayReleasedBtn->setIcon(SmallIcon("go-jump-today"));
    d->setTodayReleasedBtn->setWhatsThis(i18n("Set release date to today"));

    d->dateReleasedSel->setWhatsThis(i18n("Set here the earliest intended usable date of "
                                          "intellectual content."));
    d->timeReleasedSel->setWhatsThis(i18n("Set here the earliest intended usable time of "
                                          "intellectual content."));
    slotSetTodayReleased();

    // --------------------------------------------------------

    d->dateExpiredCheck   = new QCheckBox(i18n("Expiration date"), this);
    d->timeExpiredCheck   = new QCheckBox(i18n("Expiration time"), this);
    d->dateExpiredSel     = new KDateWidget(this);
    d->timeExpiredSel     = new QTimeEdit(this);

    d->setTodayExpiredBtn = new QPushButton();
    d->setTodayExpiredBtn->setIcon(SmallIcon("go-jump-today"));
    d->setTodayExpiredBtn->setWhatsThis(i18n("Set expiration date to today"));

    d->dateExpiredSel->setWhatsThis(i18n("Set here the latest intended usable date of "
                                         "intellectual content."));
    d->timeExpiredSel->setWhatsThis(i18n("Set here the latest intended usable time of "
                                         "intellectual content."));

    slotSetTodayExpired();

    // --------------------------------------------------------

    d->languageCheck = new MetadataCheckBox(i18n("Language:"), this);
    d->languageBtn   = new KLanguageButton(this);

    QStringList list = KGlobal::locale()->allLanguagesList();
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
    {
        // Only get all ISO 639 language code based on 2 characters
        // http://xml.coverpages.org/iso639a.html
        if ((*it).size() == 2 )
            d->languageBtn->insertLanguage(*it);
    }

    d->languageBtn->setWhatsThis(i18n("Select here the language of content."));

    // --------------------------------------------------------

    d->priorityCheck = new MetadataCheckBox(i18n("Priority:"), this);
    d->priorityCB    = new KComboBox(this);
    d->priorityCB->insertItem(0, i18n("0: None"));
    d->priorityCB->insertItem(1, i18n("1: High"));
    d->priorityCB->insertItem(2, "2");
    d->priorityCB->insertItem(3, "3");
    d->priorityCB->insertItem(4, "4");
    d->priorityCB->insertItem(5, i18n("5: Normal"));
    d->priorityCB->insertItem(6, "6");
    d->priorityCB->insertItem(7, "7");
    d->priorityCB->insertItem(8, i18n("8: Low"));
    d->priorityCB->setWhatsThis(i18n("Select here the editorial urgency of content."));

    // --------------------------------------------------------

    d->objectCycleCheck = new MetadataCheckBox(i18n("Cycle:"), this);
    d->objectCycleCB    = new KComboBox(this);
    d->objectCycleCB->insertItem(0, i18n("Morning"));
    d->objectCycleCB->insertItem(1, i18n("Afternoon"));
    d->objectCycleCB->insertItem(2, i18n("Evening"));
    d->objectCycleCB->setWhatsThis(i18n("Select here the editorial cycle of content."));

    // --------------------------------------------------------

    d->objectTypeCheck    = new MetadataCheckBox(i18n("Type:"), this);
    d->objectTypeCB       = new KComboBox(this);
    d->objectTypeDescEdit = new KLineEdit(this);
    d->objectTypeDescEdit->setClearButtonShown(true);
    d->objectTypeDescEdit->setValidator(asciiValidator);
    d->objectTypeDescEdit->setMaxLength(64);
    d->objectTypeCB->insertItem(0, i18n("News"));
    d->objectTypeCB->insertItem(1, i18n("Data"));
    d->objectTypeCB->insertItem(2, i18n("Advisory"));
    d->objectTypeCB->setWhatsThis(i18n("Select here the editorial type of content."));
    d->objectTypeDescEdit->setWhatsThis(i18n("Set here the editorial type description of content. "
                                             "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->objectAttribute = new ObjectAttributesEdit(this, true, 64);

    // --------------------------------------------------------

    d->originalTransCheck = new QCheckBox(i18n("Reference:"), this);
    d->originalTransEdit  = new KLineEdit(this);
    d->originalTransEdit->setClearButtonShown(true);
    d->originalTransEdit->setValidator(asciiValidator);
    d->originalTransEdit->setMaxLength(32);
    d->originalTransEdit->setWhatsThis(i18n("Set here the original content transmission "
                                            "reference. This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters and limit string sizes. "
                 "Use contextual help for details.</b>"), this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->addWidget(d->dateReleasedCheck,                   0, 0, 1, 2);
    grid->addWidget(d->timeReleasedCheck,                   0, 2, 1, 2);
    grid->addWidget(d->dateReleasedSel,                     1, 0, 1, 2);
    grid->addWidget(d->timeReleasedSel,                     1, 2, 1, 1);
    grid->addWidget(d->setTodayReleasedBtn,                 1, 4, 1, 1);
    grid->addWidget(d->dateExpiredCheck,                    2, 0, 1, 2);
    grid->addWidget(d->timeExpiredCheck,                    2, 2, 1, 2);
    grid->addWidget(d->dateExpiredSel,                      3, 0, 1, 2);
    grid->addWidget(d->timeExpiredSel,                      3, 2, 1, 1);
    grid->addWidget(d->setTodayExpiredBtn,                  3, 4, 1, 1);
    grid->addWidget(new KSeparator(Qt::Horizontal, this),   4, 0, 1, 5);
    grid->addWidget(d->languageCheck,                       5, 0, 1, 1);
    grid->addWidget(d->languageBtn,                         5, 1, 1, 1);
    grid->addWidget(d->priorityCheck,                       6, 0, 1, 1);
    grid->addWidget(d->priorityCB,                          6, 1, 1, 1);
    grid->addWidget(d->objectCycleCheck,                    7, 0, 1, 1);
    grid->addWidget(d->objectCycleCB,                       7, 1, 1, 1);
    grid->addWidget(d->objectTypeCheck,                     8, 0, 1, 1);
    grid->addWidget(d->objectTypeCB,                        8, 1, 1, 1);
    grid->addWidget(d->objectTypeDescEdit,                  8, 2, 1, 3);
    grid->addWidget(new KSeparator(Qt::Horizontal, this),   9, 0, 1, 5);
    grid->addWidget(d->objectAttribute,                    10, 0, 1, 5);
    grid->addWidget(new KSeparator(Qt::Horizontal, this),  11, 0, 1, 5);
    grid->addWidget(d->originalTransCheck,                 12, 0, 1, 1);
    grid->addWidget(d->originalTransEdit,                  12, 1, 1, 4);
    grid->addWidget(note,                                  13, 0, 1, 5);
    grid->setColumnStretch(3, 10);
    grid->setRowStretch(14, 10);
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

    connect(d->languageCheck, SIGNAL(toggled(bool)),
            d->languageBtn, SLOT(setEnabled(bool)));

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            d->priorityCB, SLOT(setEnabled(bool)));

    connect(d->objectCycleCheck, SIGNAL(toggled(bool)),
            d->objectCycleCB, SLOT(setEnabled(bool)));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            d->objectTypeCB, SLOT(setEnabled(bool)));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            d->objectTypeDescEdit, SLOT(setEnabled(bool)));

    connect(d->originalTransCheck, SIGNAL(toggled(bool)),
            d->originalTransEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->dateReleasedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateExpiredCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeReleasedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeExpiredCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->languageCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectCycleCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectAttribute, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->originalTransCheck, SIGNAL(toggled(bool)),
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

    // --------------------------------------------------------

    connect(d->languageBtn, SIGNAL(activated(const QString&)),
            this, SIGNAL(signalModified()));

    connect(d->priorityCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectCycleCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeDescEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->originalTransEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

IPTCProperties::~IPTCProperties()
{
    delete d;
}

void IPTCProperties::slotSetTodayReleased()
{
    d->dateReleasedSel->setDate(QDate::currentDate());
    d->timeReleasedSel->setTime(QTime::currentTime());
}

void IPTCProperties::slotSetTodayExpired()
{
    d->dateExpiredSel->setDate(QDate::currentDate());
    d->timeExpiredSel->setTime(QTime::currentTime());
}

void IPTCProperties::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    int         val;
    QString     data;
    QStringList list;
    QDate       date;
    QTime       time;
    QString     dateStr, timeStr;

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

    d->languageCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Language", false);
    if (!data.isNull())
    {
        if (d->languageBtn->contains(data))
        {
            d->languageBtn->setCurrentItem(data);
            d->languageCheck->setChecked(true);
        }
        else
            d->languageCheck->setValid(false);
    }
    d->languageBtn->setEnabled(d->languageCheck->isChecked());

    d->priorityCB->setCurrentIndex(0);
    d->priorityCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Urgency", false);
    if (!data.isNull())
    {
        val = data.toInt();
        if (val >= 0 && val <= 8)
        {
            d->priorityCB->setCurrentIndex(val);
            d->priorityCheck->setChecked(true);
        }
        else
            d->priorityCheck->setValid(false);
    }
    d->priorityCB->setEnabled(d->priorityCheck->isChecked());

    d->objectCycleCB->setCurrentIndex(0);
    d->objectCycleCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.ObjectCycle", false);
    if (!data.isNull())
    {
        if (data == QString("a"))
        {
            d->objectCycleCB->setCurrentIndex(0);
            d->objectCycleCheck->setChecked(true);
        }
        else if (data == QString("b"))
        {
            d->objectCycleCB->setCurrentIndex(1);
            d->objectCycleCheck->setChecked(true);
        }
        else if (data == QString("c"))
        {
            d->objectCycleCB->setCurrentIndex(2);
            d->objectCycleCheck->setChecked(true);
        }
        else
            d->objectCycleCheck->setValid(false);
    }
    d->objectCycleCB->setEnabled(d->objectCycleCheck->isChecked());

    d->objectTypeCB->setCurrentIndex(0);
    d->objectTypeDescEdit->clear();
    d->objectTypeCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.ObjectType", false);
    if (!data.isNull())
    {
        QString typeSec = data.section(":", 0, 0);
        if (!typeSec.isEmpty())
        {
            int type = typeSec.toInt()-1;
            if (type >= 0 && type < 3)
            {
                d->objectTypeCB->setCurrentIndex(type);
                d->objectTypeDescEdit->setText(data.section(":", -1));
                d->objectTypeCheck->setChecked(true);
            }
            else
                d->objectTypeCheck->setValid(false);
        }
    }
    d->objectTypeCB->setEnabled(d->objectTypeCheck->isChecked());
    d->objectTypeDescEdit->setEnabled(d->objectTypeCheck->isChecked());

    list = exiv2Iface.getIptcTagsStringList("Iptc.Application2.ObjectAttribute", false);
    d->objectAttribute->setValues(list);

    d->originalTransEdit->clear();
    d->originalTransCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.TransmissionReference", false);
    if (!data.isNull())
    {
        d->originalTransEdit->setText(data);
        d->originalTransCheck->setChecked(true);
    }
    d->originalTransEdit->setEnabled(d->originalTransCheck->isChecked());

    blockSignals(false);
}

void IPTCProperties::applyMetadata(QByteArray& iptcData)
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

    if (d->languageCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Language", d->languageBtn->current());
    else if (d->languageCheck->isValid())
        exiv2Iface.removeIptcTag("Iptc.Application2.Language");

    if (d->priorityCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Urgency", QString::number(d->priorityCB->currentIndex()));
    else if (d->priorityCheck->isValid())
        exiv2Iface.removeIptcTag("Iptc.Application2.Urgency");

    if (d->objectCycleCheck->isChecked())
    {
        switch (d->objectCycleCB->currentIndex())
        {
            case(0):
                exiv2Iface.setIptcTagString("Iptc.Application2.ObjectCycle", QString("a"));
                break;

            case(1):
                exiv2Iface.setIptcTagString("Iptc.Application2.ObjectCycle", QString("b"));
                break;

            case(2):
                exiv2Iface.setIptcTagString("Iptc.Application2.ObjectCycle", QString("c"));
                break;
        }
    }
    else if (d->objectCycleCheck->isValid())
        exiv2Iface.removeIptcTag("Iptc.Application2.ObjectCycle");

    if (d->objectTypeCheck->isChecked())
    {
        QString objectType;
        objectType.sprintf("%2d", d->objectTypeCB->currentIndex()+1);
        objectType.append(QString(":%1").arg(d->objectTypeDescEdit->text()));
        exiv2Iface.setIptcTagString("Iptc.Application2.ObjectType", objectType);
    }
    else if (d->objectTypeCheck->isValid())
        exiv2Iface.removeIptcTag("Iptc.Application2.ObjectType");

    QStringList oldList, newList;
    if (d->objectAttribute->getValues(oldList, newList))
        exiv2Iface.setIptcTagsStringList("Iptc.Application2.ObjectAttribute", 64, oldList, newList);
    else if (d->objectAttribute->isValid())
        exiv2Iface.removeIptcTag("Iptc.Application2.ObjectAttribute");

    if (d->originalTransCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.TransmissionReference", d->originalTransEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.TransmissionReference");

    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
