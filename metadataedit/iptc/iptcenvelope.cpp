/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-10
 * Description : IPTC envelope settings page.
 *
 * Copyright (C) 2007-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "iptcenvelope.moc"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QTimeEdit>
#include <QValidator>
#include <QGridLayout>

// KDE includes

#include <kcombobox.h>
#include <kdatewidget.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <ktextedit.h>

// LibKDcraw includes

#include <libkdcraw/squeezedcombobox.h>

// Local includes

#include "metadatacheckbox.h"
#include "timezonecombobox.h"
#include "kpversion.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIMetadataEditPlugin
{

class IPTCEnvelope::IPTCEnvelopePriv
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
        zoneSentSel      = 0;
        dateSentCheck    = 0;
        timeSentCheck    = 0;
        setTodaySentBtn  = 0;
        formatCB         = 0;
        formatCheck      = 0;

        // Map : "file format - version"  ==> description

        fileFormatMap.insert( "00-00", i18n("No ObjectData") );
        fileFormatMap.insert( "01-01", i18n("IPTC-NAA Digital Newsphoto Parameter Record (version 1)") );
        fileFormatMap.insert( "01-02", i18n("IPTC-NAA Digital Newsphoto Parameter Record (version 2)") );
        fileFormatMap.insert( "01-03", i18n("IPTC-NAA Digital Newsphoto Parameter Record (version 3)") );
        fileFormatMap.insert( "01-04", i18n("IPTC-NAA Digital Newsphoto Parameter Record (version 4)") );
        fileFormatMap.insert( "02-04", i18n("IPTC7901 Recommended Message Format") );
        fileFormatMap.insert( "03-01", i18n("Tagged Image File Format (version 5.0)") );
        fileFormatMap.insert( "03-02", i18n("Tagged Image File Format (version 6.0)") );
        fileFormatMap.insert( "04-01", i18n("Illustrator") );
        fileFormatMap.insert( "05-01", i18n("AppleSingle") );
        fileFormatMap.insert( "06-01", i18n("NAA 89-3 (ANPA 1312)") );
        fileFormatMap.insert( "07-01", i18n("MacBinary II") );
        fileFormatMap.insert( "08-01", i18n("IPTC Unstructured Character Oriented File Format") );
        fileFormatMap.insert( "09-01", i18n("United Press International ANPA 1312 variant") );
        fileFormatMap.insert( "10-01", i18n("United Press International Down-Load Message") );
        fileFormatMap.insert( "11-01", i18n("JPEG File Interchange") );
        fileFormatMap.insert( "12-01", i18n("Photo-CD Image-Pac") );
        fileFormatMap.insert( "13-01", i18n("Microsoft Bit Mapped Graphics File [*.BMP]") );
        fileFormatMap.insert( "14-01", i18n("Digital Audio File [*.WAV]") );
        fileFormatMap.insert( "15-01", i18n("Audio plus Moving Video [*.AVI]") );
        fileFormatMap.insert( "16-01", i18n("PC DOS/Windows Executable Files [*.COM][*.EXE]") );
        fileFormatMap.insert( "17-01", i18n("Compressed Binary File [*.ZIP]") );
        fileFormatMap.insert( "18-01", i18n("Audio Interchange File Format AIFF") );
        fileFormatMap.insert( "19-01", i18n("RIFF Wave (Microsoft Corporation)") );
        fileFormatMap.insert( "20-01", i18n("Freehand (version 3.1)") );
        fileFormatMap.insert( "20-02", i18n("Freehand (version 4.0)") );
        fileFormatMap.insert( "20-03", i18n("Freehand (version 5.0)") );
        fileFormatMap.insert( "20-04", i18n("Freehand (version 5.5)") );
        fileFormatMap.insert( "21-01", i18n("Hypertext Markup Language \"HTML\"") );
        fileFormatMap.insert( "22-01", i18n("MPEG 2 Audio Layer 2 (Musicom), ISO/IEC") );
        fileFormatMap.insert( "23-01", i18n("MPEG 2 Audio Layer 3, ISO/IEC") );
        fileFormatMap.insert( "24-01", i18n("Portable Document File [*.PDF] Adobe") );
        fileFormatMap.insert( "25-01", i18n("News Industry Text Format") );
        fileFormatMap.insert( "26-01", i18n("Tape Archive [*.TAR]") );
        fileFormatMap.insert( "27-01", i18n("Tidningarnas Telegrambyra NITF version (TTNITF DTD)") );
        fileFormatMap.insert( "28-01", i18n("Ritzaus Bureau NITF version (RBNITF DTD)") );
        fileFormatMap.insert( "29-01", i18n("Corel Draw [*.CDR]") );
    }

    typedef QMap<QString, QString> FileFormatMap;

    FileFormatMap                  fileFormatMap;

    QTimeEdit*                     timeSentSel;

    TimeZoneComboBox*              zoneSentSel;

    KComboBox*                     priorityCB;

    QCheckBox*                     unoIDCheck;
    QCheckBox*                     destinationCheck;
    QCheckBox*                     serviceIDCheck;
    QCheckBox*                     productIDCheck;
    QCheckBox*                     envelopeIDCheck;
    QCheckBox*                     dateSentCheck;
    QCheckBox*                     timeSentCheck;

    QPushButton*                   setTodaySentBtn;

    KLineEdit*                     unoIDEdit;
    KLineEdit*                     envelopeIDEdit;
    KLineEdit*                     serviceIDEdit;
    KLineEdit*                     productIDEdit;

    KDateWidget*                   dateSentSel;

    KTextEdit*                     destinationEdit;

    MetadataCheckBox*              priorityCheck;
    MetadataCheckBox*              formatCheck;

    SqueezedComboBox*              formatCB;
};

