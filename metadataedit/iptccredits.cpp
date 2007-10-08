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

#include <QLayout>
#include <QLabel>
#include <QValidator>
#include <QCheckBox>
#include <QPushButton>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "multistringsedit.h"
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

    QCheckBox        *copyrightCheck;
    QCheckBox        *bylineCheck;
    QCheckBox        *bylineTitleCheck;
    QCheckBox        *creditCheck;
    QCheckBox        *sourceCheck;
    QCheckBox        *contactCheck;

    KLineEdit        *copyrightEdit;
    KLineEdit        *creditEdit;
    KLineEdit        *sourceEdit;

    MultiStringsEdit *bylineEdit;
    MultiStringsEdit *bylineTitleEdit;
    MultiStringsEdit *contactEdit;
};

IPTCCredits::IPTCCredits(QWidget* parent)
           : QWidget(parent)
{
    d = new IPTCCreditsPriv;
    QVBoxLayout *vlay = new QVBoxLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->copyrightCheck = new QCheckBox(i18n("Copyright:"), this);
    d->copyrightEdit  = new KLineEdit(this);
    d->copyrightEdit->setClearButtonShown(true);
    d->copyrightEdit->setValidator(asciiValidator);
    d->copyrightEdit->setMaxLength(128);
    d->copyrightEdit->setWhatsThis(i18n("<p>Set here the necessary copyright notice. This field is limited "
                                        "to 128 ASCII characters."));
    
    // --------------------------------------------------------

    d->bylineEdit  = new MultiStringsEdit(this, i18n("Byline:"), 
                                          i18n("<p>Set here the name of content creator."), 
                                          true, 32);

    // --------------------------------------------------------

    d->bylineTitleEdit  = new MultiStringsEdit(this, i18n("Byline Title:"), 
                                               i18n("<p>Set here the title of content creator."), 
                                               true, 32);

    // --------------------------------------------------------

    d->creditCheck = new QCheckBox(i18n("Credit:"), this);
    d->creditEdit  = new KLineEdit(this);
    d->creditEdit->setClearButtonShown(true);
    d->creditEdit->setValidator(asciiValidator);
    d->creditEdit->setMaxLength(32);
    d->creditEdit->setWhatsThis(i18n("<p>Set here the content provider. "
                                     "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->sourceCheck = new QCheckBox(i18n("Source:"), this);
    d->sourceEdit  = new KLineEdit(this);
    d->sourceEdit->setClearButtonShown(true);
    d->sourceEdit->setValidator(asciiValidator);
    d->sourceEdit->setMaxLength(32);
    d->sourceEdit->setWhatsThis(i18n("<p>Set here the original owner of content. "
                                     "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->contactEdit  = new MultiStringsEdit(this, i18n("Contact:"), 
                                           i18n("<p>Set here the person or organisation to contact."), 
                                           true, 128);

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"), this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);

    // --------------------------------------------------------

    vlay->addWidget(d->copyrightCheck);
    vlay->addWidget(d->copyrightEdit);
    vlay->addWidget(d->bylineEdit);
    vlay->addWidget(d->bylineTitleEdit);
    vlay->addWidget(d->creditCheck);
    vlay->addWidget(d->creditEdit);
    vlay->addWidget(d->sourceCheck);
    vlay->addWidget(d->sourceEdit);
    vlay->addWidget(d->contactEdit);
    vlay->addWidget(note);
    vlay->addStretch();
    vlay->setMargin(0);
    vlay->setSpacing(KDialog::spacingHint());
              
    // --------------------------------------------------------

    connect(d->copyrightCheck, SIGNAL(toggled(bool)),
            d->copyrightEdit, SLOT(setEnabled(bool)));


    connect(d->creditCheck, SIGNAL(toggled(bool)),
            d->creditEdit, SLOT(setEnabled(bool)));

    connect(d->sourceCheck, SIGNAL(toggled(bool)),
            d->sourceEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->copyrightCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->bylineEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->bylineTitleEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->creditCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->sourceCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->contactEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->copyrightEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->creditEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->sourceEdit, SIGNAL(textChanged(const QString &)),
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
    QString     data;
    QStringList list;

    d->copyrightEdit->clear();
    d->copyrightCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Copyright", false);    
    if (!data.isNull())
    {
        d->copyrightEdit->setText(data);
        d->copyrightCheck->setChecked(true);
    }
    d->copyrightEdit->setEnabled(d->copyrightCheck->isChecked());

    list = exiv2Iface.getIptcTagsStringList("Iptc.Application2.Byline", false);    
    d->bylineEdit->setValues(list);

    list = exiv2Iface.getIptcTagsStringList("Iptc.Application2.BylineTitle", false);    
    d->bylineTitleEdit->setValues(list);

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

    list = exiv2Iface.getIptcTagsStringList("Iptc.Application2.Contact", false);    
    d->contactEdit->setValues(list);

    blockSignals(false);
}

void IPTCCredits::applyMetadata(QByteArray& iptcData)
{
    QStringList oldList, newList;
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->copyrightCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Copyright", d->copyrightEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Copyright");

    if (d->bylineEdit->getValues(oldList, newList))
        exiv2Iface.setIptcTagsStringList("Iptc.Application2.Byline", 32, oldList, newList);
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Byline");

    if (d->bylineTitleEdit->getValues(oldList, newList))
        exiv2Iface.setIptcTagsStringList("Iptc.Application2.BylineTitle", 32, oldList, newList);
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

    if (d->contactEdit->getValues(oldList, newList))
        exiv2Iface.setIptcTagsStringList("Iptc.Application2.Contact", 128, oldList, newList);
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Contact");

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
