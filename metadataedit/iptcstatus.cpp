/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : IPTC status settings page.
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

#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kactivelabel.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "metadatacheckbox.h"
#include "pluginsversion.h"
#include "iptcstatus.h"
#include "iptcstatus.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCStatusPriv
{
public:

    IPTCStatusPriv()
    {
        priorityCB          = 0;
        objectCycleCB       = 0;
        objectTypeCB        = 0;
        statusEdit          = 0;
        JobIDEdit           = 0;
        priorityCheck       = 0;
        objectCycleCheck    = 0;
        objectTypeCheck     = 0;
        statusCheck         = 0;
        JobIDCheck          = 0;
    }

    QCheckBox        *statusCheck;
    QCheckBox        *JobIDCheck;

    QComboBox        *priorityCB;
    QComboBox        *objectCycleCB;
    QComboBox        *objectTypeCB;
    QComboBox        *objectAttributeCB;

    KLineEdit        *statusEdit;
    KLineEdit        *objectTypeDescEdit;
    KLineEdit        *objectAttributeDescEdit;
    KLineEdit        *JobIDEdit;

    MetadataCheckBox *priorityCheck;
    MetadataCheckBox *objectCycleCheck;
    MetadataCheckBox *objectTypeCheck;
    MetadataCheckBox *objectAttributeCheck;
};

