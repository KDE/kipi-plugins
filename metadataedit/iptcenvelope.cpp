/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-10
 * Description : IPTC envelope settings page.
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

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "pluginsversion.h"
#include "iptcenvelope.h"
#include "iptcenvelope.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCEnvelopePriv
{
public:

    IPTCEnvelopePriv()
    {
        destinationCheck = 0;
        destinationEdit  = 0;
        serviceIDCheck   = 0;
        serviceIDEdit    = 0;
        productIDCheck   = 0;
        productIDEdit    = 0;
        envelopeIDCheck  = 0;
        envelopeIDEdit   = 0;
    }

    QCheckBox *destinationCheck;
    QCheckBox *serviceIDCheck;
    QCheckBox *productIDCheck;
    QCheckBox *envelopeIDCheck;

    KLineEdit *envelopeIDEdit;
    KLineEdit *serviceIDEdit;
    KLineEdit *productIDEdit;

    KTextEdit *destinationEdit;
};

IPTCEnvelope::IPTCEnvelope(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCEnvelopePriv;

    QGridLayout* grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->destinationCheck = new QCheckBox(i18n("Destination:"), this);
    d->destinationEdit  = new KTextEdit(this);
/*    d->specialInstructionEdit->setValidator(asciiValidator);
    d->specialInstructionEdit->document()->setMaxLength;*/
    d->destinationEdit->setWhatsThis(i18n("<p>Enter the envelope destination. "
                                          "This field is limited to 1024 ASCII characters."));

    // --------------------------------------------------------

    d->productIDCheck = new QCheckBox(i18n("Product ID:"), this);
    d->productIDEdit  = new KLineEdit(this);
    d->productIDEdit->setClearButtonShown(true);
    d->productIDEdit->setValidator(asciiValidator);
    d->productIDEdit->setMaxLength(32);
    d->productIDEdit->setWhatsThis(i18n("<p>Set here the product identifier. "
                                         "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->serviceIDCheck = new QCheckBox(i18n("Service ID:"), this);
    d->serviceIDEdit  = new KLineEdit(this);
    d->serviceIDEdit->setClearButtonShown(true);
    d->serviceIDEdit->setValidator(asciiValidator);
    d->serviceIDEdit->setMaxLength(10);
    d->serviceIDEdit->setWhatsThis(i18n("<p>Set here the service identifier. "
                                         "This field is limited to 10 ASCII characters."));

    // --------------------------------------------------------

    d->envelopeIDCheck = new QCheckBox(i18n("Envelope ID:"), this);
    d->envelopeIDEdit  = new KLineEdit(this);
    d->envelopeIDEdit->setClearButtonShown(true);
    d->envelopeIDEdit->setValidator(asciiValidator);
    d->envelopeIDEdit->setMaxLength(8);
    d->envelopeIDEdit->setWhatsThis(i18n("<p>Set here the envelope identifier. "
                                         "This field is limited to 8 ASCII characters."));

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"), this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->addWidget(d->destinationCheck, 0, 0, 1, 3);
    grid->addWidget(d->destinationEdit, 1, 0, 1, 3);
    grid->addWidget(d->productIDCheck, 2, 0, 1, 1);
    grid->addWidget(d->productIDEdit, 2, 1, 1, 2);
    grid->addWidget(d->serviceIDCheck, 3, 0, 1, 1);
    grid->addWidget(d->serviceIDEdit, 3, 1, 1, 1);
    grid->addWidget(d->envelopeIDCheck, 4, 0, 1, 1);
    grid->addWidget(d->envelopeIDEdit, 4, 1, 1, 1);
    grid->addWidget(note, 9, 0, 1, 3);
    grid->setColumnStretch(2, 10);                     
    grid->setRowStretch(10, 10);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->envelopeIDCheck, SIGNAL(toggled(bool)),
            d->envelopeIDEdit, SLOT(setEnabled(bool)));

    connect(d->destinationCheck, SIGNAL(toggled(bool)),
            d->destinationEdit, SLOT(setEnabled(bool)));

    connect(d->serviceIDCheck, SIGNAL(toggled(bool)),
            d->serviceIDEdit, SLOT(setEnabled(bool)));

    connect(d->productIDCheck, SIGNAL(toggled(bool)),
            d->productIDEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->envelopeIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->destinationCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->serviceIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->productIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->envelopeIDEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->destinationEdit, SIGNAL(textChanged()),
            this, SIGNAL(signalModified()));

    connect(d->serviceIDEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->productIDEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

IPTCEnvelope::~IPTCEnvelope()
{
    delete d;
}

void IPTCEnvelope::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    QString     data;
    QStringList list;

    d->destinationEdit->clear();
    d->destinationCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Envelope.Destination", false);    
    if (!data.isNull())
    {
        d->destinationEdit->setText(data);
        d->destinationCheck->setChecked(true);
    }
    d->destinationEdit->setEnabled(d->destinationCheck->isChecked());

    d->envelopeIDEdit->clear();
    d->envelopeIDCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Envelope.EnvelopeNumber", false);    
    if (!data.isNull())
    {
        d->envelopeIDEdit->setText(data);
        d->envelopeIDCheck->setChecked(true);
    }
    d->envelopeIDEdit->setEnabled(d->envelopeIDCheck->isChecked());

    d->serviceIDEdit->clear();
    d->serviceIDCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Envelope.ServiceId", false);    
    if (!data.isNull())
    {
        d->serviceIDEdit->setText(data);
        d->serviceIDCheck->setChecked(true);
    }
    d->serviceIDEdit->setEnabled(d->serviceIDCheck->isChecked());

    d->productIDEdit->clear();
    d->productIDCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Envelope.ProductId", false);    
    if (!data.isNull())
    {
        d->productIDEdit->setText(data);
        d->productIDCheck->setChecked(true);
    }
    d->productIDEdit->setEnabled(d->productIDCheck->isChecked());

    blockSignals(false);
}

void IPTCEnvelope::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->destinationCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.Destination", d->destinationEdit->toPlainText());
    else
        exiv2Iface.removeIptcTag("Iptc.Envelope.Destination");

    if (d->envelopeIDCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.EnvelopeNumber", d->envelopeIDEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Envelope.EnvelopeNumber");

    if (d->serviceIDCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.ServiceId", d->serviceIDEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Envelope.ServiceId");

    if (d->productIDCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.ProductId", d->productIDEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Envelope.ProductId");

    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
