/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-13
 * Description : IPTC origin settings page.
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
#include <qwhatsthis.h>
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>

// Local includes.

#include "exiv2iface.h"
#include "iptcorigin.h"
#include "iptcorigin.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCOriginPriv
{
public:

    IPTCOriginPriv()
    {
        objectNameEdit    = 0;
        cityEdit          = 0;
        sublocationEdit   = 0;
        provinceEdit      = 0;
        countryEdit       = 0;
        countryCodeEdit   = 0;
        locationEdit      = 0;
        locationCodeEdit  = 0;
        originalTransEdit = 0;
    }

    KLineEdit *objectNameEdit;
    KLineEdit *cityEdit;
    KLineEdit *sublocationEdit;
    KLineEdit *provinceEdit;
    KLineEdit *countryEdit;
    KLineEdit *countryCodeEdit;
    KLineEdit *locationEdit;
    KLineEdit *locationCodeEdit;
    KLineEdit *originalTransEdit;
};

IPTCOrigin::IPTCOrigin(QWidget* parent, QByteArray& iptcData)
          : QWidget(parent)
{
    d = new IPTCOriginPriv;

    QGridLayout* grid = new QGridLayout(parent, 14, 2, KDialog::spacingHint());

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    QLabel *label1    = new QLabel(i18n("Object name:"), parent);
    d->objectNameEdit = new KLineEdit(parent);
    d->objectNameEdit->setValidator(asciiValidator);
    d->objectNameEdit->setMaxLength(64);
    label1->setBuddy(d->objectNameEdit);
    grid->addMultiCellWidget(label1, 0, 0, 0, 2);
    grid->addMultiCellWidget(d->objectNameEdit, 1, 1, 0, 2);
    QWhatsThis::add(d->objectNameEdit, i18n("<p>Set here the shorthland reference of content. "
                    "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label2  = new QLabel(i18n("Location:"), parent);
    d->locationEdit = new KLineEdit(parent);
    d->locationEdit->setValidator(asciiValidator);
    d->locationEdit->setMaxLength(64);
    label2->setBuddy(d->locationEdit);
    grid->addMultiCellWidget(label2, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->locationEdit, 2, 2, 1, 2);
    QWhatsThis::add(d->locationEdit, i18n("<p>Set here the full country name referenced by the content. "
                                          "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label3      = new QLabel(i18n("Location code:"), parent);
    d->locationCodeEdit = new KLineEdit(parent);
    d->locationCodeEdit->setValidator(asciiValidator);
    d->locationCodeEdit->setMaxLength(3);
    label3->setBuddy(d->locationCodeEdit);
    grid->addMultiCellWidget(label3, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->locationCodeEdit, 3, 3, 1, 1);
    QWhatsThis::add(d->locationCodeEdit, i18n("<p>Set here the ISO country code referenced by the content. "
                                              "This field is limited to 3 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label4 = new QLabel(i18n("City:"), parent);
    d->cityEdit    = new KLineEdit(parent);
    d->cityEdit->setValidator(asciiValidator);
    d->cityEdit->setMaxLength(32);
    label4->setBuddy(d->cityEdit);
    grid->addMultiCellWidget(label4, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->cityEdit, 6, 6, 1, 2);
    QWhatsThis::add(d->cityEdit, i18n("<p>Set here the city of content origin. "
                                      "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label5     = new QLabel(i18n("Sublocation:"), parent);
    d->sublocationEdit = new KLineEdit(parent);
    d->sublocationEdit->setValidator(asciiValidator);
    d->sublocationEdit->setMaxLength(32);
    label5->setBuddy(d->sublocationEdit);
    grid->addMultiCellWidget(label5, 7, 7, 0, 0);
    grid->addMultiCellWidget(d->sublocationEdit, 7, 7, 1, 2);
    QWhatsThis::add(d->sublocationEdit, i18n("<p>Set here the content location within city. "
                                             "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label6  = new QLabel(i18n("State/Province:"), parent);
    d->provinceEdit = new KLineEdit(parent);
    d->provinceEdit->setValidator(asciiValidator);
    d->provinceEdit->setMaxLength(32);
    label6->setBuddy(d->provinceEdit);
    grid->addMultiCellWidget(label6, 8, 8, 0, 0);
    grid->addMultiCellWidget(d->provinceEdit, 8, 8, 1, 2);
    QWhatsThis::add(d->provinceEdit, i18n("<p>Set here the Province or State of content origin. "
                                          "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label7 = new QLabel(i18n("Country:"), parent);
    d->countryEdit = new KLineEdit(parent);
    d->countryEdit->setValidator(asciiValidator);
    d->countryEdit->setMaxLength(64);
    label7->setBuddy(d->countryEdit);
    grid->addMultiCellWidget(label7, 9, 9, 0, 0);
    grid->addMultiCellWidget(d->countryEdit, 9, 9, 1, 2);
    QWhatsThis::add(d->countryEdit, i18n("<p>Set here the full country name of content origin. "
                                          "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label8     = new QLabel(i18n("Country code:"), parent);
    d->countryCodeEdit = new KLineEdit(parent);
    d->countryCodeEdit->setValidator(asciiValidator);
    d->countryCodeEdit->setMaxLength(3);
    label5->setBuddy(d->countryCodeEdit);
    grid->addMultiCellWidget(label8, 10, 10, 0, 0);
    grid->addMultiCellWidget(d->countryCodeEdit, 10, 10, 1, 1);
    QWhatsThis::add(d->countryCodeEdit, i18n("<p>Set here the ISO country code of content origin. "
                                             "This field is limited to 3 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label9       = new QLabel(i18n("Original transmission reference:"), parent);
    d->originalTransEdit = new KLineEdit(parent);
    d->originalTransEdit->setValidator(asciiValidator);
    d->originalTransEdit->setMaxLength(32);
    label1->setBuddy(d->originalTransEdit);
    grid->addMultiCellWidget(label9, 11, 11, 0, 2);
    grid->addMultiCellWidget(d->originalTransEdit, 12, 12, 0, 2);
    QWhatsThis::add(d->originalTransEdit, i18n("<p>Set here the location of original content transmission. "
                    "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: IPTC text tags only support printable "
                                       "ASCII characters set.</b>"), parent);
    grid->addMultiCellWidget(iptcNote, 13, 13, 0, 2);
    grid->setColStretch(2, 10);                     
    grid->setRowStretch(14, 10);                     

    // --------------------------------------------------------
        
    readMetadata(iptcData);
}

IPTCOrigin::~IPTCOrigin()
{
    delete d;
}

void IPTCOrigin::readMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    d->objectNameEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.ObjectName", false));
    d->locationEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.LocationName", false));
    d->locationCodeEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.LocationCode", false));
    d->cityEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.City", false));
    d->sublocationEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.SubLocation", false));
    d->provinceEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.ProvinceState", false));
    d->countryEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.CountryName", false));
    d->countryCodeEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.CountryCode", false));
    d->originalTransEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.TransmissionReference", false));
}

void IPTCOrigin::applyMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    exiv2Iface.setIptcTagString("Iptc.Application2.ObjectName", d->objectNameEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.LocationName", d->locationEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.LocationCode", d->locationCodeEdit->text());    exiv2Iface.setIptcTagString("Iptc.Application2.City", d->cityEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.SubLocation", d->sublocationEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.ProvinceState", d->provinceEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.CountryName", d->countryEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.CountryCode", d->countryCodeEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.TransmissionReference", d->originalTransEdit->text());

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