IPTCStatus::IPTCStatus(QWidget* parent)
          : QWidget(parent)
{
    d = new IPTCStatusPriv;

    QGridLayout* grid = new QGridLayout(parent, 11, 2, KDialog::spacingHint());

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->statusCheck = new QCheckBox(i18n("Edit Status:"), parent);
    d->statusEdit  = new KLineEdit(parent);
    d->statusEdit->setValidator(asciiValidator);
    d->statusEdit->setMaxLength(64);
    grid->addMultiCellWidget(d->statusCheck, 0, 0, 0, 2);
    grid->addMultiCellWidget(d->statusEdit, 1, 1, 0, 2);
    QWhatsThis::add(d->statusEdit, i18n("<p>Set here the content status. This field is limited "
                                        "to 64 ASCII characters."));

    // --------------------------------------------------------

    d->priorityCheck = new MetadataCheckBox(i18n("Priority:"), parent);
    d->priorityCB    = new QComboBox(false, parent);
    d->priorityCB->insertItem(i18n("0: None"), 0);
    d->priorityCB->insertItem(i18n("1: High"), 1);
    d->priorityCB->insertItem("2", 2);
    d->priorityCB->insertItem("3", 3);
    d->priorityCB->insertItem("4", 4);
    d->priorityCB->insertItem(i18n("5: Normal"), 5);
    d->priorityCB->insertItem("6", 6);
    d->priorityCB->insertItem("7", 7);
    d->priorityCB->insertItem(i18n("8: Low"), 8);
    grid->addMultiCellWidget(d->priorityCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->priorityCB, 2, 2, 1, 1);
    QWhatsThis::add(d->priorityCB, i18n("<p>Select here the editorial urgency of content."));

    // --------------------------------------------------------

    d->objectCycleCheck = new MetadataCheckBox(i18n("Object Cycle:"), parent);
    d->objectCycleCB    = new QComboBox(false, parent);
    d->objectCycleCB->insertItem(i18n("Morning"),   0);
    d->objectCycleCB->insertItem(i18n("Afternoon"), 1);
    d->objectCycleCB->insertItem(i18n("Evening"),   2);
    grid->addMultiCellWidget(d->objectCycleCheck, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->objectCycleCB, 3, 3, 1, 1);
    QWhatsThis::add(d->objectCycleCB, i18n("<p>Select here the editorial cycle of content."));
      
    // --------------------------------------------------------

    d->objectTypeCheck    = new MetadataCheckBox(i18n("Object Type:"), parent);
    d->objectTypeCB       = new QComboBox(false, parent);
    d->objectTypeDescEdit = new KLineEdit(parent);
    d->objectTypeDescEdit->setValidator(asciiValidator);
    d->objectTypeDescEdit->setMaxLength(64);
    d->objectTypeCB->insertItem(i18n("News"),     0);
    d->objectTypeCB->insertItem(i18n("Data"),     1);
    d->objectTypeCB->insertItem(i18n("Advisory"), 2);
    grid->addMultiCellWidget(d->objectTypeCheck, 4, 4, 0, 0);
    grid->addMultiCellWidget(d->objectTypeCB, 4, 4, 1, 1);
    grid->addMultiCellWidget(d->objectTypeDescEdit, 5, 5, 0, 2);
    QWhatsThis::add(d->objectTypeCB, i18n("<p>Select here the editorial type of content."));
    QWhatsThis::add(d->objectTypeDescEdit, i18n("<p>Set here the editorial type description of content. "
                                                "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->objectAttributeCheck    = new MetadataCheckBox(i18n("Object Attribute:"), parent);
    d->objectAttributeCB       = new QComboBox(false, parent);
    d->objectAttributeDescEdit = new KLineEdit(parent);
    d->objectAttributeDescEdit->setValidator(asciiValidator);
    d->objectAttributeDescEdit->setMaxLength(64);
    d->objectAttributeCB->insertItem(i18n("Current"),                           0);
    d->objectAttributeCB->insertItem(i18n("Analysis"),                          1);
    d->objectAttributeCB->insertItem(i18n("Archive material"),                  2);
    d->objectAttributeCB->insertItem(i18n("Background"),                        3);
    d->objectAttributeCB->insertItem(i18n("Feature"),                           4);
    d->objectAttributeCB->insertItem(i18n("Forecast"),                          5);
    d->objectAttributeCB->insertItem(i18n("History"),                           6);
    d->objectAttributeCB->insertItem(i18n("Obituary"),                          7);
    d->objectAttributeCB->insertItem(i18n("Opinion"),                           8);
    d->objectAttributeCB->insertItem(i18n("Polls & Surveys"),                   9);
    d->objectAttributeCB->insertItem(i18n("Profile"),                           10);
    d->objectAttributeCB->insertItem(i18n("Results Listings & Table"),          11);
    d->objectAttributeCB->insertItem(i18n("Side bar & Supporting information"), 12);
    d->objectAttributeCB->insertItem(i18n("Summary"),                           13);
    d->objectAttributeCB->insertItem(i18n("Transcript & Verbatim"),             14);
    d->objectAttributeCB->insertItem(i18n("Interview"),                         15);
    d->objectAttributeCB->insertItem(i18n("From the Scene"),                    16);
    d->objectAttributeCB->insertItem(i18n("Retrospective"),                     17);
    d->objectAttributeCB->insertItem(i18n("Statistics"),                        18);
    d->objectAttributeCB->insertItem(i18n("Update"),                            19);
    d->objectAttributeCB->insertItem(i18n("Wrap-up"),                           20);
    d->objectAttributeCB->insertItem(i18n("Press Release"),                     21);
    grid->addMultiCellWidget(d->objectAttributeCheck, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->objectAttributeCB, 6, 6, 1, 2);
    grid->addMultiCellWidget(d->objectAttributeDescEdit, 7, 7, 0, 2);
    QWhatsThis::add(d->objectAttributeCB, i18n("<p>Select here the editorial attribute of content."));
    QWhatsThis::add(d->objectAttributeDescEdit, i18n("<p>Set here the editorial attribute description of "
                                                     "content. This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->JobIDCheck = new QCheckBox(i18n("Job Identification:"), parent);
    d->JobIDEdit  = new KLineEdit(parent);
    d->JobIDEdit->setValidator(asciiValidator);
    d->JobIDEdit->setMaxLength(32);
    grid->addMultiCellWidget(d->JobIDCheck, 8, 8, 0, 2);
    grid->addMultiCellWidget(d->JobIDEdit, 9, 9, 0, 2);
    QWhatsThis::add(d->JobIDEdit, i18n("<p>Set here the string that identifies content that recurs. "
                                       "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    KActiveLabel *note = new KActiveLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b>"
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"), parent);

    grid->addMultiCellWidget(note, 10, 10, 0, 2);
    grid->setColStretch(2, 10);                     
    grid->setRowStretch(11, 10);                     

    // --------------------------------------------------------

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            d->priorityCB, SLOT(setEnabled(bool)));

    connect(d->objectCycleCheck, SIGNAL(toggled(bool)),
            d->objectCycleCB, SLOT(setEnabled(bool)));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            d->objectTypeCB, SLOT(setEnabled(bool)));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            d->objectTypeDescEdit, SLOT(setEnabled(bool)));

    connect(d->objectAttributeCheck, SIGNAL(toggled(bool)),
            d->objectAttributeCB, SLOT(setEnabled(bool)));

    connect(d->objectAttributeCheck, SIGNAL(toggled(bool)),
            d->objectAttributeDescEdit, SLOT(setEnabled(bool)));

    connect(d->statusCheck, SIGNAL(toggled(bool)),
            d->statusEdit, SLOT(setEnabled(bool)));

    connect(d->JobIDCheck, SIGNAL(toggled(bool)),
            d->JobIDEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectCycleCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectAttributeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectAttributeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->statusCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->JobIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->priorityCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectCycleCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectAttributeCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->statusEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeDescEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->objectAttributeDescEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->JobIDEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

IPTCStatus::~IPTCStatus()
{
    delete d;
}

void IPTCStatus::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QString data;
    int     val;

    d->statusEdit->clear();
    d->statusCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.EditStatus", false);    
    if (!data.isNull())
    {
        d->statusEdit->setText(data);
        d->statusCheck->setChecked(true);
    }
    d->statusEdit->setEnabled(d->statusCheck->isChecked());

    d->priorityCB->setCurrentItem(0);
    d->priorityCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Urgency", false);    
    if (!data.isNull())
    {
        val = data.toInt(); 
        if (val >= 0 && val <= 8)
        {
            d->priorityCB->setCurrentItem(val);
            d->priorityCheck->setChecked(true);
        }
        else
            d->priorityCheck->setValid(false);
    }
    d->priorityCB->setEnabled(d->priorityCheck->isChecked());

    d->objectCycleCB->setCurrentItem(0);
    d->objectCycleCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.ObjectCycle", false);    
    if (!data.isNull())
    {
        if (data == QString("a"))
        {
            d->objectCycleCB->setCurrentItem(0);
            d->objectCycleCheck->setChecked(true);
        }
        else if (data == QString("b"))
        {
            d->objectCycleCB->setCurrentItem(1);
            d->objectCycleCheck->setChecked(true);
        }
        else if (data == QString("c"))
        {
            d->objectCycleCB->setCurrentItem(2);
            d->objectCycleCheck->setChecked(true);
        }
        else 
            d->objectCycleCheck->setValid(false);
    }
    d->objectCycleCB->setEnabled(d->objectCycleCheck->isChecked());

    d->objectTypeCB->setCurrentItem(0);
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
                d->objectTypeCB->setCurrentItem(type);
                d->objectTypeDescEdit->setText(data.section(":", -1));
                d->objectTypeCheck->setChecked(true);
            }
            else
                d->objectTypeCheck->setValid(false);
        }
    }
    d->objectTypeCB->setEnabled(d->objectTypeCheck->isChecked());
    d->objectTypeDescEdit->setEnabled(d->objectTypeCheck->isChecked());

    d->objectAttributeCB->setCurrentItem(0);
    d->objectAttributeDescEdit->clear();
    d->objectAttributeCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.ObjectAttribute", false);    
    if (!data.isNull())
    {
        QString attSec = data.section(":", 0, 0);
        if (!attSec.isEmpty())
        {
            int att = attSec.toInt()-1;
            if (att >= 0 && att < 21)
            {
                d->objectAttributeCB->setCurrentItem(att);
                d->objectAttributeDescEdit->setText(data.section(":", -1));
                d->objectAttributeCheck->setChecked(true);
            }
            else 
                d->objectAttributeCheck->setValid(false);
        }
    }
    d->objectAttributeCB->setEnabled(d->objectAttributeCheck->isChecked());
    d->objectAttributeDescEdit->setEnabled(d->objectAttributeCheck->isChecked());

    d->JobIDEdit->clear();
    d->JobIDCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.FixtureId", false);    
    if (!data.isNull())
    {
        d->JobIDEdit->setText(data);
        d->JobIDCheck->setChecked(true);
    }
    d->JobIDEdit->setEnabled(d->JobIDCheck->isChecked());

    blockSignals(false);
}

void IPTCStatus::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->statusCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.EditStatus", d->statusEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.EditStatus");

    if (d->priorityCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Urgency", QString::number(d->priorityCB->currentItem()));
    else if (d->priorityCheck->isValid())
        exiv2Iface.removeIptcTag("Iptc.Application2.Urgency");

    if (d->objectCycleCheck->isChecked())
    {
        switch (d->objectCycleCB->currentItem())
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
        objectType.sprintf("%2d", d->objectTypeCB->currentItem()+1);
        objectType.append(QString(":%1").arg(d->objectTypeDescEdit->text()));
        exiv2Iface.setIptcTagString("Iptc.Application2.ObjectType", objectType);
    }
    else if (d->objectTypeCheck->isValid())
        exiv2Iface.removeIptcTag("Iptc.Application2.ObjectType");

    if (d->objectAttributeCheck->isChecked())
    {
        QString objectAttribute;
        objectAttribute.sprintf("%3d", d->objectAttributeCB->currentItem()+1);
        objectAttribute.append(QString(":%1").arg(d->objectAttributeDescEdit->text()));
        exiv2Iface.setIptcTagString("Iptc.Application2.ObjectAttribute", objectAttribute);
    }
    else if (d->objectAttributeCheck->isValid())
        exiv2Iface.removeIptcTag("Iptc.Application2.ObjectAttribute");

    if (d->JobIDCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.FixtureId", d->JobIDEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.FixtureId");

    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
