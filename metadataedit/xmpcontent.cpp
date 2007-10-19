/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-18
 * Description : XMP content settings page.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "altlangstringedit.h"
#include "xmpcontent.h"
#include "xmpcontent.moc"

namespace KIPIMetadataEditPlugin
{

class XMPContentPriv
{
public:

    XMPContentPriv()
    {
        writerCheck             = 0;
        headlineCheck           = 0;
        captionEdit             = 0;
        writerEdit              = 0;
        headlineEdit            = 0;
        syncJFIFCommentCheck    = 0;
        syncHOSTCommentCheck    = 0;
        syncEXIFCommentCheck    = 0;
    }

    QCheckBox          *headlineCheck;
    QCheckBox          *syncJFIFCommentCheck;
    QCheckBox          *syncHOSTCommentCheck;
    QCheckBox          *syncEXIFCommentCheck;
    QCheckBox          *writerCheck;

    KLineEdit          *headlineEdit;

    AltLangStringsEdit *captionEdit;

    KLineEdit          *writerEdit;
};

XMPContent::XMPContent(QWidget* parent)
          : QWidget(parent)
{
    d = new XMPContentPriv;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->headlineCheck = new QCheckBox(i18n("Headline:"), this);
    d->headlineEdit  = new KLineEdit(this);
    d->headlineEdit->setClearButtonShown(true);
    d->headlineEdit->setWhatsThis(i18n("<p>Enter here the content synopsis."));
    
    // --------------------------------------------------------

    d->captionEdit          = new AltLangStringsEdit(this, i18n("Caption:"), 
                                                     i18n("<p>Enter the content description."));
    d->syncJFIFCommentCheck = new QCheckBox(i18n("Sync JFIF Comment section"), this);
    d->syncHOSTCommentCheck = new QCheckBox(i18n("Sync caption entered through %1",
                                              KGlobal::mainComponent().aboutData()->programName()), 
                                            this);
    d->syncEXIFCommentCheck = new QCheckBox(i18n("Sync EXIF Comment"), this);
    d->captionEdit->setWhatsThis(i18n("<p>Enter the content description."));

    // --------------------------------------------------------

    d->writerCheck = new QCheckBox(i18n("Caption Writer:"), this);
    d->writerEdit  = new KLineEdit(this);
    d->writerEdit->setClearButtonShown(true);
    d->writerEdit->setWhatsThis(i18n("<p>Enter the name of the caption author."));
        
    // --------------------------------------------------------

    grid->addWidget(d->headlineCheck, 0, 0, 1, 1);
    grid->addWidget(d->headlineEdit, 0, 1, 1, 2);
    grid->addWidget(d->captionEdit, 1, 0, 1, 3);
    grid->addWidget(d->syncJFIFCommentCheck, 2, 0, 1, 3);
    grid->addWidget(d->syncHOSTCommentCheck, 3, 0, 1, 3);
    grid->addWidget(d->syncEXIFCommentCheck, 4, 0, 1, 3);
    grid->addWidget(new KSeparator(Qt::Horizontal, this), 5, 0, 1, 3);
    grid->addWidget(d->writerCheck, 6, 0, 1, 1);
    grid->addWidget(d->writerEdit, 6, 1, 1, 2);
    grid->setRowStretch(7, 10);  
    grid->setColumnStretch(2, 10);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());    

    // --------------------------------------------------------
                                     
    connect(d->captionEdit, SIGNAL(signalToggled(bool)),
            d->captionEdit, SLOT(setEnabled(bool)));

    connect(d->captionEdit, SIGNAL(signalToggled(bool)),
            d->syncJFIFCommentCheck, SLOT(setEnabled(bool)));

    connect(d->captionEdit, SIGNAL(signalToggled(bool)),
            d->syncHOSTCommentCheck, SLOT(setEnabled(bool)));

    connect(d->captionEdit, SIGNAL(signalToggled(bool)),
            d->syncEXIFCommentCheck, SLOT(setEnabled(bool)));

    connect(d->headlineCheck, SIGNAL(toggled(bool)),
            d->headlineEdit, SLOT(setEnabled(bool)));

    connect(d->writerCheck, SIGNAL(toggled(bool)),
            d->writerEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->captionEdit, SIGNAL(signalToggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->writerCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->headlineCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));
    
    // --------------------------------------------------------

    connect(d->captionEdit, SIGNAL(textChanged()),
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
    return QString();
//TODO    return d->captionEdit->toPlainText();
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
    QString     data;
    QStringList list;

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
    list = exiv2Iface.getXmpRedondantTagsString("Xmp.dc.description", false);    
    if (!list.isEmpty())
    {
        AltLangDataList altLangList;
        for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
        {
            QString lang;
            QString text = KExiv2Iface::KExiv2::detectLanguageAlt(*it, lang);
            altLangList.append(AltLangData(lang, text));
        }
        d->captionEdit->setValues(altLangList);
    }
    d->syncJFIFCommentCheck->setEnabled(d->captionEdit->isValid());
    d->syncHOSTCommentCheck->setEnabled(d->captionEdit->isValid());
    d->syncEXIFCommentCheck->setEnabled(d->captionEdit->isValid());

    data = exiv2Iface.getXmpTagString("Xmp.photoshop.CaptionWriter", false);    
    if (!data.isNull())
    {
        d->writerEdit->setText(data);
        d->writerCheck->setChecked(true);
    }
    d->writerEdit->setEnabled(d->writerCheck->isChecked());

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

/*
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
*/

    if (d->writerCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.photoshop.CaptionWriter", d->writerEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.photoshop.CaptionWriter");

    exifData = exiv2Iface.getExif();
    xmpData = exiv2Iface.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
