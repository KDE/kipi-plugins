/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-18
 * Description : XMP content settings page.
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "xmpcontent.h"
#include "xmpcontent.moc"

// Qt includes

#include <QCheckBox>
#include <QGroupBox>
#include <QGridLayout>

// KDE includes

#include <kaboutdata.h>
#include <kcomponentdata.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klineedit.h>
#include <klocale.h>
#include <kseparator.h>
#include <ktextedit.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "altlangstringedit.h"

namespace KIPIMetadataEditPlugin
{

class XMPContentPriv
{
public:

    XMPContentPriv()
    {
        writerCheck          = 0;
        headlineCheck        = 0;
        captionEdit          = 0;
        writerEdit           = 0;
        headlineEdit         = 0;
        syncJFIFCommentCheck = 0;
        syncHOSTCommentCheck = 0;
        syncEXIFCommentCheck = 0;
    }

    QCheckBox          *headlineCheck;
    QCheckBox          *syncJFIFCommentCheck;
    QCheckBox          *syncHOSTCommentCheck;
    QCheckBox          *syncEXIFCommentCheck;
    QCheckBox          *writerCheck;

    KLineEdit          *headlineEdit;
    KLineEdit          *writerEdit;

    AltLangStringsEdit *captionEdit;
    AltLangStringsEdit *copyrightEdit;
};

XMPContent::XMPContent(QWidget* parent)
          : QWidget(parent), d(new XMPContentPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->headlineCheck = new QCheckBox(i18n("Headline:"), this);
    d->headlineEdit  = new KLineEdit(this);
    d->headlineEdit->setClearButtonShown(true);
    d->headlineEdit->setWhatsThis(i18n("Enter here the content synopsis."));

    // --------------------------------------------------------

    d->captionEdit          = new AltLangStringsEdit(this, i18n("Caption:"),
                                                     i18n("Enter the content description."));

    QGroupBox *syncOptions  = new QGroupBox(i18n("Default Language Caption Options"), this);
    QVBoxLayout *vlay       = new QVBoxLayout(syncOptions);

    d->syncJFIFCommentCheck = new QCheckBox(i18n("Sync JFIF Comment section"), syncOptions);
    d->syncHOSTCommentCheck = new QCheckBox(i18n("Sync caption entered through %1",
                                            KGlobal::mainComponent().aboutData()->programName()),
                                            syncOptions);
    d->syncEXIFCommentCheck = new QCheckBox(i18n("Sync EXIF Comment"), syncOptions);

    vlay->setMargin(KDialog::spacingHint());
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->syncJFIFCommentCheck);
    vlay->addWidget(d->syncHOSTCommentCheck);
    vlay->addWidget(d->syncEXIFCommentCheck);

    // --------------------------------------------------------

    d->writerCheck = new QCheckBox(i18n("Caption Writer:"), this);
    d->writerEdit  = new KLineEdit(this);
    d->writerEdit->setClearButtonShown(true);
    d->writerEdit->setWhatsThis(i18n("Enter the name of the caption author."));

    d->copyrightEdit = new AltLangStringsEdit(this, i18n("Copyright:"),
                                              i18n("Enter the necessary copyright notice."));

    // --------------------------------------------------------

    grid->addWidget(d->headlineCheck,                     0, 0, 1, 1);
    grid->addWidget(d->headlineEdit,                      0, 1, 1, 2);
    grid->addWidget(new KSeparator(Qt::Horizontal, this), 1, 0, 1, 3);
    grid->addWidget(d->captionEdit,                       2, 0, 1, 3);
    grid->addWidget(syncOptions,                          3, 0, 1, 3);
    grid->addWidget(d->writerCheck,                       4, 0, 1, 1);
    grid->addWidget(d->writerEdit,                        4, 1, 1, 2);
    grid->addWidget(d->copyrightEdit,                     5, 0, 1, 3);
    grid->setRowStretch(6, 10);
    grid->setColumnStretch(2, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->captionEdit, SIGNAL(signalDefaultLanguageEnabled(bool)),
            this, SLOT(slotSyncOptionsEnabled(bool)));

    connect(d->headlineCheck, SIGNAL(toggled(bool)),
            d->headlineEdit, SLOT(setEnabled(bool)));

