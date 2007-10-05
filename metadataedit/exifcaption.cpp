/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : EXIF caption settings page.
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
#include <ktextedit.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kactivelabel.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "pluginsversion.h"
#include "exifcaption.h"
#include "exifcaption.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFCaptionPriv
{
public:

    EXIFCaptionPriv()
    {
        documentNameEdit     = 0;
        imageDescEdit        = 0;
        artistEdit           = 0;
        copyrightEdit        = 0;
        userCommentEdit      = 0;
        userCommentCheck     = 0;
        documentNameCheck    = 0;
        imageDescCheck       = 0;
        artistCheck          = 0;
        copyrightCheck       = 0;
        syncJFIFCommentCheck = 0;
        syncHOSTCommentCheck = 0;
        syncIPTCCaptionCheck = 0;
    }

    QCheckBox *documentNameCheck;
    QCheckBox *imageDescCheck;
    QCheckBox *artistCheck;
    QCheckBox *copyrightCheck;
    QCheckBox *userCommentCheck;
    QCheckBox *syncJFIFCommentCheck;
    QCheckBox *syncHOSTCommentCheck;
    QCheckBox *syncIPTCCaptionCheck;

    KTextEdit *userCommentEdit;

    KLineEdit *documentNameEdit;
    KLineEdit *imageDescEdit;
    KLineEdit *artistEdit;
    KLineEdit *copyrightEdit;
};

