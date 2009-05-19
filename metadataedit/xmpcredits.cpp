/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-24
 * Description : XMP credits settings page.
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

#include "xmpcredits.h"
#include "xmpcredits.moc"

// Qt includes

#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QGridLayout>

// KDE includes

#include <kdialog.h>
#include <klineedit.h>
#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "multistringsedit.h"

namespace KIPIMetadataEditPlugin
{

class XMPCreditsPriv
{
public:

    XMPCreditsPriv()
    {
        bylineEdit       = 0;
        bylineTitleEdit  = 0;
        creditEdit       = 0;
        sourceEdit       = 0;
        emailEdit        = 0;
        urlEdit          = 0;
        phoneEdit        = 0;
        addressEdit      = 0;
        postalCodeEdit   = 0;
        cityEdit         = 0;
        countryEdit      = 0;
        bylineTitleCheck = 0;
        creditCheck      = 0;
        sourceCheck      = 0;
        emailCheck       = 0;
        urlCheck         = 0;
        phoneCheck       = 0;
        addressCheck     = 0;
        postalCodeCheck  = 0;
        cityCheck        = 0;
        countryCheck     = 0;
    }

    QCheckBox        *bylineTitleCheck;
    QCheckBox        *creditCheck;
    QCheckBox        *sourceCheck;
    QCheckBox        *contactCheck;
    QCheckBox        *emailCheck;
    QCheckBox        *urlCheck;
    QCheckBox        *phoneCheck;
    QCheckBox        *addressCheck;
    QCheckBox        *postalCodeCheck;
    QCheckBox        *cityCheck;
    QCheckBox        *countryCheck;

    KLineEdit        *bylineTitleEdit;
    KLineEdit        *creditEdit;
    KLineEdit        *sourceEdit;
    KLineEdit        *emailEdit;
    KLineEdit        *urlEdit;
    KLineEdit        *phoneEdit;
    KLineEdit        *addressEdit;
    KLineEdit        *postalCodeEdit;
    KLineEdit        *cityEdit;
    KLineEdit        *countryEdit;

