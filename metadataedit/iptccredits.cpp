/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : IPTC credits settings page.
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
#include <qhgroupbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>

// Local includes.

#include "exiv2iface.h"
#include "iptccredits.h"
#include "iptccredits.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCCreditsPriv
{
public:

    IPTCCreditsPriv()
    {
        copyrightEdit   = 0;
        bylineEdit      = 0;
        bylineTitleEdit = 0;
        creditEdit      = 0;
        sourceEdit      = 0;
    }

    KLineEdit *copyrightEdit;
    KLineEdit *bylineEdit;
    KLineEdit *bylineTitleEdit;
    KLineEdit *creditEdit;
    KLineEdit *sourceEdit;
};

IPTCCredits::IPTCCredits(QWidget* parent, QByteArray& iptcData)
           : QWidget(parent)
{
    d = new IPTCCreditsPriv;
    QVBoxLayout *vlay = new QVBoxLayout( parent, 0, KDialog::spacingHint() );

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    QLabel *label1   = new QLabel(i18n("Copyright:"), parent);
    d->copyrightEdit = new KLineEdit(parent);
    d->copyrightEdit->setValidator(asciiValidator);
    d->copyrightEdit->setMaxLength(128);
    label1->setBuddy(d->copyrightEdit);
    vlay->addWidget(label1);
    vlay->addWidget(d->copyrightEdit);
    QWhatsThis::add(d->copyrightEdit, i18n("<p>Set here the necessary copyright notice. This field is limited "
                                           "to 128 ASCII characters."));
    
    // --------------------------------------------------------

    QLabel *label2 = new QLabel(i18n("Byline:"), parent);
    d->bylineEdit  = new KLineEdit(parent);
    d->bylineEdit->setValidator(asciiValidator);
    d->bylineEdit->setMaxLength(32);
    label2->setBuddy(d->bylineEdit);
    vlay->addWidget(label2);
    vlay->addWidget(d->bylineEdit);
    QWhatsThis::add(d->bylineEdit, i18n("<p>Set here the name of content creator. This field is limited "
                                        "to 32 ASCII characters."));
        
    // --------------------------------------------------------

    QLabel *label3     = new QLabel(i18n("Byline Title:"), parent);
    d->bylineTitleEdit = new KLineEdit(parent);
    d->bylineTitleEdit->setValidator(asciiValidator);
    d->bylineTitleEdit->setMaxLength(32);
    label3->setBuddy(d->bylineTitleEdit);
    vlay->addWidget(label3);
    vlay->addWidget(d->bylineTitleEdit);
    QWhatsThis::add(d->bylineTitleEdit, i18n("<p>Set here the title of content creator. This field is limited "
                                             "to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label4 = new QLabel(i18n("Credit:"), parent);
    d->creditEdit  = new KLineEdit(parent);
    d->creditEdit->setValidator(asciiValidator);
    d->creditEdit->setMaxLength(32);
    label4->setBuddy(d->creditEdit);
    vlay->addWidget(label4);
    vlay->addWidget(d->creditEdit);
    QWhatsThis::add(d->creditEdit, i18n("<p>Set here the content provider. "
                                        "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label5 = new QLabel(i18n("Source:"), parent);
    d->sourceEdit  = new KLineEdit(parent);
    d->sourceEdit->setValidator(asciiValidator);
    d->sourceEdit->setMaxLength(32);
    label5->setBuddy(d->sourceEdit);
    vlay->addWidget(label5);
    vlay->addWidget(d->sourceEdit);
    QWhatsThis::add(d->sourceEdit, i18n("<p>Set here the original owner of content. "
                                        "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: IPTC text tags only support printable "
                                       "ASCII characters set.</b>"), parent);
    vlay->addWidget(iptcNote);
    vlay->addStretch();
                                         
    // --------------------------------------------------------
        
    readMetadata(iptcData);
}

IPTCCredits::~IPTCCredits()
{
    delete d;
}

void IPTCCredits::readMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->copyrightEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.Copyright", false));
    d->bylineEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.Byline", false));
    d->bylineTitleEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.BylineTitle", false));
    d->creditEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.Credit", false));
    d->sourceEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.Source", false));
}

void IPTCCredits::applyMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    exiv2Iface.setIptcTagString("Iptc.Application2.Copyright", d->copyrightEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.Byline", d->bylineEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.BylineTitle", d->bylineTitleEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.Credit", d->creditEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.Source", d->sourceEdit->text());
    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

