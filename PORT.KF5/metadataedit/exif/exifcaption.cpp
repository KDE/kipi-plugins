/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-10-12
 * Description : EXIF caption settings page.
 *
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Local includes

#include "kpversion.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIMetadataEditPlugin
{

class EXIFCaption::EXIFCaptionPriv
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

    QCheckBox* documentNameCheck;
    QCheckBox* imageDescCheck;
    QCheckBox* artistCheck;
    QCheckBox* copyrightCheck;
    QCheckBox* userCommentCheck;
    QCheckBox* syncJFIFCommentCheck;
    QCheckBox* syncHOSTCommentCheck;
    QCheckBox* syncXMPCaptionCheck;
    QCheckBox* syncIPTCCaptionCheck;

    KTextEdit* userCommentEdit;

    KLineEdit* documentNameEdit;
    KLineEdit* imageDescEdit;
    KLineEdit* artistEdit;
    KLineEdit* copyrightEdit;
};

EXIFCaption::EXIFCaption(QWidget* const parent)
    : QWidget(parent), d(new EXIFCaptionPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // EXIF only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator* asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->documentNameCheck = new QCheckBox(i18nc("name of the document this image has been scanned from", "Name (*):"), this);
    d->documentNameEdit  = new KLineEdit(this);
    d->documentNameEdit->setClearButtonShown(true);
    d->documentNameEdit->setValidator(asciiValidator);
    d->documentNameEdit->setWhatsThis(i18n("Enter the name of the document from which "
                                           "this image was been scanned. This field is limited "
                                           "to ASCII characters."));

    // --------------------------------------------------------

    d->imageDescCheck = new QCheckBox(i18nc("image description", "Description (*):"), this);
    d->imageDescEdit  = new KLineEdit(this);
    d->imageDescEdit->setClearButtonShown(true);
    d->imageDescEdit->setValidator(asciiValidator);
    d->imageDescEdit->setWhatsThis(i18n("Enter the image description. This field is limited "
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

    d->userCommentCheck = new QCheckBox(i18nc("image caption", "Caption:"), this);
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

    if (!KPMetadata::supportXmp())
        d->syncXMPCaptionCheck->setEnabled(false);

    // --------------------------------------------------------

    QLabel* note = new QLabel(i18n("<b>Note: "
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

    connect(d->documentNameEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->imageDescEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->artistEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->copyrightEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));
}

EXIFCaption::~EXIFCaption()
{
    delete d;
}

bool EXIFCaption::syncJFIFCommentIsChecked() const
{
    return d->syncJFIFCommentCheck->isChecked();
}

bool EXIFCaption::syncHOSTCommentIsChecked() const
{
    return d->syncHOSTCommentCheck->isChecked();
}

bool EXIFCaption::syncXMPCaptionIsChecked() const
{
    return d->syncXMPCaptionCheck->isChecked();
}

bool EXIFCaption::syncIPTCCaptionIsChecked() const
{
    return d->syncIPTCCaptionCheck->isChecked();
}

QString EXIFCaption::getEXIFUserComments() const
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
    KPMetadata meta;
    meta.setExif(exifData);
    QString data;

    d->documentNameEdit->clear();
    d->documentNameCheck->setChecked(false);
    data = meta.getExifTagString("Exif.Image.DocumentName", false);
    if (!data.isNull())
    {
        d->documentNameEdit->setText(data);
        d->documentNameCheck->setChecked(true);
    }
    d->documentNameEdit->setEnabled(d->documentNameCheck->isChecked());

    d->imageDescEdit->clear();
    d->imageDescCheck->setChecked(false);
    data = meta.getExifTagString("Exif.Image.ImageDescription", false);
    if (!data.isNull())
    {
        d->imageDescEdit->setText(data);
        d->imageDescCheck->setChecked(true);
    }
    d->imageDescEdit->setEnabled(d->imageDescCheck->isChecked());

    d->artistEdit->clear();
    d->artistCheck->setChecked(false);
    data = meta.getExifTagString("Exif.Image.Artist", false);
    if (!data.isNull())
    {
        d->artistEdit->setText(data);
        d->artistCheck->setChecked(true);
    }
    d->artistEdit->setEnabled(d->artistCheck->isChecked());

    d->copyrightEdit->clear();
    d->copyrightCheck->setChecked(false);
    data = meta.getExifTagString("Exif.Image.Copyright", false);
    if (!data.isNull())
    {
        d->copyrightEdit->setText(data);
        d->copyrightCheck->setChecked(true);
    }
    d->copyrightEdit->setEnabled(d->copyrightCheck->isChecked());

    d->userCommentEdit->clear();
    d->userCommentCheck->setChecked(false);
    data = meta.getExifComment();
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
    KPMetadata meta;
    meta.setExif(exifData);
    meta.setIptc(iptcData);
    meta.setXmp(xmpData);

    if (d->documentNameCheck->isChecked())
        meta.setExifTagString("Exif.Image.DocumentName", d->documentNameEdit->text());
    else
        meta.removeExifTag("Exif.Image.DocumentName");

    if (d->imageDescCheck->isChecked())
        meta.setExifTagString("Exif.Image.ImageDescription", d->imageDescEdit->text());
    else
        meta.removeExifTag("Exif.Image.ImageDescription");

    if (d->artistCheck->isChecked())
        meta.setExifTagString("Exif.Image.Artist", d->artistEdit->text());
    else
        meta.removeExifTag("Exif.Image.Artist");

    if (d->copyrightCheck->isChecked())
        meta.setExifTagString("Exif.Image.Copyright", d->copyrightEdit->text());
    else
        meta.removeExifTag("Exif.Image.Copyright");

    if (d->userCommentCheck->isChecked())
    {
        meta.setExifComment(d->userCommentEdit->toPlainText());

        if (syncJFIFCommentIsChecked())
            meta.setComments(d->userCommentEdit->toPlainText().toUtf8());

        if (meta.supportXmp() && syncXMPCaptionIsChecked())
        {
            meta.setXmpTagStringLangAlt("Xmp.dc.description",
                                              d->userCommentEdit->toPlainText(),
                                              QString(), false);

            meta.setXmpTagStringLangAlt("Xmp.exif.UserComment",
                                              d->userCommentEdit->toPlainText(),
                                              QString(), false);
        }

        if (syncIPTCCaptionIsChecked())
            meta.setIptcTagString("Iptc.Application2.Caption", d->userCommentEdit->toPlainText());
    }
    else
        meta.removeExifTag("Exif.Photo.UserComment");

    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

#if KEXIV2_VERSION >= 0x010000
    exifData = meta.getExifEncoded();
#else
    exifData = meta.getExif();
#endif

    iptcData = meta.getIptc();
    xmpData  = meta.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