    MultiStringsEdit *bylineEdit;
};

XMPCredits::XMPCredits(QWidget* parent)
          : QWidget(parent), d(new XMPCreditsPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->bylineEdit  = new MultiStringsEdit(this, i18n("Byline:"),
                                          i18n("Set here the name of content creator."));

    // --------------------------------------------------------

    d->bylineTitleCheck = new QCheckBox(i18n("Byline Title:"), this);
    d->bylineTitleEdit  = new KLineEdit(this);
    d->bylineTitleEdit->setClearButtonShown(true);
    d->bylineTitleEdit->setWhatsThis(i18n("Set here the title of content creator."));

    // --------------------------------------------------------

    QGroupBox *contactBox = new QGroupBox(i18n("Contact"), this);
    QGridLayout* grid2    = new QGridLayout(contactBox);

    d->emailCheck = new QCheckBox(i18n("E-mail:"), contactBox);
    d->emailEdit  = new KLineEdit(contactBox);
    d->emailEdit->setClearButtonShown(true);
    d->emailEdit->setWhatsThis(i18n("Set here the contact e-mail."));

    d->urlCheck = new QCheckBox(i18n("URL:"), contactBox);
    d->urlEdit  = new KLineEdit(contactBox);
    d->urlEdit->setClearButtonShown(true);
    d->urlEdit->setWhatsThis(i18n("Set here the contact URL."));

    d->phoneCheck = new QCheckBox(i18n("Phone:"), contactBox);
    d->phoneEdit  = new KLineEdit(contactBox);
    d->phoneEdit->setClearButtonShown(true);
    d->phoneEdit->setWhatsThis(i18n("Set here the contact 'phone number."));

    d->addressCheck = new QCheckBox(i18nc("Street address", "Address:"), contactBox);
    d->addressEdit  = new KLineEdit(contactBox);
    d->addressEdit->setClearButtonShown(true);
    d->addressEdit->setWhatsThis(i18n("Set here the contact address."));

    d->postalCodeCheck = new QCheckBox(i18n("Postal code:"), contactBox);
    d->postalCodeEdit  = new KLineEdit(contactBox);
    d->postalCodeEdit->setClearButtonShown(true);
    d->postalCodeEdit->setWhatsThis(i18n("Set here the contact postal code."));

    d->cityCheck = new QCheckBox(i18n("City:"), contactBox);
    d->cityEdit  = new KLineEdit(contactBox);
    d->cityEdit->setClearButtonShown(true);
    d->cityEdit->setWhatsThis(i18n("Set here the contact city."));

    d->countryCheck = new QCheckBox(i18n("Country:"), contactBox);
    d->countryEdit  = new KLineEdit(contactBox);
    d->countryEdit->setClearButtonShown(true);
    d->countryEdit->setWhatsThis(i18n("Set here the contact country."));

    grid2->addWidget(d->emailCheck,         0, 0, 1, 1);
    grid2->addWidget(d->emailEdit,          0, 1, 1, 2);
    grid2->addWidget(d->urlCheck,           1, 0, 1, 1);
    grid2->addWidget(d->urlEdit,            1, 1, 1, 2);
    grid2->addWidget(d->phoneCheck,         2, 0, 1, 1);
    grid2->addWidget(d->phoneEdit,          2, 1, 1, 2);
    grid2->addWidget(d->addressCheck,       3, 0, 1, 1);
    grid2->addWidget(d->addressEdit,        3, 1, 1, 2);
    grid2->addWidget(d->postalCodeCheck,    4, 0, 1, 1);
    grid2->addWidget(d->postalCodeEdit,     4, 1, 1, 2);
    grid2->addWidget(d->cityCheck,          5, 0, 1, 1);
    grid2->addWidget(d->cityEdit,           5, 1, 1, 2);
    grid2->addWidget(d->countryCheck,       6, 0, 1, 1);
    grid2->addWidget(d->countryEdit,        6, 1, 1, 2);
    grid2->setColumnStretch(2, 10);
    grid2->setMargin(KDialog::spacingHint());
    grid2->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    d->creditCheck = new QCheckBox(i18n("Credit:"), this);
    d->creditEdit  = new KLineEdit(this);
    d->creditEdit->setClearButtonShown(true);
    d->creditEdit->setWhatsThis(i18n("Set here the content provider."));

    // --------------------------------------------------------

    d->sourceCheck = new QCheckBox(i18n("Source:"), this);
    d->sourceEdit  = new KLineEdit(this);
    d->sourceEdit->setClearButtonShown(true);
    d->sourceEdit->setWhatsThis(i18n("Set here the original owner of content."));

    // --------------------------------------------------------

    grid->addWidget(d->bylineEdit,          0, 0, 1, 3);
    grid->addWidget(d->bylineTitleCheck,    1, 0, 1, 1);
    grid->addWidget(d->bylineTitleEdit,     1, 1, 1, 2);
    grid->addWidget(contactBox,             2, 0, 1, 3);
    grid->addWidget(d->creditCheck,         3, 0, 1, 1);
    grid->addWidget(d->creditEdit,          3, 1, 1, 2);
    grid->addWidget(d->sourceCheck,         4, 0, 1, 1);
    grid->addWidget(d->sourceEdit,          4, 1, 1, 2);
    grid->setRowStretch(5, 10);
    grid->setColumnStretch(2, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->bylineTitleCheck, SIGNAL(toggled(bool)),
            d->bylineTitleEdit, SLOT(setEnabled(bool)));

    connect(d->emailCheck, SIGNAL(toggled(bool)),
            d->emailEdit, SLOT(setEnabled(bool)));

    connect(d->urlCheck, SIGNAL(toggled(bool)),
            d->urlEdit, SLOT(setEnabled(bool)));

    connect(d->phoneCheck, SIGNAL(toggled(bool)),
            d->phoneEdit, SLOT(setEnabled(bool)));

    connect(d->addressCheck, SIGNAL(toggled(bool)),
            d->addressEdit, SLOT(setEnabled(bool)));

    connect(d->postalCodeCheck, SIGNAL(toggled(bool)),
            d->postalCodeEdit, SLOT(setEnabled(bool)));

    connect(d->cityCheck, SIGNAL(toggled(bool)),
            d->cityEdit, SLOT(setEnabled(bool)));

    connect(d->countryCheck, SIGNAL(toggled(bool)),
            d->countryEdit, SLOT(setEnabled(bool)));

    connect(d->creditCheck, SIGNAL(toggled(bool)),
            d->creditEdit, SLOT(setEnabled(bool)));

    connect(d->sourceCheck, SIGNAL(toggled(bool)),
            d->sourceEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->bylineEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->bylineTitleCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->emailCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->urlCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->phoneCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addressCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->postalCodeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->cityCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->countryCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->creditCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->sourceCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->bylineTitleEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->emailEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->urlEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->phoneEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->addressEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->postalCodeEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->cityEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->countryEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->creditEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->sourceEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

XMPCredits::~XMPCredits()
{
    delete d;
}

void XMPCredits::readMetadata(QByteArray& xmpData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);
    QString     data;
    QStringList list;

    list = exiv2Iface.getXmpTagStringSeq("Xmp.dc.creator", false);
    d->bylineEdit->setValues(list);

    d->bylineTitleEdit->clear();
    d->bylineTitleCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.photoshop.AuthorsPosition", false);
    if (!data.isNull())
    {
        d->bylineTitleEdit->setText(data);
        d->bylineTitleCheck->setChecked(true);
    }
    d->bylineTitleEdit->setEnabled(d->bylineTitleCheck->isChecked());

    d->emailEdit->clear();
    d->emailCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.iptc.CiEmailWork", false);
    if (!data.isNull())
    {
        d->emailEdit->setText(data);
        d->emailCheck->setChecked(true);
    }
    d->emailEdit->setEnabled(d->emailCheck->isChecked());

    d->urlEdit->clear();
    d->urlCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.iptc.CiUrlWork", false);
    if (!data.isNull())
    {
        d->urlEdit->setText(data);
        d->urlCheck->setChecked(true);
    }
    d->urlEdit->setEnabled(d->urlCheck->isChecked());

    d->phoneEdit->clear();
    d->phoneCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.iptc.CiTelWork", false);
    if (!data.isNull())
    {
        d->phoneEdit->setText(data);
        d->phoneCheck->setChecked(true);
    }
    d->phoneEdit->setEnabled(d->phoneCheck->isChecked());

    d->addressEdit->clear();
    d->addressCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.iptc.CiAdrExtadr", false);
    if (!data.isNull())
    {
        d->addressEdit->setText(data);
        d->addressCheck->setChecked(true);
    }
    d->addressEdit->setEnabled(d->addressCheck->isChecked());

    d->postalCodeEdit->clear();
    d->postalCodeCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.iptc.CiAdrPcode", false);
    if (!data.isNull())
    {
        d->postalCodeEdit->setText(data);
        d->postalCodeCheck->setChecked(true);
    }
    d->postalCodeEdit->setEnabled(d->postalCodeCheck->isChecked());

    d->cityEdit->clear();
    d->cityCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.iptc.CiAdrCity", false);
    if (!data.isNull())
    {
        d->cityEdit->setText(data);
        d->cityCheck->setChecked(true);
    }
    d->cityEdit->setEnabled(d->cityCheck->isChecked());

    d->countryEdit->clear();
    d->countryCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.iptc.CiAdrCtry", false);
    if (!data.isNull())
    {
        d->countryEdit->setText(data);
        d->countryCheck->setChecked(true);
    }
    d->countryEdit->setEnabled(d->countryCheck->isChecked());

    d->creditEdit->clear();
    d->creditCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.photoshop.Credit", false);
    if (!data.isNull())
    {
        d->creditEdit->setText(data);
        d->creditCheck->setChecked(true);
    }
    d->creditEdit->setEnabled(d->creditCheck->isChecked());

    d->sourceEdit->clear();
    d->sourceCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.photoshop.Source", false);
    if (data.isNull())
        data = exiv2Iface.getXmpTagString("Xmp.dc.source", false);

    if (!data.isNull())
    {
        d->sourceEdit->setText(data);
        d->sourceCheck->setChecked(true);
    }
    d->sourceEdit->setEnabled(d->sourceCheck->isChecked());

    blockSignals(false);
}

void XMPCredits::applyMetadata(QByteArray& xmpData)
{
    QStringList oldList, newList;
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);

    if (d->bylineEdit->getValues(oldList, newList))
        exiv2Iface.setXmpTagStringSeq("Xmp.dc.creator", newList);
    else
        exiv2Iface.removeXmpTag("Xmp.dc.creator");

    if (d->bylineTitleCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.photoshop.AuthorsPosition", d->bylineTitleEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.photoshop.AuthorsPosition");

    if (d->emailCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.iptc.CiEmailWork", d->emailEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.iptc.CiEmailWork");

    if (d->urlCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.iptc.CiUrlWork", d->urlEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.iptc.CiUrlWork");

    if (d->phoneCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.iptc.CiTelWork", d->phoneEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.iptc.CiTelWork");

    if (d->addressCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.iptc.CiAdrExtadr", d->addressEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.iptc.CiAdrExtadr");

    if (d->postalCodeCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.iptc.CiAdrPcode", d->postalCodeEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.iptc.CiAdrPcode");

    if (d->cityCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.iptc.CiAdrCity", d->cityEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.iptc.CiAdrCity");

    if (d->countryCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.iptc.CiAdrCtry", d->countryEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.iptc.CiAdrCtry");

    if (d->creditCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.photoshop.Credit", d->creditEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.photoshop.Credit");

    if (d->sourceCheck->isChecked())
    {
        exiv2Iface.setXmpTagString("Xmp.photoshop.Source", d->sourceEdit->text());
        exiv2Iface.setXmpTagString("Xmp.dc.source", d->sourceEdit->text());
    }
    else
    {
        exiv2Iface.removeXmpTag("Xmp.photoshop.Source");
        exiv2Iface.removeXmpTag("Xmp.dc.source");
    }

    xmpData = exiv2Iface.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
