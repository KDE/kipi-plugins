/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : IPTC caption settings page.
 * 
 * Copyright 2006-2007 by Gilles Caulier
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
#include <kapplication.h>
#include <kaboutdata.h>
#include <kseparator.h>

// LibKExiv2 includes. 

#include <libkexiv2/libkexiv2.h>

// Local includes.

#include "iptccaption.h"
#include "iptccaption.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCCaptionPriv
{
public:

    IPTCCaptionPriv()
    {
        captionEdit             = 0;
        writerEdit              = 0;
        headlineEdit            = 0;
        specialInstructionEdit  = 0;
        captionCheck            = 0;
        specialInstructionCheck = 0;
        writerCheck             = 0;
        headlineCheck           = 0;
        syncJFIFCommentCheck    = 0;
        syncHOSTCommentCheck    = 0;
        syncEXIFCommentCheck    = 0;
    }

    QCheckBox *captionCheck;
    QCheckBox *specialInstructionCheck;
    QCheckBox *writerCheck;
    QCheckBox *headlineCheck;
    QCheckBox *syncJFIFCommentCheck;
    QCheckBox *syncHOSTCommentCheck;
    QCheckBox *syncEXIFCommentCheck;

    KTextEdit *captionEdit;
    KTextEdit *specialInstructionEdit;

    KLineEdit *writerEdit;
    KLineEdit *headlineEdit;
};

IPTCCaption::IPTCCaption(QWidget* parent)
           : QWidget(parent)
{
    d = new IPTCCaptionPriv;
    QVBoxLayout *vlay = new QVBoxLayout( parent, 0, KDialog::spacingHint() );

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);
    
    // --------------------------------------------------------

    d->captionCheck         = new QCheckBox(i18n("Caption:"), parent);
    d->captionEdit          = new KTextEdit(parent);
    d->syncJFIFCommentCheck = new QCheckBox(i18n("Sync JFIF comment section"), parent);
    d->syncHOSTCommentCheck = new QCheckBox(i18n("Sync comment hosted by %1")
                                            .arg(KApplication::kApplication()->aboutData()->appName()), 
                                            parent);
    d->syncEXIFCommentCheck = new QCheckBox(i18n("Sync EXIF comment"), parent);
    KSeparator *line        = new KSeparator(Horizontal, parent);

/*    d->captionEdit->setValidator(asciiValidator);
    d->captionEdit->setMaxLength(2000);*/
    vlay->addWidget(d->captionCheck);
    vlay->addWidget(d->captionEdit);
    vlay->addWidget(d->syncJFIFCommentCheck);
    vlay->addWidget(d->syncHOSTCommentCheck);
    vlay->addWidget(d->syncEXIFCommentCheck);
    vlay->addWidget(line);
    QWhatsThis::add(d->captionEdit, i18n("<p>Set here the content description. This field is limited "
                                         "to 2000 ASCII characters."));

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
/*    d->specialInstructionEdit->setValidator(asciiValidator);
    d->specialInstructionEdit->setMaxLength(256);*/
    vlay->addWidget(d->specialInstructionCheck);
    vlay->addWidget(d->specialInstructionEdit);
    QWhatsThis::add(d->specialInstructionEdit, i18n("<p>Set here the editorial usage instructions. "
                                                    "This field is limited to 256 ASCII characters."));

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: IPTC text tags only support printable "
                                       "ASCII characters set.</b>"), parent);
    vlay->addWidget(iptcNote);
    vlay->addStretch();

    // --------------------------------------------------------
                                     
    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->captionEdit, SLOT(setEnabled(bool)));

    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->syncJFIFCommentCheck, SLOT(setEnabled(bool)));

    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->syncHOSTCommentCheck, SLOT(setEnabled(bool)));

    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->syncEXIFCommentCheck, SLOT(setEnabled(bool)));

    connect(d->writerCheck, SIGNAL(toggled(bool)),
            d->writerEdit, SLOT(setEnabled(bool)));

    connect(d->headlineCheck, SIGNAL(toggled(bool)),
            d->headlineEdit, SLOT(setEnabled(bool)));
    
    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            d->specialInstructionEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->captionCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->writerCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->headlineCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));
    
    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->captionEdit, SIGNAL(textChanged()),
            this, SIGNAL(signalModified()));
    
    connect(d->specialInstructionEdit, SIGNAL(textChanged()),
            this, SIGNAL(signalModified()));

    connect(d->writerEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->headlineEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

IPTCCaption::~IPTCCaption()
{
    delete d;
}

bool IPTCCaption::syncJFIFCommentIsChecked()
{
    return d->syncJFIFCommentCheck->isChecked();
}

bool IPTCCaption::syncHOSTCommentIsChecked()
{
    return d->syncHOSTCommentCheck->isChecked();
}

bool IPTCCaption::syncEXIFCommentIsChecked()
{
    return d->syncEXIFCommentCheck->isChecked();
}

QString IPTCCaption::getIPTCCaption()
{
    return d->captionEdit->text();
}

void IPTCCaption::setCheckedSyncJFIFComment(bool c)
{
    d->syncJFIFCommentCheck->setChecked(c);
}

void IPTCCaption::setCheckedSyncHOSTComment(bool c)
{
    d->syncHOSTCommentCheck->setChecked(c);
}

void IPTCCaption::setCheckedSyncEXIFComment(bool c)
{
    d->syncEXIFCommentCheck->setChecked(c);
}

void IPTCCaption::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Library::LibKExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QString data;

    d->captionEdit->clear();
    d->captionCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Caption", false);    
    if (!data.isNull())
    {
        d->captionEdit->setText(data);
        d->captionCheck->setChecked(true);
    }
    d->captionEdit->setEnabled(d->captionCheck->isChecked());
    d->syncJFIFCommentCheck->setEnabled(d->captionCheck->isChecked());
    d->syncHOSTCommentCheck->setEnabled(d->captionCheck->isChecked());
    d->syncEXIFCommentCheck->setEnabled(d->captionCheck->isChecked());

    d->writerEdit->clear();
    d->writerCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Writer", false);    
    if (!data.isNull())
    {
        d->writerEdit->setText(data);
        d->writerCheck->setChecked(true);
    }
    d->writerEdit->setEnabled(d->writerCheck->isChecked());

    d->headlineEdit->clear();
    d->headlineCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.Headline", false);    
    if (!data.isNull())
    {
        d->headlineEdit->setText(data);
        d->headlineCheck->setChecked(true);
    }
    d->headlineEdit->setEnabled(d->headlineCheck->isChecked());

    d->specialInstructionEdit->clear();
    d->specialInstructionCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.SpecialInstructions", false);    
    if (!data.isNull())
    {
        d->specialInstructionEdit->setText(data);
        d->specialInstructionCheck->setChecked(true);
    }
    d->specialInstructionEdit->setEnabled(d->specialInstructionCheck->isChecked());

    blockSignals(false);
}

void IPTCCaption::applyMetadata(QByteArray& exifData, QByteArray& iptcData)
{
    KExiv2Library::LibKExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setIptc(iptcData);

    if (d->captionCheck->isChecked())
    {
        exiv2Iface.setIptcTagString("Iptc.Application2.Caption", d->captionEdit->text());

        if (syncEXIFCommentIsChecked())
            exiv2Iface.setExifComment(d->captionEdit->text());

        if (syncJFIFCommentIsChecked())
            exiv2Iface.setComments(d->captionEdit->text().utf8());
    }
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Caption");

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

    exifData = exiv2Iface.getExif();
    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

