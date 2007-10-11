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

#include <QDateTime>
#include <QTimeEdit>
#include <QLayout>
#include <QLabel>
#include <QValidator>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kdatewidget.h>
#include <kiconloader.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "pluginsversion.h"
#include "metadatacheckbox.h"
#include "iptcenvelope.h"
#include "iptcenvelope.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCEnvelopePriv
{
public:

    IPTCEnvelopePriv()
    {
        unoIDCheck       = 0;
        unoIDEdit        = 0;
        destinationCheck = 0;
        destinationEdit  = 0;
        serviceIDCheck   = 0;
        serviceIDEdit    = 0;
        productIDCheck   = 0;
        productIDEdit    = 0;
        envelopeIDCheck  = 0;
        envelopeIDEdit   = 0;
        priorityCB       = 0;
        priorityCheck    = 0;
        dateSentSel      = 0;
        timeSentSel      = 0;
        dateSentCheck    = 0;
        timeSentCheck    = 0;
        setTodaySentBtn  = 0;
    }

    QTimeEdit        *timeSentSel;

    QComboBox        *priorityCB;

    QCheckBox        *unoIDCheck;
    QCheckBox        *destinationCheck;
    QCheckBox        *serviceIDCheck;
    QCheckBox        *productIDCheck;
    QCheckBox        *envelopeIDCheck;
    QCheckBox        *dateSentCheck;
    QCheckBox        *timeSentCheck;

    QPushButton      *setTodaySentBtn;

    KLineEdit        *unoIDEdit;
    KLineEdit        *envelopeIDEdit;
    KLineEdit        *serviceIDEdit;
    KLineEdit        *productIDEdit;

    KDateWidget      *dateSentSel;

    KTextEdit        *destinationEdit;

    MetadataCheckBox *priorityCheck;
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

    d->unoIDCheck = new QCheckBox(i18n("U.N.O ID:"), this);
    d->unoIDEdit  = new KLineEdit(this);
    d->unoIDEdit->setClearButtonShown(true);
    d->unoIDEdit->setValidator(asciiValidator);
    d->unoIDEdit->setMaxLength(80);
    d->unoIDEdit->setWhatsThis(i18n("<p>Set here the Unique Name of Object identifier. "
                                  "This field is limited to 80 ASCII characters."));

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

    d->priorityCheck = new MetadataCheckBox(i18n("Priority:"), this);
    d->priorityCB    = new QComboBox(this);
    d->priorityCB->insertItem(0, i18n("0: None"));
    d->priorityCB->insertItem(1, i18n("1: High"));
    d->priorityCB->insertItem(2, "2");
    d->priorityCB->insertItem(3, "3");
    d->priorityCB->insertItem(4, "4");
    d->priorityCB->insertItem(5, i18n("5: Normal"));
    d->priorityCB->insertItem(6, "6");
    d->priorityCB->insertItem(7, "7");
    d->priorityCB->insertItem(8, i18n("8: Low"));
    d->priorityCB->setWhatsThis(i18n("<p>Select here the envelope priority."));

    // --------------------------------------------------------

    d->dateSentCheck   = new QCheckBox(i18n("Sent date:"), this);
    d->timeSentCheck   = new QCheckBox(i18n("Sent time:"), this);
    d->dateSentSel     = new KDateWidget(this);
    d->timeSentSel     = new QTimeEdit(this);

    d->setTodaySentBtn = new QPushButton();
    d->setTodaySentBtn->setIcon(SmallIcon("calendar-today"));
    d->setTodaySentBtn->setWhatsThis(i18n("Set envelope sent date to today"));

    d->dateSentSel->setWhatsThis(i18n("<p>Set here the date when the service sent the material."));
    d->timeSentSel->setWhatsThis(i18n("<p>Set here the time when the service sent the material."));
    slotSetTodaySent();

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

    grid->addWidget(d->destinationCheck, 0, 0, 1, 5);
    grid->addWidget(d->destinationEdit, 1, 0, 1, 5);
    grid->addWidget(d->unoIDCheck, 2, 0, 1, 1);
    grid->addWidget(d->unoIDEdit, 2, 1, 1, 4);
    grid->addWidget(d->productIDCheck, 3, 0, 1, 1);
    grid->addWidget(d->productIDEdit, 3, 1, 1, 4);
    grid->addWidget(d->serviceIDCheck, 4, 0, 1, 1);
    grid->addWidget(d->serviceIDEdit, 4, 1, 1, 1);
    grid->addWidget(d->envelopeIDCheck, 5, 0, 1, 1);
    grid->addWidget(d->envelopeIDEdit, 5, 1, 1, 1);
    grid->addWidget(d->priorityCheck, 6, 0, 1, 1);
    grid->addWidget(d->priorityCB, 6, 1, 1, 1);
    grid->addWidget(d->dateSentCheck, 7, 0, 1, 2);
    grid->addWidget(d->timeSentCheck, 7, 2, 1, 2);
    grid->addWidget(d->dateSentSel, 8, 0, 1, 2);
    grid->addWidget(d->timeSentSel, 8, 2, 1, 1);
    grid->addWidget(d->setTodaySentBtn, 8, 4, 1, 1);
    grid->addWidget(note, 9, 0, 1, 5);
    grid->setColumnStretch(3, 10);                     
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

    connect(d->unoIDCheck, SIGNAL(toggled(bool)),
            d->unoIDEdit, SLOT(setEnabled(bool)));

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            d->priorityCB, SLOT(setEnabled(bool)));

    connect(d->dateSentCheck, SIGNAL(toggled(bool)),
            d->dateSentSel, SLOT(setEnabled(bool)));

    connect(d->timeSentCheck, SIGNAL(toggled(bool)),
            d->timeSentSel, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->envelopeIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->destinationCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->serviceIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->productIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->unoIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateSentCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeSentCheck, SIGNAL(toggled(bool)),
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

    connect(d->unoIDEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->priorityCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->dateSentSel, SIGNAL(changed(const QDate&)),
            this, SIGNAL(signalModified()));

    connect(d->timeSentSel, SIGNAL(timeChanged(const QTime &)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->setTodaySentBtn, SIGNAL(clicked()),
            this, SLOT(slotSetTodaySent()));
}

IPTCEnvelope::~IPTCEnvelope()
{
    delete d;
}

void IPTCEnvelope::slotSetTodaySent()
{
    d->dateSentSel->setDate(QDate::currentDate());
    d->timeSentSel->setTime(QTime::currentTime());
}

void IPTCEnvelope::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    int         val;
    QString     data;
    QStringList list;
    QDate       date;
    QTime       time;
    QString     dateStr, timeStr;

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

    d->unoIDEdit->clear();
    d->unoIDCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Envelope.UNO", false);    
    if (!data.isNull())
    {
        d->unoIDEdit->setText(data);
        d->unoIDCheck->setChecked(true);
    }
    d->unoIDEdit->setEnabled(d->unoIDCheck->isChecked());

    d->productIDEdit->clear();
    d->productIDCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Envelope.ProductId", false);    
    if (!data.isNull())
    {
        d->productIDEdit->setText(data);
        d->productIDCheck->setChecked(true);
    }
    d->productIDEdit->setEnabled(d->productIDCheck->isChecked());

    d->priorityCB->setCurrentIndex(0);
    d->priorityCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Envelope.EnvelopePriority", false);    
    if (!data.isNull())
    {
        val = data.toInt(); 
        if (val >= 0 && val <= 8)
        {
            d->priorityCB->setCurrentIndex(val);
            d->priorityCheck->setChecked(true);
        }
        else
            d->priorityCheck->setValid(false);
    }
    d->priorityCB->setEnabled(d->priorityCheck->isChecked());

    dateStr = exiv2Iface.getIptcTagString("Iptc.Envelope.DateSent", false);
    timeStr = exiv2Iface.getIptcTagString("Iptc.Envelope.TimeSent", false);

    d->dateSentSel->setDate(QDate::currentDate());
    d->dateSentCheck->setChecked(false);
    if (!dateStr.isEmpty()) 
    {
        date = QDate::fromString(dateStr, Qt::ISODate);
        if (date.isValid())
        {
            d->dateSentSel->setDate(date);
            d->dateSentCheck->setChecked(true);
        }
    }    
    d->dateSentSel->setEnabled(d->dateSentCheck->isChecked());

    d->timeSentSel->setTime(QTime::currentTime());
    d->timeSentCheck->setChecked(false);
    if (!timeStr.isEmpty()) 
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeSentSel->setTime(time);
            d->timeSentCheck->setChecked(true);
        }
    }    
    d->timeSentSel->setEnabled(d->timeSentCheck->isChecked());

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

    if (d->unoIDCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.UNO", d->unoIDEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Envelope.UNO");

    if (d->productIDCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.ProductId", d->productIDEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Envelope.ProductId");

    if (d->priorityCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.EnvelopePriority", QString::number(d->priorityCB->currentIndex()));
    else if (d->priorityCheck->isValid())
        exiv2Iface.removeIptcTag("Iptc.Envelope.EnvelopePriority");

    if (d->dateSentCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.DateSent",
                                    d->dateSentSel->date().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Envelope.DateSent");

    if (d->timeSentCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Envelope.TimeSent",
                                    d->timeSentSel->time().toString(Qt::ISODate));
    else
        exiv2Iface.removeIptcTag("Iptc.Envelope.TimeSent");

    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
