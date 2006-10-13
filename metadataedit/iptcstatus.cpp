/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : IPTC status settings page.
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
#include <qcombobox.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>

// Local includes.

#include "exiv2iface.h"
#include "iptcstatus.h"
#include "iptcstatus.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCStatusPriv
{
public:

    IPTCStatusPriv()
    {
        priorityCB         = 0;
        objectCycleCB      = 0;
        statusEdit         = 0;
        JobIDEdit          = 0;
        programEdit        = 0;
        programVersionEdit = 0;
    }

    QComboBox *priorityCB;
    QComboBox *objectCycleCB;

    KLineEdit *statusEdit;
    KLineEdit *JobIDEdit;
    KLineEdit *programEdit;
    KLineEdit *programVersionEdit;
};

IPTCStatus::IPTCStatus(QWidget* parent, QByteArray& iptcData)
          : QWidget(parent)
{
    d = new IPTCStatusPriv;

    QGridLayout* grid = new QGridLayout(parent, 11, 2, KDialog::spacingHint());

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    QLabel *label1 = new QLabel(i18n("Edit Status:"), parent);
    d->statusEdit  = new KLineEdit(parent);
    d->statusEdit->setValidator(asciiValidator);
    d->statusEdit->setMaxLength(64);
    label1->setBuddy(d->statusEdit);
    grid->addMultiCellWidget(label1, 0, 0, 0, 2);
    grid->addMultiCellWidget(d->statusEdit, 1, 1, 0, 2);
    QWhatsThis::add(d->statusEdit, i18n("<p>Set here the content status. This field is limited "
                                        "to 64 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label2 = new QLabel(i18n("Priority:"), parent);
    d->priorityCB  = new QComboBox(false, parent);
    d->priorityCB->insertItem(i18n("0: None"), 0);
    d->priorityCB->insertItem(i18n("1: High"), 1);
    d->priorityCB->insertItem("2", 2);
    d->priorityCB->insertItem("3", 3);
    d->priorityCB->insertItem("4", 4);
    d->priorityCB->insertItem(i18n("5: Normal"), 5);
    d->priorityCB->insertItem("6", 6);
    d->priorityCB->insertItem("7", 7);
    d->priorityCB->insertItem(i18n("8: Low"), 8);
    label2->setBuddy(d->priorityCB);
    grid->addMultiCellWidget(label2, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->priorityCB, 2, 2, 1, 1);
    QWhatsThis::add(d->priorityCB, i18n("<p>Select here the editorial urgency of content."));

    // --------------------------------------------------------

    QLabel *label3   = new QLabel(i18n("Object Cycle:"), parent);
    d->objectCycleCB = new QComboBox(false, parent);
    d->objectCycleCB->insertItem(i18n("a: Morning"),   0);
    d->objectCycleCB->insertItem(i18n("b: Afternoon"), 1);
    d->objectCycleCB->insertItem(i18n("c: Evening"),   2);
    label3->setBuddy(d->objectCycleCB);
    grid->addMultiCellWidget(label3, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->objectCycleCB, 3, 3, 1, 1);
    QWhatsThis::add(d->objectCycleCB, i18n("<p>Select here the editorial cycle of content."));
      
    // --------------------------------------------------------

    QLabel *label4 = new QLabel(i18n("Job ID:"), parent);
    d->JobIDEdit   = new KLineEdit(parent);
    d->JobIDEdit->setValidator(asciiValidator);
    d->JobIDEdit->setMaxLength(32);
    label4->setBuddy(d->JobIDEdit);
    grid->addMultiCellWidget(label4, 4, 4, 0, 2);
    grid->addMultiCellWidget(d->JobIDEdit, 5, 5, 0, 2);
    QWhatsThis::add(d->JobIDEdit, i18n("<p>Set here the string that identifies content that recurs. "
                                       "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label5 = new QLabel(i18n("Program:"), parent);
    d->programEdit = new KLineEdit(parent);
    d->programEdit->setValidator(asciiValidator);
    d->programEdit->setMaxLength(32);
    label5->setBuddy(d->programEdit);
    grid->addMultiCellWidget(label5, 6, 6, 0, 2);
    grid->addMultiCellWidget(d->programEdit, 7, 7, 0, 2);
    QWhatsThis::add(d->programEdit, i18n("<p>Set here the content creation program name. "
                                         "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label6        = new QLabel(i18n("Program Version:"), parent);
    d->programVersionEdit = new KLineEdit(parent);
    d->programVersionEdit->setValidator(asciiValidator);
    d->programVersionEdit->setMaxLength(10);
    label5->setBuddy(d->programVersionEdit);
    grid->addMultiCellWidget(label6, 8, 8, 0, 2);
    grid->addMultiCellWidget(d->programVersionEdit, 9, 9, 0, 0);
    QWhatsThis::add(d->programVersionEdit, i18n("<p>Set here the content creation program version. "
                                                "This field is limited to 10 ASCII characters."));

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: IPTC text tags only support printable "
                                       "ASCII characters set.</b>"), parent);
    grid->addMultiCellWidget(iptcNote, 10, 10, 0, 2);
    grid->setColStretch(2, 10);                     
    grid->setRowStretch(11, 10);                     

    // --------------------------------------------------------
        
    readMetadata(iptcData);
}

IPTCStatus::~IPTCStatus()
{
    delete d;
}

void IPTCStatus::readMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->statusEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.EditStatus", false));
    d->JobIDEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.FixtureId", false));
    d->programEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.Program", false));
    d->programVersionEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.ProgramVersion", false));

    d->priorityCB->setCurrentItem(exiv2Iface.getIptcTagString("Iptc.Application2.Urgency", false).toInt());
    
    QString prio = exiv2Iface.getIptcTagString("Iptc.Application2.ObjectCycle", false);
    if (prio == QString("a"))
        d->objectCycleCB->setCurrentItem(0);
    else if (prio == QString("b"))
        d->objectCycleCB->setCurrentItem(1);
    else if (prio == QString("c"))
        d->objectCycleCB->setCurrentItem(2);
}

void IPTCStatus::applyMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    exiv2Iface.setIptcTagString("Iptc.Application2.EditStatus", d->statusEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.FixtureId", d->JobIDEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.Program", d->programEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.ProgramVersion", d->programVersionEdit->text());

    exiv2Iface.setIptcTagString("Iptc.Application2.Urgency", QString::number(d->priorityCB->currentItem()));

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

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

