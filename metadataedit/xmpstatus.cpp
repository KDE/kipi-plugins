/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-24
 * Description : XMP workflow status settings page.
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
#include <QCheckBox>

// KDE includes.

#include <kiconloader.h>
#include <klocale.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kseparator.h>
#include <ktextedit.h>
#include <klineedit.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "pluginsversion.h"
#include "altlangstringedit.h"
#include "xmpstatus.h"
#include "xmpstatus.moc"

namespace KIPIMetadataEditPlugin
{

class XMPStatusPriv
{
public:

    XMPStatusPriv()
    {
        objectNameEdit          = 0;
        specialInstructionEdit  = 0;
        specialInstructionCheck = 0;
    }

    QCheckBox                      *specialInstructionCheck;

    KTextEdit                      *specialInstructionEdit;

    AltLangStringsEdit             *objectNameEdit;
};

XMPStatus::XMPStatus(QWidget* parent)
         : QWidget(parent)
{
    d = new XMPStatusPriv;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->objectNameEdit  = new AltLangStringsEdit(this, i18n("Title:"), 
                                                i18n("<p>Set here the shorthand reference of content."));

    // --------------------------------------------------------

    d->specialInstructionCheck = new QCheckBox(i18n("Special Instructions:"), this);
    d->specialInstructionEdit  = new KTextEdit(this);
    d->specialInstructionEdit->setWhatsThis(i18n("<p>Enter the editorial usage instructions."));

    // --------------------------------------------------------


    // --------------------------------------------------------

    grid->addWidget(d->objectNameEdit, 0, 0, 1, 5);
    grid->addWidget(d->specialInstructionCheck, 1, 0, 1, 5);
    grid->addWidget(d->specialInstructionEdit, 2, 0, 1, 5);
    grid->setRowStretch(3, 10);                     
    grid->setColumnStretch(3, 10);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            d->specialInstructionEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->objectNameEdit, SIGNAL(signalToggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->objectNameEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->specialInstructionEdit, SIGNAL(textChanged()),
            this, SIGNAL(signalModified()));
}

XMPStatus::~XMPStatus()
{
    delete d;
}

void XMPStatus::readMetadata(QByteArray& xmpData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);

    QString                         data;
    KExiv2Iface::KExiv2::AltLangMap map;   

    d->objectNameEdit->setValid(false);
    map = exiv2Iface.getXmpTagStringListLangAlt("Xmp.dc.title", false);
    if (!map.isEmpty())
        d->objectNameEdit->setValues(map);

    d->specialInstructionEdit->clear();
    d->specialInstructionCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.photoshop.Instructions", false);    
    if (!data.isNull())
    {
        d->specialInstructionEdit->setText(data);
        d->specialInstructionCheck->setChecked(true);
    }
    d->specialInstructionEdit->setEnabled(d->specialInstructionCheck->isChecked());

    blockSignals(false);
}

void XMPStatus::applyMetadata(QByteArray& xmpData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);

    KExiv2Iface::KExiv2::AltLangMap oldAltLangMap, newAltLangMap;
    if (d->objectNameEdit->getValues(oldAltLangMap, newAltLangMap))
        exiv2Iface.setXmpTagStringListLangAlt("Xmp.dc.title", newAltLangMap, false);
    else if (d->objectNameEdit->isValid())
        exiv2Iface.removeXmpTag("Xmp.dc.title");

    if (d->specialInstructionCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.photoshop.Instructions", d->specialInstructionEdit->toPlainText());
    else
        exiv2Iface.removeXmpTag("Xmp.photoshop.Instructions");

    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    xmpData = exiv2Iface.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
