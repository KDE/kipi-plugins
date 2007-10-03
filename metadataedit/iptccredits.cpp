/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : IPTC credits settings page.
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
#include <qhgroupbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qvalidator.h>
#include <qcheckbox.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kactivelabel.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "iptccredits.h"
#include "iptccredits.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCCreditsPriv
{
public:

    IPTCCreditsPriv()
    {
        copyrightEdit    = 0;
        bylineEdit       = 0;
        bylineTitleEdit  = 0;
        creditEdit       = 0;
        sourceEdit       = 0;
        contactEdit      = 0;
        copyrightCheck   = 0;
        bylineCheck      = 0;
        bylineTitleCheck = 0;
        creditCheck      = 0;
        sourceCheck      = 0;
        contactCheck     = 0;
    }

    QCheckBox *copyrightCheck;
    QCheckBox *bylineCheck;
    QCheckBox *bylineTitleCheck;
    QCheckBox *creditCheck;
    QCheckBox *sourceCheck;
    QCheckBox *contactCheck;

    KLineEdit *copyrightEdit;
    KLineEdit *bylineEdit;
    KLineEdit *bylineTitleEdit;
    KLineEdit *creditEdit;
    KLineEdit *sourceEdit;
    KLineEdit *contactEdit;
};

IPTCCredits::IPTCCredits(QWidget* parent)
           : QWidget(parent)
{
    d = new IPTCCreditsPriv;
    QVBoxLayout *vlay = new QVBoxLayout( parent, 0, KDialog::spacingHint() );

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->copyrightCheck = new QCheckBox(i18n("Copyright:"), parent);
    d->copyrightEdit  = new KLineEdit(parent);
    d->copyrightEdit->setValidator(asciiValidator);
    d->copyrightEdit->setMaxLength(128);
    vlay->addWidget(d->copyrightCheck);
    vlay->addWidget(d->copyrightEdit);
    QWhatsThis::add(d->copyrightEdit, i18n("<p>Set here the necessary copyright notice. This field is limited "
                                           "to 128 ASCII characters."));
    
    // --------------------------------------------------------

    d->bylineCheck = new QCheckBox(i18n("Byline:"), parent);
    d->bylineEdit  = new KLineEdit(parent);
    d->bylineEdit->setValidator(asciiValidator);
    d->bylineEdit->setMaxLength(32);
    vlay->addWidget(d->bylineCheck);
    vlay->addWidget(d->bylineEdit);
    QWhatsThis::add(d->bylineEdit, i18n("<p>Set here the name of content creator. This field is limited "
                                        "to 32 ASCII characters."));
        
    // --------------------------------------------------------

    d->bylineTitleCheck = new QCheckBox(i18n("Byline Title:"), parent);
    d->bylineTitleEdit  = new KLineEdit(parent);
    d->bylineTitleEdit->setValidator(asciiValidator);
    d->bylineTitleEdit->setMaxLength(32);
    vlay->addWidget(d->bylineTitleCheck);
    vlay->addWidget(d->bylineTitleEdit);
    QWhatsThis::add(d->bylineTitleEdit, i18n("<p>Set here the title of content creator. This field is limited "
                                             "to 32 ASCII characters."));

    // --------------------------------------------------------

    d->creditCheck = new QCheckBox(i18n("Credit:"), parent);
    d->creditEdit  = new KLineEdit(parent);
    d->creditEdit->setValidator(asciiValidator);
    d->creditEdit->setMaxLength(32);
    vlay->addWidget(d->creditCheck);
    vlay->addWidget(d->creditEdit);
    QWhatsThis::add(d->creditEdit, i18n("<p>Set here the content provider. "
                                        "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->sourceCheck = new QCheckBox(i18n("Source:"), parent);
    d->sourceEdit  = new KLineEdit(parent);
    d->sourceEdit->setValidator(asciiValidator);
    d->sourceEdit->setMaxLength(32);
    vlay->addWidget(d->sourceCheck);
    vlay->addWidget(d->sourceEdit);
    QWhatsThis::add(d->sourceEdit, i18n("<p>Set here the original owner of content. "
                                        "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->contactCheck = new QCheckBox(i18n("Contact:"), parent);
    d->contactEdit  = new KLineEdit(parent);
    d->contactEdit->setValidator(asciiValidator);
    d->contactEdit->setMaxLength(128);
    vlay->addWidget(d->contactCheck);
    vlay->addWidget(d->contactEdit);
    QWhatsThis::add(d->contactEdit, i18n("<p>Set here the person or organisation to contact. "
                                         "This field is limited to 128 ASCII characters."));

    // --------------------------------------------------------

    KActiveLabel *note = new KActiveLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"), parent);

    vlay->addWidget(note);
    vlay->addStretch();
                                         
    // --------------------------------------------------------

    connect(d->copyrightCheck, SIGNAL(toggled(bool)),
            d->copyrightEdit, SLOT(setEnabled(bool)));

    connect(d->bylineCheck, SIGNAL(toggled(bool)),
            d->bylineEdit, SLOT(setEnabled(bool)));

    connect(d->bylineTitleCheck, SIGNAL(toggled(bool)),
            d->bylineTitleEdit, SLOT(setEnabled(bool)));

    connect(d->creditCheck, SIGNAL(toggled(bool)),
            d->creditEdit, SLOT(setEnabled(bool)));

    connect(d->sourceCheck, SIGNAL(toggled(bool)),
            d->sourceEdit, SLOT(setEnabled(bool)));

    connect(d->contactCheck, SIGNAL(toggled(bool)),
            d->contactEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->copyrightCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->bylineCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->bylineTitleCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->creditCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->sourceCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->contactCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->copyrightEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->bylineEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->bylineTitleEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->creditEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->sourceEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->contactEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

IPTCCredits::~IPTCCredits()
{
    delete d;
}

void IPTCCredits::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QString data;

    d->copyrightEdit->clear();
    d->copyrightCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Copyright", false);    
    if (!data.isNull())
    {
        d->copyrightEdit->setText(data);
        d->copyrightCheck->setChecked(true);
    }
    d->copyrightEdit->setEnabled(d->copyrightCheck->isChecked());

    d->bylineEdit->clear();
    d->bylineCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Byline", false);    
    if (!data.isNull())
    {
        d->bylineEdit->setText(data);
        d->bylineCheck->setChecked(true);
    }
    d->bylineEdit->setEnabled(d->bylineCheck->isChecked());

    d->bylineTitleEdit->clear();
    d->bylineTitleCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.BylineTitle", false);    
    if (!data.isNull())
    {
        d->bylineTitleEdit->setText(data);
        d->bylineTitleCheck->setChecked(true);
    }
    d->bylineTitleEdit->setEnabled(d->bylineTitleCheck->isChecked());

    d->creditEdit->clear();
    d->creditCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Credit", false);    
    if (!data.isNull())
    {
        d->creditEdit->setText(data);
        d->creditCheck->setChecked(true);
    }
    d->creditEdit->setEnabled(d->creditCheck->isChecked());

    d->sourceEdit->clear();
    d->sourceCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Source", false);    
    if (!data.isNull())
    {
        d->sourceEdit->setText(data);
        d->sourceCheck->setChecked(true);
    }
    d->sourceEdit->setEnabled(d->sourceCheck->isChecked());

    d->contactEdit->clear();
    d->contactCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Contact", false);    
    if (!data.isNull())
    {
        d->contactEdit->setText(data);
        d->contactCheck->setChecked(true);
    }
    d->contactEdit->setEnabled(d->contactCheck->isChecked());

    blockSignals(false);
}

void IPTCCredits::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->copyrightCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Copyright", d->copyrightEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Copyright");

    if (d->bylineCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Byline", d->bylineEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Byline");

    if (d->bylineTitleCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.BylineTitle", d->bylineTitleEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.BylineTitle");

    if (d->creditCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Credit", d->creditEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Credit");

    if (d->sourceCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Source", d->sourceEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Source");

    if (d->contactCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Contact", d->contactEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Contact");

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