    connect(d->writerCheck, SIGNAL(toggled(bool)),
            d->writerEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->captionEdit, SIGNAL(signalToggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->copyrightEdit, SIGNAL(signalToggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->writerCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->headlineCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->captionEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->copyrightEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->headlineEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->writerEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

XMPContent::~XMPContent()
{
    delete d;
}

bool XMPContent::syncJFIFCommentIsChecked()
{
    return d->syncJFIFCommentCheck->isChecked();
}

bool XMPContent::syncHOSTCommentIsChecked()
{
    return d->syncHOSTCommentCheck->isChecked();
}

bool XMPContent::syncEXIFCommentIsChecked()
{
    return d->syncEXIFCommentCheck->isChecked();
}

QString XMPContent::getXMPCaption()
{
    return d->captionEdit->defaultAltLang();
}

void XMPContent::setCheckedSyncJFIFComment(bool c)
{
    d->syncJFIFCommentCheck->setChecked(c);
}

void XMPContent::setCheckedSyncHOSTComment(bool c)
{
    d->syncHOSTCommentCheck->setChecked(c);
}

void XMPContent::setCheckedSyncEXIFComment(bool c)
{
    d->syncEXIFCommentCheck->setChecked(c);
}

void XMPContent::readMetadata(QByteArray& xmpData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);

    KExiv2Iface::KExiv2::AltLangMap map;
    QString data;

    d->headlineEdit->clear();
    d->headlineCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.photoshop.Headline", false);
    if (!data.isNull())
    {
        d->headlineEdit->setText(data);
        d->headlineCheck->setChecked(true);
    }
    d->headlineEdit->setEnabled(d->headlineCheck->isChecked());

    d->captionEdit->setValid(false);
    map = exiv2Iface.getXmpTagStringListLangAlt("Xmp.dc.description", false);
    if (!map.isEmpty())
        d->captionEdit->setValues(map);

    data = exiv2Iface.getXmpTagString("Xmp.photoshop.CaptionWriter", false);
    if (!data.isNull())
    {
        d->writerEdit->setText(data);
        d->writerCheck->setChecked(true);
    }
    d->writerEdit->setEnabled(d->writerCheck->isChecked());

    d->copyrightEdit->setValid(false);
    map = exiv2Iface.getXmpTagStringListLangAlt("Xmp.dc.rights", false);
    if (!map.isEmpty())
        d->copyrightEdit->setValues(map);

    blockSignals(false);
}

void XMPContent::applyMetadata(QByteArray& exifData, QByteArray& xmpData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setXmp(xmpData);

    if (d->headlineCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.photoshop.Headline", d->headlineEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.photoshop.Headline");

    KExiv2Iface::KExiv2::AltLangMap oldAltLangMap, newAltLangMap;
    if (d->captionEdit->getValues(oldAltLangMap, newAltLangMap))
    {
        exiv2Iface.setXmpTagStringListLangAlt("Xmp.dc.description", newAltLangMap, false);

        if (syncEXIFCommentIsChecked())
            exiv2Iface.setExifComment(getXMPCaption());

        if (syncJFIFCommentIsChecked())
            exiv2Iface.setComments(getXMPCaption().toUtf8());
    }
    else if (d->captionEdit->isValid())
        exiv2Iface.removeXmpTag("Xmp.dc.description");

    if (d->writerCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.photoshop.CaptionWriter", d->writerEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.photoshop.CaptionWriter");

    if (d->copyrightEdit->getValues(oldAltLangMap, newAltLangMap))
        exiv2Iface.setXmpTagStringListLangAlt("Xmp.dc.rights", newAltLangMap, false);
    else if (d->copyrightEdit->isValid())
        exiv2Iface.removeXmpTag("Xmp.dc.rights");

    exifData = exiv2Iface.getExif();
    xmpData  = exiv2Iface.getXmp();
}

void XMPContent::slotSyncOptionsEnabled(bool defaultLangAlt)
{
    bool cond = defaultLangAlt & d->captionEdit->isValid();
    d->syncJFIFCommentCheck->setEnabled(cond);
    d->syncHOSTCommentCheck->setEnabled(cond);
    d->syncEXIFCommentCheck->setEnabled(cond);
}

}  // namespace KIPIMetadataEditPlugin
