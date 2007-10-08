/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : IPTC caption settings page.
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

// KDE includes.

#include <kcomponentdata.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kaboutdata.h>
#include <kseparator.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "multistringsedit.h"
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
        headlineCheck           = 0;
        syncJFIFCommentCheck    = 0;
        syncHOSTCommentCheck    = 0;
        syncEXIFCommentCheck    = 0;
    }

    QCheckBox        *captionCheck;
    QCheckBox        *specialInstructionCheck;
    QCheckBox        *headlineCheck;
    QCheckBox        *syncJFIFCommentCheck;
    QCheckBox        *syncHOSTCommentCheck;
    QCheckBox        *syncEXIFCommentCheck;

    KTextEdit        *captionEdit;
    KTextEdit        *specialInstructionEdit;

    KLineEdit        *headlineEdit;

    MultiStringsEdit *writerEdit;
};

IPTCCaption::IPTCCaption(QWidget* parent)
           : QWidget(parent)
{
    d = new IPTCCaptionPriv;
    QVBoxLayout *vlay = new QVBoxLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);
    
    // --------------------------------------------------------

    d->captionCheck         = new QCheckBox(i18n("Caption:"), this);
    d->captionEdit          = new KTextEdit(this);
    d->syncJFIFCommentCheck = new QCheckBox(i18n("Sync JFIF Comment section"), this);
    d->syncHOSTCommentCheck = new QCheckBox(i18n("Sync caption entered through %1",
                                              KGlobal::mainComponent().aboutData()->programName()), 
                                            this);
    d->syncEXIFCommentCheck = new QCheckBox(i18n("Sync EXIF Comment"), this);

/*    d->captionEdit->setValidator(asciiValidator);
    d->captionEdit->setMaxLength(2000);*/
    d->captionEdit->setWhatsThis(i18n("<p>Enter the content description. This field is limited "
                                      "to 2000 ASCII characters."));

    // --------------------------------------------------------

    d->writerEdit  = new MultiStringsEdit(this, i18n("Caption Writer:"), 
                                          i18n("<p>Enter the name of the caption author."), 
                                          true, 32);
        
    // --------------------------------------------------------

    d->headlineCheck = new QCheckBox(i18n("Headline:"), this);
    d->headlineEdit  = new KLineEdit(this);
    d->headlineEdit->setClearButtonShown(true);
    d->headlineEdit->setValidator(asciiValidator);
    d->headlineEdit->setMaxLength(256);
    d->headlineEdit->setWhatsThis(i18n("<p>Enter here the content synopsis. This field is limited "
                                       "to 256 ASCII characters."));

    // --------------------------------------------------------

    d->specialInstructionCheck = new QCheckBox(i18n("Special Instructions:"), this);
    d->specialInstructionEdit  = new KTextEdit(this);
/*    d->specialInstructionEdit->setValidator(asciiValidator);
    d->specialInstructionEdit->setMaxLength(256);*/
    d->specialInstructionEdit->setWhatsThis(i18n("<p>Enter the editorial usage instructions. "
                                                 "This field is limited to 256 ASCII characters."));

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

    vlay->addWidget(d->captionCheck);
    vlay->addWidget(d->captionEdit);
    vlay->addWidget(d->syncJFIFCommentCheck);
    vlay->addWidget(d->syncHOSTCommentCheck);
    vlay->addWidget(d->syncEXIFCommentCheck);
    vlay->addWidget(new KSeparator(Qt::Horizontal, this));
    vlay->addWidget(d->writerEdit);
    vlay->addWidget(d->headlineCheck);
    vlay->addWidget(d->headlineEdit);
    vlay->addWidget(d->specialInstructionCheck);
    vlay->addWidget(d->specialInstructionEdit);
    vlay->addWidget(note);
    vlay->addStretch();
    vlay->setMargin(0);
    vlay->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------
                                     
    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->captionEdit, SLOT(setEnabled(bool)));

    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->syncJFIFCommentCheck, SLOT(setEnabled(bool)));

    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->syncHOSTCommentCheck, SLOT(setEnabled(bool)));

    connect(d->captionCheck, SIGNAL(toggled(bool)),
            d->syncEXIFCommentCheck, SLOT(setEnabled(bool)));

    connect(d->headlineCheck, SIGNAL(toggled(bool)),
            d->headlineEdit, SLOT(setEnabled(bool)));
    
    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            d->specialInstructionEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->captionCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->writerEdit, SIGNAL(signalModified()),
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
    return d->captionEdit->toPlainText();
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
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QString     data;
    QStringList list;

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

    list = exiv2Iface.getIptcTagsStringList("Iptc.Application2.Writer", false);    
    d->writerEdit->setValues(list);

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
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setIptc(iptcData);

    if (d->captionCheck->isChecked())
    {
        exiv2Iface.setIptcTagString("Iptc.Application2.Caption", d->captionEdit->toPlainText());

        if (syncEXIFCommentIsChecked())
            exiv2Iface.setExifComment(d->captionEdit->toPlainText());

        if (syncJFIFCommentIsChecked())
            exiv2Iface.setComments(d->captionEdit->toPlainText().toUtf8());
    }
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Caption");

    QStringList oldList, newList;
    if (d->writerEdit->getValues(oldList, newList))
        exiv2Iface.setIptcTagsStringList("Iptc.Application2.Writer", 32, oldList, newList);
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Writer");

    if (d->headlineCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Headline", d->headlineEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Headline");

    if (d->specialInstructionCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.SpecialInstructions", d->specialInstructionEdit->toPlainText());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.SpecialInstructions");

    exifData = exiv2Iface.getExif();
    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
