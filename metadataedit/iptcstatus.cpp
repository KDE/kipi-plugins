/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : IPTC status settings page.
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

#include "iptcstatus.h"
#include "iptcstatus.moc"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QValidator>
#include <QGridLayout>

// KDE includes

#include <kdialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <ktextedit.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "pluginsversion.h"

namespace KIPIMetadataEditPlugin
{

class IPTCStatusPriv
{
public:

    IPTCStatusPriv()
    {
        statusEdit              = 0;
        JobIDEdit               = 0;
        statusCheck             = 0;
        JobIDCheck              = 0;
        specialInstructionEdit  = 0;
        specialInstructionCheck = 0;
        objectNameEdit          = 0;
        objectNameCheck         = 0;
    }

    QCheckBox *statusCheck;
    QCheckBox *JobIDCheck;
    QCheckBox *specialInstructionCheck;
    QCheckBox *objectNameCheck;

    KLineEdit *objectNameEdit;
    KLineEdit *statusEdit;
    KLineEdit *JobIDEdit;

    KTextEdit *specialInstructionEdit;
};

IPTCStatus::IPTCStatus(QWidget* parent)
          : QWidget(parent), d(new IPTCStatusPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->objectNameCheck = new QCheckBox(i18n("Title:"), this);
    d->objectNameEdit  = new KLineEdit(this);
    d->objectNameEdit->setClearButtonShown(true);
    d->objectNameEdit->setValidator(asciiValidator);
    d->objectNameEdit->setMaxLength(64);
    d->objectNameEdit->setWhatsThis(i18n("Set here the shorthand reference of content. "
                                         "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->statusCheck = new QCheckBox(i18n("Edit Status:"), this);
    d->statusEdit  = new KLineEdit(this);
    d->statusEdit->setClearButtonShown(true);
    d->statusEdit->setValidator(asciiValidator);
    d->statusEdit->setMaxLength(64);
    d->statusEdit->setWhatsThis(i18n("Set here the title of content status. This field is limited "
                                     "to 64 ASCII characters."));

    // --------------------------------------------------------

    d->JobIDCheck = new QCheckBox(i18n("Job Identifier:"), this);
    d->JobIDEdit  = new KLineEdit(this);
    d->JobIDEdit->setClearButtonShown(true);
    d->JobIDEdit->setValidator(asciiValidator);
    d->JobIDEdit->setMaxLength(32);
    d->JobIDEdit->setWhatsThis(i18n("Set here the string that identifies content that recurs. "
                                    "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->specialInstructionCheck = new QCheckBox(i18n("Special Instructions:"), this);
    d->specialInstructionEdit  = new KTextEdit(this);
/*    d->specialInstructionEdit->setValidator(asciiValidator);
    d->specialInstructionEdit->document()->setMaxLength;*/
    d->specialInstructionEdit->setWhatsThis(i18n("Enter the editorial usage instructions. "
                                                 "This field is limited to 256 ASCII characters."));

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters and limit string sizes. "
                 "Use contextual help for details.</b>"), this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->addWidget(d->objectNameCheck,         0, 0, 1, 1);
    grid->addWidget(d->objectNameEdit,          0, 1, 1, 2);
    grid->addWidget(d->statusCheck,             1, 0, 1, 1);
    grid->addWidget(d->statusEdit,              1, 1, 1, 2);
    grid->addWidget(d->JobIDCheck,              2, 0, 1, 1);
    grid->addWidget(d->JobIDEdit,               2, 1, 1, 2);
    grid->addWidget(d->specialInstructionCheck, 3, 0, 1, 3);
    grid->addWidget(d->specialInstructionEdit,  4, 0, 1, 3);
    grid->addWidget(note,                       9, 0, 1, 3);
    grid->setColumnStretch(2, 10);
    grid->setRowStretch(10, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->objectNameCheck, SIGNAL(toggled(bool)),
            d->objectNameEdit, SLOT(setEnabled(bool)));

    connect(d->statusCheck, SIGNAL(toggled(bool)),
            d->statusEdit, SLOT(setEnabled(bool)));

    connect(d->JobIDCheck, SIGNAL(toggled(bool)),
            d->JobIDEdit, SLOT(setEnabled(bool)));

    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            d->specialInstructionEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->objectNameCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->statusCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->JobIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->objectNameEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->statusEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->JobIDEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->specialInstructionEdit, SIGNAL(textChanged()),
            this, SIGNAL(signalModified()));
}

IPTCStatus::~IPTCStatus()
{
    delete d;
}

void IPTCStatus::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    QString     data;
    QStringList list;

    d->objectNameEdit->clear();
    d->objectNameCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.ObjectName", false);
    if (!data.isNull())
    {
        d->objectNameEdit->setText(data);
        d->objectNameCheck->setChecked(true);
    }
    d->objectNameEdit->setEnabled(d->objectNameCheck->isChecked());

    d->statusEdit->clear();
    d->statusCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.EditStatus", false);
    if (!data.isNull())
    {
        d->statusEdit->setText(data);
        d->statusCheck->setChecked(true);
    }
    d->statusEdit->setEnabled(d->statusCheck->isChecked());

    d->JobIDEdit->clear();
    d->JobIDCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.FixtureId", false);
    if (!data.isNull())
    {
        d->JobIDEdit->setText(data);
        d->JobIDCheck->setChecked(true);
    }
    d->JobIDEdit->setEnabled(d->JobIDCheck->isChecked());

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

void IPTCStatus::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->objectNameCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ObjectName", d->objectNameEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ObjectName");

    if (d->statusCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.EditStatus", d->statusEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.EditStatus");

    if (d->JobIDCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.FixtureId", d->JobIDEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.FixtureId");

    if (d->specialInstructionCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.SpecialInstructions", d->specialInstructionEdit->toPlainText());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.SpecialInstructions");

    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