IPTCEnvelope::IPTCEnvelope(QWidget* const parent)
    : QWidget(parent), d(new IPTCEnvelopePriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator* asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->destinationCheck = new QCheckBox(i18n("Destination:"), this);
    d->destinationEdit  = new KTextEdit(this);
/*    d->specialInstructionEdit->setValidator(asciiValidator);
    d->specialInstructionEdit->document()->setMaxLength;*/
    d->destinationEdit->setWhatsThis(i18n("Enter the envelope destination. "
                                          "This field is limited to 1024 ASCII characters."));

    // --------------------------------------------------------

    d->unoIDCheck = new QCheckBox(i18n("U.N.O ID:"), this);
    d->unoIDEdit  = new KLineEdit(this);
    d->unoIDEdit->setClearButtonShown(true);
    d->unoIDEdit->setValidator(asciiValidator);
    d->unoIDEdit->setMaxLength(80);
    d->unoIDEdit->setWhatsThis(i18n("Set here the Unique Name of Object identifier. "
                                  "This field is limited to 80 ASCII characters."));

    // --------------------------------------------------------

    d->productIDCheck = new QCheckBox(i18n("Product ID:"), this);
    d->productIDEdit  = new KLineEdit(this);
    d->productIDEdit->setClearButtonShown(true);
    d->productIDEdit->setValidator(asciiValidator);
    d->productIDEdit->setMaxLength(32);
    d->productIDEdit->setWhatsThis(i18n("Set here the product identifier. "
                                         "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->serviceIDCheck = new QCheckBox(i18n("Service ID:"), this);
    d->serviceIDEdit  = new KLineEdit(this);
    d->serviceIDEdit->setClearButtonShown(true);
    d->serviceIDEdit->setValidator(asciiValidator);
    d->serviceIDEdit->setMaxLength(10);
    d->serviceIDEdit->setWhatsThis(i18n("Set here the service identifier. "
                                         "This field is limited to 10 ASCII characters."));

    // --------------------------------------------------------

    d->envelopeIDCheck = new QCheckBox(i18n("Envelope ID:"), this);
    d->envelopeIDEdit  = new KLineEdit(this);
    d->envelopeIDEdit->setClearButtonShown(true);
    d->envelopeIDEdit->setValidator(asciiValidator);
    d->envelopeIDEdit->setMaxLength(8);
    d->envelopeIDEdit->setWhatsThis(i18n("Set here the envelope identifier. "
                                         "This field is limited to 8 ASCII characters."));

    // --------------------------------------------------------

    d->priorityCheck = new MetadataCheckBox(i18n("Priority:"), this);
    d->priorityCB    = new KComboBox(this);
    d->priorityCB->insertItem(0, i18nc("priority for the envelope",    "0: None"));
    d->priorityCB->insertItem(1, i18nc("priority for the envelope",    "1: High"));
    d->priorityCB->insertItem(2, "2");
    d->priorityCB->insertItem(3, "3");
    d->priorityCB->insertItem(4, "4");
    d->priorityCB->insertItem(5, i18nc("priority for the envelope",    "5: Normal"));
    d->priorityCB->insertItem(6, "6");
    d->priorityCB->insertItem(7, "7");
    d->priorityCB->insertItem(8, i18nc("priority for the envelope",    "8: Low"));
    d->priorityCB->insertItem(9, i18nc("editorial urgency of content", "9: User-defined"));
    d->priorityCB->setWhatsThis(i18n("Select here the envelope priority."));

    // --------------------------------------------------------

    d->formatCheck = new MetadataCheckBox(i18n("Format:"), this);
    d->formatCB    = new SqueezedComboBox(this);

    int index = 0;
    for (IPTCEnvelopePriv::FileFormatMap::Iterator it = d->fileFormatMap.begin();
         it != d->fileFormatMap.end(); ++it)
    {
        d->formatCB->insertSqueezedItem(it.value(), index);
        index++;
    }

    d->formatCB->model()->sort(0);
    d->formatCB->setWhatsThis(i18n("Select here envelope file format."));

    // --------------------------------------------------------

    d->dateSentCheck   = new QCheckBox(i18n("Sent date:"), this);
    d->timeSentCheck   = new QCheckBox(i18n("Sent time:"), this);
    d->zoneSentSel     = new TimeZoneComboBox(this);
    d->dateSentSel     = new KDateWidget(this);
    d->timeSentSel     = new QTimeEdit(this);

    d->setTodaySentBtn = new QPushButton();
    d->setTodaySentBtn->setIcon(SmallIcon("go-jump-today"));
    d->setTodaySentBtn->setWhatsThis(i18n("Set envelope sent date to today"));

    d->dateSentSel->setWhatsThis(i18n("Set here the date when the service sent the material."));
    d->timeSentSel->setWhatsThis(i18n("Set here the time when the service sent the material."));
    d->zoneSentSel->setWhatsThis(i18n("Set here the time zone when the service sent the material."));

    slotSetTodaySent();

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC_Information_Interchange_Model'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters and limit string sizes. "
                 "Use contextual help for details.</b>"), this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->addWidget(d->destinationCheck,    0, 0, 1, 6);
    grid->addWidget(d->destinationEdit,     1, 0, 1, 6);
    grid->addWidget(d->unoIDCheck,          2, 0, 1, 1);
    grid->addWidget(d->unoIDEdit,           2, 1, 1, 5);
    grid->addWidget(d->productIDCheck,      3, 0, 1, 1);
    grid->addWidget(d->productIDEdit,       3, 1, 1, 5);
    grid->addWidget(d->serviceIDCheck,      4, 0, 1, 1);
    grid->addWidget(d->serviceIDEdit,       4, 1, 1, 1);
    grid->addWidget(d->envelopeIDCheck,     5, 0, 1, 1);
    grid->addWidget(d->envelopeIDEdit,      5, 1, 1, 1);
    grid->addWidget(d->priorityCheck,       6, 0, 1, 1);
    grid->addWidget(d->priorityCB,          6, 1, 1, 1);
    grid->addWidget(d->formatCheck,         7, 0, 1, 1);
    grid->addWidget(d->formatCB,            7, 1, 1, 5);
    grid->addWidget(d->dateSentCheck,       8, 0, 1, 2);
    grid->addWidget(d->timeSentCheck,       8, 2, 1, 2);
    grid->addWidget(d->dateSentSel,         9, 0, 1, 2);
    grid->addWidget(d->timeSentSel,         9, 2, 1, 1);
    grid->addWidget(d->zoneSentSel,         9, 3, 1, 1);
    grid->addWidget(d->setTodaySentBtn,     9, 5, 1, 1);
    grid->addWidget(note,                  10, 0, 1, 6);
    grid->setColumnStretch(4, 10);
    grid->setRowStretch(11, 10);
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

    connect(d->formatCheck, SIGNAL(toggled(bool)),
            d->formatCB, SLOT(setEnabled(bool)));

    connect(d->dateSentCheck, SIGNAL(toggled(bool)),
            d->dateSentSel, SLOT(setEnabled(bool)));

    connect(d->timeSentCheck, SIGNAL(toggled(bool)),
            d->timeSentSel, SLOT(setEnabled(bool)));

    connect(d->timeSentCheck, SIGNAL(toggled(bool)),
            d->zoneSentSel, SLOT(setEnabled(bool)));

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

    connect(d->formatCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateSentCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->timeSentCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->envelopeIDEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->destinationEdit, SIGNAL(textChanged()),
            this, SIGNAL(signalModified()));

    connect(d->serviceIDEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->productIDEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->unoIDEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->priorityCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->formatCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->dateSentSel, SIGNAL(changed(QDate)),
            this, SIGNAL(signalModified()));

    connect(d->timeSentSel, SIGNAL(timeChanged(QTime)),
            this, SIGNAL(signalModified()));

    connect(d->zoneSentSel, SIGNAL(currentIndexChanged(QString)),
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
    d->zoneSentSel->setToUTC();
}

void IPTCEnvelope::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KPMetadata meta;
    meta.setIptc(iptcData);

    QString     data, format, version;
    QStringList list;
    QDate       date;
    QTime       time;
    QString     dateStr, timeStr;

    d->destinationEdit->clear();
    d->destinationCheck->setChecked(false);
    data = meta.getIptcTagString("Iptc.Envelope.Destination", false);

    if (!data.isNull())
    {
        d->destinationEdit->setText(data);
        d->destinationCheck->setChecked(true);
    }

    d->destinationEdit->setEnabled(d->destinationCheck->isChecked());

    d->envelopeIDEdit->clear();
    d->envelopeIDCheck->setChecked(false);
    data = meta.getIptcTagString("Iptc.Envelope.EnvelopeNumber", false);

    if (!data.isNull())
    {
        d->envelopeIDEdit->setText(data);
        d->envelopeIDCheck->setChecked(true);
    }

    d->envelopeIDEdit->setEnabled(d->envelopeIDCheck->isChecked());

    d->serviceIDEdit->clear();
    d->serviceIDCheck->setChecked(false);
    data = meta.getIptcTagString("Iptc.Envelope.ServiceId", false);

    if (!data.isNull())
    {
        d->serviceIDEdit->setText(data);
        d->serviceIDCheck->setChecked(true);
    }

    d->serviceIDEdit->setEnabled(d->serviceIDCheck->isChecked());

    d->unoIDEdit->clear();
    d->unoIDCheck->setChecked(false);
    data = meta.getIptcTagString("Iptc.Envelope.UNO", false);

    if (!data.isNull())
    {
        d->unoIDEdit->setText(data);
        d->unoIDCheck->setChecked(true);
    }

    d->unoIDEdit->setEnabled(d->unoIDCheck->isChecked());

    d->productIDEdit->clear();
    d->productIDCheck->setChecked(false);
    data = meta.getIptcTagString("Iptc.Envelope.ProductId", false);

    if (!data.isNull())
    {
        d->productIDEdit->setText(data);
        d->productIDCheck->setChecked(true);
    }

    d->productIDEdit->setEnabled(d->productIDCheck->isChecked());

    d->priorityCB->setCurrentIndex(0);
    d->priorityCheck->setChecked(false);
    data = meta.getIptcTagString("Iptc.Envelope.EnvelopePriority", false);

    if (!data.isNull())
    {
        const int val = data.toInt();
        if (val >= 0 && val <= 9)
        {
            d->priorityCB->setCurrentIndex(val);
            d->priorityCheck->setChecked(true);
        }
        else
            d->priorityCheck->setValid(false);
    }

    d->priorityCB->setEnabled(d->priorityCheck->isChecked());

    d->formatCB->setCurrentIndex(0);
    d->formatCheck->setChecked(false);
    format  = meta.getIptcTagString("Iptc.Envelope.FileFormat", false);
    version = meta.getIptcTagString("Iptc.Envelope.FileVersion", false);

    if (!format.isNull())
    {
        if (!version.isNull())
        {
            if (format.size() == 1) format.prepend("0");
            if (version.size() == 1) version.prepend("0");
            QString key = QString("%1-%2").arg(format).arg(version);
            int index = -1, i = 0;
            for (IPTCEnvelopePriv::FileFormatMap::Iterator it = d->fileFormatMap.begin();
                 it != d->fileFormatMap.end(); ++it)
            {
                if (it.key() == key) index = i;
                i++;
            }

            if (index != -1)
            {
                d->formatCB->setCurrentIndex(index);
                d->formatCheck->setChecked(true);
            }
            else
                d->formatCheck->setValid(false);
        }
        else
            d->formatCheck->setValid(false);
    }

    d->formatCB->setEnabled(d->formatCheck->isChecked());

    dateStr = meta.getIptcTagString("Iptc.Envelope.DateSent", false);
    timeStr = meta.getIptcTagString("Iptc.Envelope.TimeSent", false);

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
    d->zoneSentSel->setToUTC();

    if (!timeStr.isEmpty())
    {
        time = QTime::fromString(timeStr, Qt::ISODate);
        if (time.isValid())
        {
            d->timeSentSel->setTime(time);
            d->timeSentCheck->setChecked(true);
            d->zoneSentSel->setTimeZone(timeStr);
       }
    }

    d->timeSentSel->setEnabled(d->timeSentCheck->isChecked());
    d->zoneSentSel->setEnabled(d->timeSentCheck->isChecked());

    blockSignals(false);
}

void IPTCEnvelope::applyMetadata(QByteArray& iptcData)
{
    KPMetadata meta;
    meta.setIptc(iptcData);

    if (d->destinationCheck->isChecked())
    {
        meta.setIptcTagString("Iptc.Envelope.Destination", d->destinationEdit->toPlainText());
    }
    else
    {
        meta.removeIptcTag("Iptc.Envelope.Destination");
    }

    if (d->envelopeIDCheck->isChecked())
    {
        meta.setIptcTagString("Iptc.Envelope.EnvelopeNumber", d->envelopeIDEdit->text());
    }
    else
    {
        meta.removeIptcTag("Iptc.Envelope.EnvelopeNumber");
    }

    if (d->serviceIDCheck->isChecked())
    {
        meta.setIptcTagString("Iptc.Envelope.ServiceId", d->serviceIDEdit->text());
    }
    else
    {
        meta.removeIptcTag("Iptc.Envelope.ServiceId");
    }

    if (d->unoIDCheck->isChecked())
    {
        meta.setIptcTagString("Iptc.Envelope.UNO", d->unoIDEdit->text());
    }
    else
    {
        meta.removeIptcTag("Iptc.Envelope.UNO");
    }

    if (d->productIDCheck->isChecked())
    {
        meta.setIptcTagString("Iptc.Envelope.ProductId", d->productIDEdit->text());
    }
    else
    {
        meta.removeIptcTag("Iptc.Envelope.ProductId");
    }

    if (d->priorityCheck->isChecked())
    {
        meta.setIptcTagString("Iptc.Envelope.EnvelopePriority", QString::number(d->priorityCB->currentIndex()));
    }
    else if (d->priorityCheck->isValid())
    {
        meta.removeIptcTag("Iptc.Envelope.EnvelopePriority");
    }

    if (d->formatCheck->isChecked())
    {
        QString key;
        int i = 0;

        for (IPTCEnvelopePriv::FileFormatMap::Iterator it = d->fileFormatMap.begin();
             it != d->fileFormatMap.end(); ++it)
        {
            if (i == d->formatCB->currentIndex()) key = it.key();
            i++;
        }

        QString format  = key.section('-', 0, 0);
        QString version = key.section('-', -1);
        meta.setIptcTagString("Iptc.Envelope.FileFormat", format);
        meta.setIptcTagString("Iptc.Envelope.FileVersion", version);
    }
    else if (d->priorityCheck->isValid())
    {
        meta.removeIptcTag("Iptc.Envelope.FileFormat");
        meta.removeIptcTag("Iptc.Envelope.FileVersion");
    }

    if (d->dateSentCheck->isChecked())
    {
        meta.setIptcTagString("Iptc.Envelope.DateSent",
                                    d->dateSentSel->date().toString(Qt::ISODate));
    }
    else
    {
        meta.removeIptcTag("Iptc.Envelope.DateSent");
    }

    if (d->timeSentCheck->isChecked())
    {
        meta.setIptcTagString("Iptc.Envelope.TimeSent",
                                    d->timeSentSel->time().toString(Qt::ISODate) +
                                    d->zoneSentSel->getTimeZone());
    }
    else
    {
        meta.removeIptcTag("Iptc.Envelope.TimeSent");
    }

    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    iptcData = meta.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
