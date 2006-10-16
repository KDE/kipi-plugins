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
#include <qcheckbox.h>

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
        objectNameEdit     = 0;
        cityEdit           = 0;
        sublocationEdit    = 0;
        provinceEdit       = 0;
        countryEdit        = 0;
        countryCodeEdit    = 0;
        locationEdit       = 0;
        locationCodeEdit   = 0;
        originalTransEdit  = 0;
        objectNameCheck    = 0;
        cityCheck          = 0;
        sublocationCheck   = 0;
        provinceCheck      = 0;
        countryCheck       = 0;
        countryCodeCheck   = 0;
        locationCheck      = 0;
        locationCodeCheck  = 0;
        originalTransCheck = 0;
    }

    QCheckBox *objectNameCheck;
    QCheckBox *cityCheck;
    QCheckBox *sublocationCheck;
    QCheckBox *provinceCheck;
    QCheckBox *countryCheck;
    QCheckBox *countryCodeCheck;
    QCheckBox *locationCheck;
    QCheckBox *locationCodeCheck;
    QCheckBox *originalTransCheck;

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

    d->objectNameCheck = new QCheckBox(i18n("Object name:"), parent);
    d->objectNameEdit  = new KLineEdit(parent);
    d->objectNameEdit->setValidator(asciiValidator);
    d->objectNameEdit->setMaxLength(64);
    grid->addMultiCellWidget(d->objectNameCheck, 0, 0, 0, 2);
    grid->addMultiCellWidget(d->objectNameEdit, 1, 1, 0, 2);
    QWhatsThis::add(d->objectNameEdit, i18n("<p>Set here the shorthand reference of content. "
                    "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->locationCheck = new QCheckBox(i18n("Location:"), parent);
    d->locationEdit  = new KLineEdit(parent);
    d->locationEdit->setValidator(asciiValidator);
    d->locationEdit->setMaxLength(64);
    grid->addMultiCellWidget(d->locationCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->locationEdit, 2, 2, 1, 2);
    QWhatsThis::add(d->locationEdit, i18n("<p>Set here the full country name referenced by the content. "
                                          "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->locationCodeCheck = new QCheckBox(i18n("Location code:"), parent);
    d->locationCodeEdit  = new KLineEdit(parent);
    d->locationCodeEdit->setValidator(asciiValidator);
    d->locationCodeEdit->setMaxLength(3);
    grid->addMultiCellWidget(d->locationCodeCheck, 3, 3, 0, 0);
    grid->addMultiCellWidget(d->locationCodeEdit, 3, 3, 1, 1);
    QWhatsThis::add(d->locationCodeEdit, i18n("<p>Set here the ISO country code referenced by the content. "
                                              "This field is limited to 3 ASCII characters."));

    // --------------------------------------------------------

    d->cityCheck = new QCheckBox(i18n("City:"), parent);
    d->cityEdit  = new KLineEdit(parent);
    d->cityEdit->setValidator(asciiValidator);
    d->cityEdit->setMaxLength(32);
    grid->addMultiCellWidget(d->cityCheck, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->cityEdit, 6, 6, 1, 2);
    QWhatsThis::add(d->cityEdit, i18n("<p>Set here the city of content origin. "
                                      "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->sublocationCheck = new QCheckBox(i18n("Sublocation:"), parent);
    d->sublocationEdit  = new KLineEdit(parent);
    d->sublocationEdit->setValidator(asciiValidator);
    d->sublocationEdit->setMaxLength(32);
    grid->addMultiCellWidget(d->sublocationCheck, 7, 7, 0, 0);
    grid->addMultiCellWidget(d->sublocationEdit, 7, 7, 1, 2);
    QWhatsThis::add(d->sublocationEdit, i18n("<p>Set here the content location within city. "
                                             "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->provinceCheck = new QCheckBox(i18n("State/Province:"), parent);
    d->provinceEdit  = new KLineEdit(parent);
    d->provinceEdit->setValidator(asciiValidator);
    d->provinceEdit->setMaxLength(32);
    grid->addMultiCellWidget(d->provinceCheck, 8, 8, 0, 0);
    grid->addMultiCellWidget(d->provinceEdit, 8, 8, 1, 2);
    QWhatsThis::add(d->provinceEdit, i18n("<p>Set here the Province or State of content origin. "
                                          "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->countryCheck = new QCheckBox(i18n("Country:"), parent);
    d->countryEdit  = new KLineEdit(parent);
    d->countryEdit->setValidator(asciiValidator);
    d->countryEdit->setMaxLength(64);
    grid->addMultiCellWidget(d->countryCheck, 9, 9, 0, 0);
    grid->addMultiCellWidget(d->countryEdit, 9, 9, 1, 2);
    QWhatsThis::add(d->countryEdit, i18n("<p>Set here the full country name of content origin. "
                                          "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->countryCodeCheck = new QCheckBox(i18n("Country code:"), parent);
    d->countryCodeEdit  = new KLineEdit(parent);
    d->countryCodeEdit->setValidator(asciiValidator);
    d->countryCodeEdit->setMaxLength(3);
    grid->addMultiCellWidget(d->countryCodeCheck, 10, 10, 0, 0);
    grid->addMultiCellWidget(d->countryCodeEdit, 10, 10, 1, 1);
    QWhatsThis::add(d->countryCodeEdit, i18n("<p>Set here the ISO country code of content origin. "
                                             "This field is limited to 3 ASCII characters."));

    // --------------------------------------------------------

    d->originalTransCheck = new QCheckBox(i18n("Original transmission reference:"), parent);
    d->originalTransEdit  = new KLineEdit(parent);
    d->originalTransEdit->setValidator(asciiValidator);
    d->originalTransEdit->setMaxLength(32);
    grid->addMultiCellWidget(d->originalTransCheck, 11, 11, 0, 2);
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

    connect(d->objectNameCheck, SIGNAL(toggled(bool)),
            d->objectNameEdit, SLOT(setEnabled(bool)));

    connect(d->cityCheck, SIGNAL(toggled(bool)),
            d->cityEdit, SLOT(setEnabled(bool)));

    connect(d->sublocationCheck, SIGNAL(toggled(bool)),
            d->sublocationEdit, SLOT(setEnabled(bool)));

    connect(d->provinceCheck, SIGNAL(toggled(bool)),
            d->provinceEdit, SLOT(setEnabled(bool)));

    connect(d->countryCheck, SIGNAL(toggled(bool)),
            d->countryEdit, SLOT(setEnabled(bool)));

    connect(d->countryCodeCheck, SIGNAL(toggled(bool)),
            d->countryCodeEdit, SLOT(setEnabled(bool)));

    connect(d->locationCheck, SIGNAL(toggled(bool)),
            d->locationEdit, SLOT(setEnabled(bool)));

    connect(d->locationCodeCheck, SIGNAL(toggled(bool)),
            d->locationCodeEdit, SLOT(setEnabled(bool)));

    connect(d->originalTransCheck, SIGNAL(toggled(bool)),
            d->originalTransEdit, SLOT(setEnabled(bool)));

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
    QString data;

    data = exiv2Iface.getIptcTagString("Iptc.Application2.ObjectName", false);    
    if (!data.isNull())
    {
        d->objectNameEdit->setText(data);
        d->objectNameCheck->setChecked(true);
    }
    d->objectNameEdit->setEnabled(d->objectNameCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.LocationName", false);    
    if (!data.isNull())
    {
        d->locationEdit->setText(data);
        d->locationCheck->setChecked(true);
    }
    d->locationEdit->setEnabled(d->locationCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.LocationCode", false);    
    if (!data.isNull())
    {
        d->locationCodeEdit->setText(data);
        d->locationCodeCheck->setChecked(true);
    }
    d->locationCodeEdit->setEnabled(d->locationCodeCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.City", false);    
    if (!data.isNull())
    {
        d->cityEdit->setText(data);
        d->cityCheck->setChecked(true);
    }
    d->cityEdit->setEnabled(d->cityCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.SubLocation", false);    
    if (!data.isNull())
    {
        d->sublocationEdit->setText(data);
        d->sublocationCheck->setChecked(true);
    }
    d->sublocationEdit->setEnabled(d->sublocationCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.ProvinceState", false);    
    if (!data.isNull())
    {
        d->provinceEdit->setText(data);
        d->provinceCheck->setChecked(true);
    }
    d->provinceEdit->setEnabled(d->provinceCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.CountryName", false);    
    if (!data.isNull())
    {
        d->countryEdit->setText(data);
        d->countryCheck->setChecked(true);
    }
    d->countryEdit->setEnabled(d->countryCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.CountryCode", false);    
    if (!data.isNull())
    {
        d->countryCodeEdit->setText(data);
        d->countryCodeCheck->setChecked(true);
    }
    d->countryCodeEdit->setEnabled(d->countryCodeCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.TransmissionReference", false);    
    if (!data.isNull())
    {
        d->originalTransEdit->setText(data);
        d->originalTransCheck->setChecked(true);
    }
    d->originalTransEdit->setEnabled(d->originalTransCheck->isChecked());
}

void IPTCOrigin::applyMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->objectNameCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ObjectName", d->objectNameEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ObjectName");

    if (d->locationCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.LocationName", d->locationEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.LocationName");

    if (d->locationCodeCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.LocationCode", d->locationCodeEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.LocationCode");

    if (d->cityCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.City", d->cityEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.City");

    if (d->sublocationCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.SubLocation", d->sublocationEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.SubLocation");

    if (d->provinceCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ProvinceState", d->provinceEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ProvinceState");

    if (d->countryCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.CountryName", d->countryEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.CountryName");

    if (d->countryCodeCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.CountryCode", d->countryCodeEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.CountryCode");

    if (d->originalTransCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.TransmissionReference", d->originalTransEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.TransmissionReference");

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

