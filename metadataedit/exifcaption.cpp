/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : EXIF caption settings page.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exifcaption.h"
#include "exifcaption.moc"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QValidator>
#include <QGridLayout>
// KDE includes

#include <kaboutdata.h>
#include <kcomponentdata.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klineedit.h>
#include <klocale.h>
#include <ktextedit.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "pluginsversion.h"

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
        syncXMPCaptionCheck  = 0;
        syncIPTCCaptionCheck = 0;
    }

    QCheckBox *documentNameCheck;
    QCheckBox *imageDescCheck;
    QCheckBox *artistCheck;
    QCheckBox *copyrightCheck;
    QCheckBox *userCommentCheck;
    QCheckBox *syncJFIFCommentCheck;
    QCheckBox *syncHOSTCommentCheck;
    QCheckBox *syncXMPCaptionCheck;
    QCheckBox *syncIPTCCaptionCheck;

    KTextEdit *userCommentEdit;

    KLineEdit *documentNameEdit;
    KLineEdit *imageDescEdit;
    KLineEdit *artistEdit;
    KLineEdit *copyrightEdit;
};

EXIFCaption::EXIFCaption(QWidget* parent)
           : QWidget(parent), d(new EXIFCaptionPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // EXIF only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->documentNameCheck = new QCheckBox(i18n("Name (*):"), this);
    d->documentNameEdit  = new KLineEdit(this);
    d->documentNameEdit->setClearButtonShown(true);
    d->documentNameEdit->setValidator(asciiValidator);
    d->documentNameEdit->setWhatsThis(i18n("Enter the name of the document from which "
                                           "this image was been scanned. This field is limited "
                                           "to ASCII characters."));

    // --------------------------------------------------------

    d->imageDescCheck = new QCheckBox(i18n("Title (*):"), this);
    d->imageDescEdit  = new KLineEdit(this);
    d->imageDescEdit->setClearButtonShown(true);
    d->imageDescEdit->setValidator(asciiValidator);
    d->imageDescEdit->setWhatsThis(i18n("Enter the image title. This field is limited "
                                        "to ASCII characters."));

    // --------------------------------------------------------

    d->artistCheck = new QCheckBox(i18n("Artist (*):"), this);
    d->artistEdit  = new KLineEdit(this);
    d->artistEdit->setClearButtonShown(true);
    d->artistEdit->setValidator(asciiValidator);
    d->artistEdit->setWhatsThis(i18n("Enter the image author's name. "
                                     "This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->copyrightCheck = new QCheckBox(i18n("Copyright (*):"), this);
    d->copyrightEdit  = new KLineEdit(this);
    d->copyrightEdit->setClearButtonShown(true);
    d->copyrightEdit->setValidator(asciiValidator);
    d->copyrightEdit->setWhatsThis(i18n("Enter the copyright owner of the image. "
                                        "This field is limited to ASCII characters."));

    // --------------------------------------------------------

    d->userCommentCheck = new QCheckBox(i18n("Caption:"), this);
    d->userCommentEdit  = new KTextEdit(this);
    d->userCommentEdit->setWhatsThis(i18n("Enter the image's caption. "
                                          "This field is not limited. UTF8 encoding "
                                          "will be used to save the text."));

    d->syncHOSTCommentCheck = new QCheckBox(i18n("Sync captions entered through %1",
                                            KGlobal::mainComponent().aboutData()->programName()),
                                            this);
    d->syncJFIFCommentCheck = new QCheckBox(i18n("Sync JFIF Comment section"), this);
    d->syncXMPCaptionCheck  = new QCheckBox(i18n("Sync XMP caption"), this);
    d->syncIPTCCaptionCheck = new QCheckBox(i18n("Sync IPTC caption (warning: limited to 2000 printable "
                                                 "Ascii characters)"), this);

    if (!KExiv2Iface::KExiv2::supportXmp())
        d->syncXMPCaptionCheck->setEnabled(false);

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/EXIF'>EXIF</a></b> "
                 "text tags marked by (*) only support printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters.</b>"), this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->addWidget(d->documentNameCheck,       0, 0, 1, 1);
    grid->addWidget(d->documentNameEdit,        0, 1, 1, 2);
    grid->addWidget(d->imageDescCheck,          1, 0, 1, 1);
    grid->addWidget(d->imageDescEdit,           1, 1, 1, 2);
    grid->addWidget(d->artistCheck,             2, 0, 1, 1);
    grid->addWidget(d->artistEdit,              2, 1, 1, 2);
    grid->addWidget(d->copyrightCheck,          3, 0, 1, 1);
    grid->addWidget(d->copyrightEdit,           3, 1, 1, 2);
    grid->addWidget(d->userCommentCheck,        4, 0, 1, 3);
    grid->addWidget(d->userCommentEdit,         5, 0, 1, 3);
    grid->addWidget(d->syncHOSTCommentCheck,    6, 0, 1, 3);
    grid->addWidget(d->syncJFIFCommentCheck,    7, 0, 1, 3);
    grid->addWidget(d->syncXMPCaptionCheck,     8, 0, 1, 3);
    grid->addWidget(d->syncIPTCCaptionCheck,    9, 0, 1, 3);
    grid->addWidget(note,                      10, 0, 1, 3);
    grid->setRowStretch(11, 10);
    grid->setColumnStretch(2, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

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
            d->syncXMPCaptionCheck, SLOT(setEnabled(bool)));

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

bool EXIFCaption::syncXMPCaptionIsChecked()
{
    return d->syncXMPCaptionCheck->isChecked();
}

bool EXIFCaption::syncIPTCCaptionIsChecked()
{
    return d->syncIPTCCaptionCheck->isChecked();
}

QString EXIFCaption::getEXIFUserComments()
{
    return d->userCommentEdit->toPlainText();
}

void EXIFCaption::setCheckedSyncJFIFComment(bool c)
{
    d->syncJFIFCommentCheck->setChecked(c);
}

void EXIFCaption::setCheckedSyncHOSTComment(bool c)
{
    d->syncHOSTCommentCheck->setChecked(c);
}

void EXIFCaption::setCheckedSyncXMPCaption(bool c)
{
    d->syncXMPCaptionCheck->setChecked(c);
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
    d->syncXMPCaptionCheck->setEnabled(d->userCommentCheck->isChecked());
    d->syncIPTCCaptionCheck->setEnabled(d->userCommentCheck->isChecked());

    blockSignals(false);
}

void EXIFCaption::applyMetadata(QByteArray& exifData, QByteArray& iptcData, QByteArray& xmpData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setIptc(iptcData);
    exiv2Iface.setXmp(xmpData);

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
        exiv2Iface.setExifComment(d->userCommentEdit->toPlainText());

        if (syncJFIFCommentIsChecked())
            exiv2Iface.setComments(d->userCommentEdit->toPlainText().toUtf8());

        if (exiv2Iface.supportXmp() && syncXMPCaptionIsChecked())
        {
            exiv2Iface.setXmpTagStringLangAlt("Xmp.dc.description",
                                              d->userCommentEdit->toPlainText(),
                                              QString(), false);

            exiv2Iface.setXmpTagStringLangAlt("Xmp.exif.UserComment",
                                              d->userCommentEdit->toPlainText(),
                                              QString(), false);
        }

        if (syncIPTCCaptionIsChecked())
            exiv2Iface.setIptcTagString("Iptc.Application2.Caption", d->userCommentEdit->toPlainText());
    }
    else
        exiv2Iface.removeExifTag("Exif.Photo.UserComment");

    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    exifData = exiv2Iface.getExif();
    iptcData = exiv2Iface.getIptc();
    xmpData = exiv2Iface.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
