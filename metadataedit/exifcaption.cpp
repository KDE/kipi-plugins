/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : EXIF caption settings page.
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
#include <qcheckbox.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>

// Local includes.

#include "exiv2iface.h"
#include "exifcaption.h"
#include "exifcaption.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFCaptionPriv
{
public:

    EXIFCaptionPriv()
    {
        captionEdit             = 0;
        subjectEdit             = 0;
        writerEdit              = 0;
        headlineEdit            = 0;
        specialInstructionEdit  = 0;
        captionCheck            = 0;
        specialInstructionCheck = 0;
        subjectCheck            = 0;
        writerCheck             = 0;
        headlineCheck           = 0;
    }

    QCheckBox *captionCheck;
    QCheckBox *specialInstructionCheck;
    QCheckBox *subjectCheck;
    QCheckBox *writerCheck;
    QCheckBox *headlineCheck;

    KTextEdit *captionEdit;
    KTextEdit *specialInstructionEdit;

    KLineEdit *subjectEdit;
    KLineEdit *writerEdit;
    KLineEdit *headlineEdit;
};

EXIFCaption::EXIFCaption(QWidget* parent, QByteArray& exifData)
           : QWidget(parent)
{
    d = new EXIFCaptionPriv;
    QVBoxLayout *vlay = new QVBoxLayout( parent, 0, KDialog::spacingHint() );

    // EXIF only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);
  
/*  
    // --------------------------------------------------------

    d->captionCheck = new QCheckBox(i18n("Caption:"), parent);
    d->captionEdit  = new KTextEdit(parent);
    vlay->addWidget(d->captionCheck);
    vlay->addWidget(d->captionEdit);
    QWhatsThis::add(d->captionEdit, i18n("<p>Set here the content description. This field is limited "
                                         "to 2000 ASCII characters."));

    // --------------------------------------------------------

    d->subjectCheck = new QCheckBox(i18n("Subject:"), parent);
    d->subjectEdit  = new KLineEdit(parent);
    d->subjectEdit->setValidator(asciiValidator);
    d->subjectEdit->setMaxLength(236);
    vlay->addWidget(d->subjectCheck);
    vlay->addWidget(d->subjectEdit);
    QWhatsThis::add(d->subjectEdit, i18n("<p>Set here the structured definition of the subject. "
                    "This field is limited to 236 ASCII characters."));

    // --------------------------------------------------------

    d->writerCheck = new QCheckBox(i18n("Caption Writer:"), parent);
    d->writerEdit  = new KLineEdit(parent);
    d->writerEdit->setValidator(asciiValidator);
    d->writerEdit->setMaxLength(32);
    vlay->addWidget(d->writerCheck);
    vlay->addWidget(d->writerEdit);
    QWhatsThis::add(d->writerEdit, i18n("<p>Set here the person responsible for caption. This field is limited "
                                        "to 32 ASCII characters."));
        
    // --------------------------------------------------------

    d->headlineCheck = new QCheckBox(i18n("Headline:"), parent);
    d->headlineEdit  = new KLineEdit(parent);
    d->headlineEdit->setValidator(asciiValidator);
    d->headlineEdit->setMaxLength(256);
    vlay->addWidget(d->headlineCheck);
    vlay->addWidget(d->headlineEdit);
    QWhatsThis::add(d->headlineEdit, i18n("<p>Set here the content synopsis. This field is limited "
                                          "to 256 ASCII characters."));

    // --------------------------------------------------------

    d->specialInstructionCheck = new QCheckBox(i18n("Special Instructions:"), parent);
    d->specialInstructionEdit  = new KTextEdit(parent);
    vlay->addWidget(d->specialInstructionCheck);
    vlay->addWidget(d->specialInstructionEdit);
    QWhatsThis::add(d->specialInstructionEdit, i18n("<p>Set here the editorial usage instructions. "
                                                    "This field is limited to 256 ASCII characters."));

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: EXIF text tags only support printable "
                                       "ASCII characters set.</b>"), parent);
    vlay->addWidget(iptcNote);
    vlay->addStretch();

    // --------------------------------------------------------
                                     
    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->captionEdit, SLOT(setEnabled(bool)));

    connect(d->subjectCheck, SIGNAL(toggled(bool)),
            d->subjectEdit, SLOT(setEnabled(bool)));

    connect(d->writerCheck, SIGNAL(toggled(bool)),
            d->writerEdit, SLOT(setEnabled(bool)));

    connect(d->headlineCheck, SIGNAL(toggled(bool)),
            d->headlineEdit, SLOT(setEnabled(bool)));
    
    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            d->specialInstructionEdit, SLOT(setEnabled(bool)));
*/
    // --------------------------------------------------------
        
    readMetadata(exifData);
}

EXIFCaption::~EXIFCaption()
{
    delete d;
}

void EXIFCaption::readMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QString data;

/*
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Caption", false);    
    if (!data.isNull())
    {
        d->captionEdit->setText(data);
        d->captionCheck->setChecked(true);
    }
    d->captionEdit->setEnabled(d->captionCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.Subject", false);    
    if (!data.isNull())
    {
        d->subjectEdit->setText(data);
        d->subjectCheck->setChecked(true);
    }
    d->subjectEdit->setEnabled(d->subjectCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.Writer", false);    
    if (!data.isNull())
    {
        d->writerEdit->setText(data);
        d->writerCheck->setChecked(true);
    }
    d->writerEdit->setEnabled(d->writerCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.Headline", false);    
    if (!data.isNull())
    {
        d->headlineEdit->setText(data);
        d->headlineCheck->setChecked(true);
    }
    d->headlineEdit->setEnabled(d->headlineCheck->isChecked());

    data = exiv2Iface.getIptcTagString("Iptc.Application2.SpecialInstructions", false);    
    if (!data.isNull())
    {
        d->specialInstructionEdit->setText(data);
        d->specialInstructionCheck->setChecked(true);
    }
    d->specialInstructionEdit->setEnabled(d->specialInstructionCheck->isChecked());*/
}

void EXIFCaption::applyMetadata(QByteArray& exifData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setExif(exifData);

/*
    if (d->captionCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Caption", d->captionEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Caption");

    if (d->subjectCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Subject", d->subjectEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Subject");

    if (d->writerCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Writer", d->writerEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Writer");

    if (d->headlineCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Headline", d->headlineEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Headline");

    if (d->specialInstructionCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.SpecialInstructions", d->specialInstructionEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.SpecialInstructions");
*/
    exifData = exiv2Iface.getExif();
}

}  // namespace KIPIMetadataEditPlugin

