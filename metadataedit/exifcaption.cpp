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
        documentNameEdit  = 0;
        imageDescEdit     = 0;
        makeEdit          = 0;
        modelEdit         = 0;
        artistEdit        = 0;
        copyrightEdit     = 0;
        userCommentEdit   = 0;
        userCommentCheck  = 0;
        documentNameCheck = 0;
        imageDescCheck    = 0;
        makeCheck         = 0;
        modelCheck        = 0;
        artistCheck       = 0;
        copyrightCheck    = 0;
    }

    QCheckBox *documentNameCheck;
    QCheckBox *imageDescCheck;
    QCheckBox *makeCheck;
    QCheckBox *modelCheck;
    QCheckBox *artistCheck;
    QCheckBox *copyrightCheck;
    QCheckBox *userCommentCheck;

    KTextEdit *userCommentEdit;

    KLineEdit *documentNameEdit;
    KLineEdit *imageDescEdit;
    KLineEdit *makeEdit;
    KLineEdit *modelEdit;
    KLineEdit *artistEdit;
    KLineEdit *copyrightEdit;
};

EXIFCaption::EXIFCaption(QWidget* parent, QByteArray& exifData)
           : QWidget(parent)
{
    d = new EXIFCaptionPriv;
    QVBoxLayout *vlay = new QVBoxLayout( parent, 0, KDialog::spacingHint() );

    // EXIF only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);
  
  
    // --------------------------------------------------------

    d->documentNameCheck = new QCheckBox(i18n("Document name (*):"), parent);
    d->documentNameEdit  = new KLineEdit(parent);
    d->documentNameEdit->setValidator(asciiValidator);
    vlay->addWidget(d->documentNameCheck);
    vlay->addWidget(d->documentNameEdit);
    QWhatsThis::add(d->documentNameEdit, i18n("<p>Set here the name of the document from which "
                                         "this picture was been scanned. This field is limited "
                                         "to ASCII characters."));

    // --------------------------------------------------------

    d->imageDescCheck = new QCheckBox(i18n("Image description (*):"), parent);
    d->imageDescEdit  = new KLineEdit(parent);
    d->imageDescEdit->setValidator(asciiValidator);
    vlay->addWidget(d->imageDescCheck);
    vlay->addWidget(d->imageDescEdit);
    QWhatsThis::add(d->imageDescEdit, i18n("<p>Set here the picture title. This field is limited "
                                      "to ASCII characters."));
        
    // --------------------------------------------------------

    d->makeCheck = new QCheckBox(i18n("Make (*):"), parent);
    d->makeEdit  = new KLineEdit(parent);
    d->makeEdit->setValidator(asciiValidator);
    vlay->addWidget(d->makeCheck);
    vlay->addWidget(d->makeEdit);
    QWhatsThis::add(d->makeEdit, i18n("<p>Set here the manufacturer of image input equipment. "
                                 "This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->modelCheck = new QCheckBox(i18n("Model (*):"), parent);
    d->modelEdit  = new KLineEdit(parent);
    d->modelEdit->setValidator(asciiValidator);
    vlay->addWidget(d->modelCheck);
    vlay->addWidget(d->modelEdit);
    QWhatsThis::add(d->modelEdit, i18n("<p>Set here the model of image input equipment. "
                                  "This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->artistCheck = new QCheckBox(i18n("Artist (*):"), parent);
    d->artistEdit  = new KLineEdit(parent);
    d->artistEdit->setValidator(asciiValidator);
    vlay->addWidget(d->artistCheck);
    vlay->addWidget(d->artistEdit);
    QWhatsThis::add(d->artistEdit, i18n("<p>Set here the name of the person who created the picture. "
                                  "This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->copyrightCheck = new QCheckBox(i18n("Copyright (*):"), parent);
    d->copyrightEdit  = new KLineEdit(parent);
    d->copyrightEdit->setValidator(asciiValidator);
    vlay->addWidget(d->copyrightCheck);
    vlay->addWidget(d->copyrightEdit);
    QWhatsThis::add(d->copyrightEdit, i18n("<p>Set here the copyright holder of picture. "
                                      "This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->userCommentCheck = new QCheckBox(i18n("User comments:"), parent);
    d->userCommentEdit  = new KTextEdit(parent);
    vlay->addWidget(d->userCommentCheck);
    vlay->addWidget(d->userCommentEdit);
    QWhatsThis::add(d->userCommentEdit, i18n("<p>Set here the picture user comments. "
                                             "This field is not limited. UTF8 encoding "
                                             "will be used to save text."));

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: EXIF text tags annoted by (*) only support printable "
                                       "ASCII characters set.</b>"), parent);
    vlay->addWidget(iptcNote);
    vlay->addStretch();

    // --------------------------------------------------------

    connect(d->documentNameCheck, SIGNAL(toggled(bool)),
            d->documentNameEdit, SLOT(setEnabled(bool)));

    connect(d->imageDescCheck, SIGNAL(toggled(bool)),
            d->imageDescEdit, SLOT(setEnabled(bool)));

    connect(d->makeCheck, SIGNAL(toggled(bool)),
            d->makeEdit, SLOT(setEnabled(bool)));

    connect(d->modelCheck, SIGNAL(toggled(bool)),
            d->modelEdit, SLOT(setEnabled(bool)));
    
    connect(d->artistCheck, SIGNAL(toggled(bool)),
            d->artistEdit, SLOT(setEnabled(bool)));

    connect(d->copyrightCheck, SIGNAL(toggled(bool)),
            d->copyrightEdit, SLOT(setEnabled(bool)));

    connect(d->userCommentCheck, SIGNAL(toggled(bool)),
            d->userCommentEdit, SLOT(setEnabled(bool)));

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
    exiv2Iface.setIptc(exifData);
    QString data;

    data = exiv2Iface.getExifTagString("Exif.Image.DocumentName", false);    
    if (!data.isNull())
    {
        d->documentNameEdit->setText(data);
        d->documentNameCheck->setChecked(true);
    }
    d->documentNameEdit->setEnabled(d->documentNameCheck->isChecked());

    data = exiv2Iface.getExifTagString("Exif.Image.ImageDescription", false);     
    if (!data.isNull())
    {
        d->imageDescEdit->setText(data);
        d->imageDescCheck->setChecked(true);
    }
    d->imageDescEdit->setEnabled(d->imageDescCheck->isChecked());

    data = exiv2Iface.getExifTagString("Exif.Image.Make", false);       
    if (!data.isNull())
    {
        d->makeEdit->setText(data);
        d->makeCheck->setChecked(true);
    }
    d->makeEdit->setEnabled(d->makeCheck->isChecked());

    data = exiv2Iface.getExifTagString("Exif.Image.Model", false);     
    if (!data.isNull())
    {
        d->modelEdit->setText(data);
        d->modelCheck->setChecked(true);
    }
    d->modelEdit->setEnabled(d->modelCheck->isChecked());

    data = exiv2Iface.getExifTagString("Exif.Image.Artist", false);    
    if (!data.isNull())
    {
        d->artistEdit->setText(data);
        d->artistCheck->setChecked(true);
    }
    d->artistEdit->setEnabled(d->artistCheck->isChecked());

    data = exiv2Iface.getExifTagString("Exif.Image.Copyright", false);    
    if (!data.isNull())
    {
        d->copyrightEdit->setText(data);
        d->copyrightCheck->setChecked(true);
    }
    d->copyrightEdit->setEnabled(d->copyrightCheck->isChecked());

/* TODO
    data = exiv2Iface.getIma;    
    if (!data.isNull())
    {
        d->userCommentEdit->setText(data);
        d->userCommentCheck->setChecked(true);
    }
    d->userCommentEdit->setEnabled(d->userCommentCheck->isChecked());
*/
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

