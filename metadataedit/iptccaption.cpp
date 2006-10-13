/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : IPTC caption settings page.
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
#include <ktextedit.h>

// Local includes.

#include "exiv2iface.h"
#include "iptccaption.h"
#include "iptccaption.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCCaptionPriv
{
public:

    IPTCCaptionPriv()
    {
        captionEdit            = 0;
        writerEdit             = 0;
        headlineEdit           = 0;
        specialInstructionEdit = 0;
    }

    KLineEdit *writerEdit;
    KLineEdit *headlineEdit;

    KTextEdit *captionEdit;
    KTextEdit *specialInstructionEdit;
};

IPTCCaption::IPTCCaption(QWidget* parent, QByteArray& iptcData)
           : QWidget(parent)
{
    d = new IPTCCaptionPriv;
    QVBoxLayout *vlay = new QVBoxLayout( parent, 0, KDialog::spacingHint() );

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);
    
    // --------------------------------------------------------

    QLabel *label1 = new QLabel(i18n("Caption:"), parent);
    d->captionEdit  = new KTextEdit(parent);
/*    d->captionEdit->setValidator(asciiValidator);
    d->captionEdit->setMaxLength(2000);*/
    label1->setBuddy(d->captionEdit);
    vlay->addWidget(label1);
    vlay->addWidget(d->captionEdit);
    QWhatsThis::add( d->captionEdit, i18n("<p>Set here the content description. This field is limited "
                                         "to 2000 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label2 = new QLabel(i18n("Caption Writer:"), parent);
    d->writerEdit  = new KLineEdit(parent);
    d->writerEdit->setValidator(asciiValidator);
    d->writerEdit->setMaxLength(32);
    label2->setBuddy(d->writerEdit);
    vlay->addWidget(label2);
    vlay->addWidget(d->writerEdit);
    QWhatsThis::add(d->writerEdit, i18n("<p>Set here the person responsible for caption. This field is limited "
                                        "to 32 ASCII characters."));
        
    // --------------------------------------------------------

    QLabel *label3  = new QLabel(i18n("Headline:"), parent);
    d->headlineEdit = new KLineEdit(parent);
    d->headlineEdit->setValidator(asciiValidator);
    d->headlineEdit->setMaxLength(256);
    label3->setBuddy(d->headlineEdit);
    vlay->addWidget(label3);
    vlay->addWidget(d->headlineEdit);
    QWhatsThis::add(d->headlineEdit, i18n("<p>Set here the content synopsis. This field is limited "
                                          "to 256 ASCII characters."));

    // --------------------------------------------------------

    QLabel *label4            = new QLabel(i18n("Special Instructions:"), parent);
    d->specialInstructionEdit = new KTextEdit(parent);
/*    d->specialInstructionEdit->setValidator(asciiValidator);
    d->specialInstructionEdit->setMaxLength(256);*/
    label4->setBuddy(d->specialInstructionEdit);
    vlay->addWidget(label4);
    vlay->addWidget(d->specialInstructionEdit);
    QWhatsThis::add(d->specialInstructionEdit, i18n("<p>Set here the editorial usage instructions. "
                                                    "This field is limited to 256 ASCII characters."));

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: IPTC text tags only support printable "
                                       "ASCII characters set.</b>"), parent);
    vlay->addWidget(iptcNote);
    vlay->addStretch();
                                         
    // --------------------------------------------------------
        
    readMetadata(iptcData);
}

IPTCCaption::~IPTCCaption()
{
    delete d;
}

void IPTCCaption::readMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->captionEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.Caption", false));
    d->writerEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.Writer", false));
    d->headlineEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.Headline", false));
    d->specialInstructionEdit->setText(exiv2Iface.getIptcTagString("Iptc.Application2.SpecialInstructions", false));
}

void IPTCCaption::applyMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    exiv2Iface.setIptcTagString("Iptc.Application2.Caption", d->captionEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.Writer", d->writerEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.Headline", d->headlineEdit->text());
    exiv2Iface.setIptcTagString("Iptc.Application2.SpecialInstructions", d->specialInstructionEdit->text());
    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