EXIFCaption::EXIFCaption(QWidget* parent)
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
    QWhatsThis::add(d->documentNameEdit, i18n("<p>Enter the name of the document from which "
                                         "this image was been scanned. This field is limited "
                                         "to ASCII characters."));

    // --------------------------------------------------------

    d->imageDescCheck = new QCheckBox(i18n("Image description (*):"), parent);
    d->imageDescEdit  = new KLineEdit(parent);
    d->imageDescEdit->setValidator(asciiValidator);
    vlay->addWidget(d->imageDescCheck);
    vlay->addWidget(d->imageDescEdit);
    QWhatsThis::add(d->imageDescEdit, i18n("<p>Enter the image title. This field is limited "
                                      "to ASCII characters."));
        
    // --------------------------------------------------------

    d->artistCheck = new QCheckBox(i18n("Artist (*):"), parent);
    d->artistEdit  = new KLineEdit(parent);
    d->artistEdit->setValidator(asciiValidator);
    vlay->addWidget(d->artistCheck);
    vlay->addWidget(d->artistEdit);
    QWhatsThis::add(d->artistEdit, i18n("<p>Enter the image author's name. "
                                   "This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->copyrightCheck = new QCheckBox(i18n("Copyright (*):"), parent);
    d->copyrightEdit  = new KLineEdit(parent);
    d->copyrightEdit->setValidator(asciiValidator);
    vlay->addWidget(d->copyrightCheck);
    vlay->addWidget(d->copyrightEdit);
    QWhatsThis::add(d->copyrightEdit, i18n("<p>Enter the copyright owner of the image. "
                                      "This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->userCommentCheck = new QCheckBox(i18n("Caption:"), parent);
    d->userCommentEdit  = new KTextEdit(parent);
    QWhatsThis::add(d->userCommentEdit, i18n("<p>Enter the image's caption. "
                                             "This field is not limited. UTF8 encoding "
                                             "will be used to save the text."));

    d->syncHOSTCommentCheck = new QCheckBox(i18n("Sync captions entered through <b>%1</b>")
                                            .arg(KApplication::kApplication()->aboutData()->appName()), 
                                            parent);
    d->syncJFIFCommentCheck = new QCheckBox(i18n("Sync JFIF Comment section"), parent);
    d->syncIPTCCaptionCheck = new QCheckBox(i18n("Sync IPTC caption (warning: limited to 2000 printable "
                                                 "Ascii characters set)"), parent);

    vlay->addWidget(d->userCommentCheck);
    vlay->addWidget(d->userCommentEdit);
    vlay->addWidget(d->syncHOSTCommentCheck);
    vlay->addWidget(d->syncJFIFCommentCheck);
    vlay->addWidget(d->syncIPTCCaptionCheck);

    // --------------------------------------------------------

    KActiveLabel *note = new KActiveLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/EXIF'>EXIF</a></b> "
                 "text tags marked by (*) only support printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set.</b>"), parent);
    vlay->addWidget(note);
    vlay->addStretch();

    // --------------------------------------------------------

    connect(d->documentNameCheck, SIGNAL(toggled(bool)),
            d->documentNameEdit, SLOT(setEnabled(bool)));

    connect(d->imageDescCheck, SIGNAL(toggled(bool)),
            d->imageDescEdit, SLOT(setEnabled(bool)));

    connect(d->artistCheck, SIGNAL(toggled(bool)),
            d->artistEdit, SLOT(setEnabled(bool)));

    connect(d->copyrightCheck, SIGNAL(toggled(bool)),
            d->copyrightEdit, SLOT(setEnabled(bool)));

    connect(d->userCommentCheck, SIGNAL(toggled(bool)),
            d->userCommentEdit, SLOT(setEnabled(bool)));

    connect(d->userCommentCheck, SIGNAL(toggled(bool)),
            d->syncJFIFCommentCheck, SLOT(setEnabled(bool)));

    connect(d->userCommentCheck, SIGNAL(toggled(bool)),
            d->syncHOSTCommentCheck, SLOT(setEnabled(bool)));

    connect(d->userCommentCheck, SIGNAL(toggled(bool)),
            d->syncIPTCCaptionCheck, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->documentNameCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));
    
    connect(d->imageDescCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->artistCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->copyrightCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->userCommentCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->userCommentEdit, SIGNAL(textChanged()),
            this, SIGNAL(signalModified()));
    
    connect(d->documentNameEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->imageDescEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->artistEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->copyrightEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

EXIFCaption::~EXIFCaption()
{
    delete d;
}

bool EXIFCaption::syncJFIFCommentIsChecked()
{
    return d->syncJFIFCommentCheck->isChecked();
}

bool EXIFCaption::syncHOSTCommentIsChecked()
{
    return d->syncHOSTCommentCheck->isChecked();
}

bool EXIFCaption::syncIPTCCaptionIsChecked()
{
    return d->syncIPTCCaptionCheck->isChecked();
}

QString EXIFCaption::getEXIFUserComments()
{
    return d->userCommentEdit->text();
}

void EXIFCaption::setCheckedSyncJFIFComment(bool c)
{
    d->syncJFIFCommentCheck->setChecked(c);
}

void EXIFCaption::setCheckedSyncHOSTComment(bool c)
{
    d->syncHOSTCommentCheck->setChecked(c);
}

void EXIFCaption::setCheckedSyncIPTCCaption(bool c)
{
    d->syncIPTCCaptionCheck->setChecked(c);
}

void EXIFCaption::readMetadata(QByteArray& exifData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    QString data;

    d->documentNameEdit->clear();
    d->documentNameCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Image.DocumentName", false);    
    if (!data.isNull())
    {
        d->documentNameEdit->setText(data);
        d->documentNameCheck->setChecked(true);
    }
    d->documentNameEdit->setEnabled(d->documentNameCheck->isChecked());

    d->imageDescEdit->clear();
    d->imageDescCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Image.ImageDescription", false);     
    if (!data.isNull())
    {
        d->imageDescEdit->setText(data);
        d->imageDescCheck->setChecked(true);
    }
    d->imageDescEdit->setEnabled(d->imageDescCheck->isChecked());

    d->artistEdit->clear();
    d->artistCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Image.Artist", false);    
    if (!data.isNull())
    {
        d->artistEdit->setText(data);
        d->artistCheck->setChecked(true);
    }
    d->artistEdit->setEnabled(d->artistCheck->isChecked());

    d->copyrightEdit->clear();
    d->copyrightCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Image.Copyright", false);    
    if (!data.isNull())
    {
        d->copyrightEdit->setText(data);
        d->copyrightCheck->setChecked(true);
    }
    d->copyrightEdit->setEnabled(d->copyrightCheck->isChecked());

    d->userCommentEdit->clear();
    d->userCommentCheck->setChecked(false);
    data = exiv2Iface.getExifComment();    
    if (!data.isNull())
    {
        d->userCommentEdit->setText(data);
        d->userCommentCheck->setChecked(true);
    }
    d->userCommentEdit->setEnabled(d->userCommentCheck->isChecked());
    d->syncJFIFCommentCheck->setEnabled(d->userCommentCheck->isChecked());
    d->syncHOSTCommentCheck->setEnabled(d->userCommentCheck->isChecked());
    d->syncIPTCCaptionCheck->setEnabled(d->userCommentCheck->isChecked());

    blockSignals(false);
}

void EXIFCaption::applyMetadata(QByteArray& exifData, QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setIptc(iptcData);

    if (d->documentNameCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Image.DocumentName", d->documentNameEdit->text());
    else
        exiv2Iface.removeExifTag("Exif.Image.DocumentName");

    if (d->imageDescCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Image.ImageDescription", d->imageDescEdit->text());
    else
        exiv2Iface.removeExifTag("Exif.Image.ImageDescription");

    if (d->artistCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Image.Artist", d->artistEdit->text());
    else
        exiv2Iface.removeExifTag("Exif.Image.Artist");

    if (d->copyrightCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Image.Copyright", d->copyrightEdit->text());
    else
        exiv2Iface.removeExifTag("Exif.Image.Copyright");

    if (d->userCommentCheck->isChecked())
    {
        exiv2Iface.setExifComment(d->userCommentEdit->text());
        
        if (syncJFIFCommentIsChecked())
            exiv2Iface.setComments(d->userCommentEdit->text().utf8());
        
        if (syncIPTCCaptionIsChecked())
            exiv2Iface.setIptcTagString("Iptc.Application2.Caption", d->userCommentEdit->text());
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.UserComment");

    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    exifData = exiv2Iface.getExif();
    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
