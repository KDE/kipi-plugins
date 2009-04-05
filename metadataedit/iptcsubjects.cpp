/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : IPTC subjects settings page.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "iptcsubjects.h"
#include "iptcsubjects.moc"
#include "metadataedit_global.h"

// Qt includes

#include <QCheckBox>
#include <QLabel>

// KDE includes

#include <klineedit.h>
#include <klistwidget.h>
#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

namespace KIPIMetadataEditPlugin
{

IPTCSubjects::IPTCSubjects(QWidget* parent)
            : Subjects(parent)
{
    // Subject string only accept printable Ascii char excepted these one:
    // - '*' (\x2A)
    // - ':' (\x3A)
    // - '?' (\x3F)
    QRegExp subjectAsciiRx("[\x20-\x29\x2B-\x39\x3B-\x3E\x40-\x7F]+$");
    QValidator *subjectAsciiValidator = new QRegExpValidator(subjectAsciiRx, this);

    // --------------------------------------------------------

    d->iprEdit->setText(QString("IPTC"));
    d->iprEdit->setValidator(subjectAsciiValidator);
    d->iprEdit->setWhatsThis(i18n("Enter here the Informative Provider Reference. "
                                  "I.P.R is a name registered with the IPTC/NAA, identifying the "
                                  "provider that provides an indicator of the content. "
                                  "The default value for the I.P.R is \"IPTC\" if a standard Reference "
                                  "Code is used. This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->refEdit->setWhatsThis(i18n("Enter here the Subject Reference Number. "
                                  "Provides a numeric code to indicate the Subject Name plus "
                                  "optional Subject Matter and Subject Detail Names in the "
                                  "language of the service. Subject Reference is a number "
                                  "from the range 01000000 to 17999999 and represent a "
                                  "language independent international reference to "
                                  "a Subject. A Subject is identified by its Reference Number "
                                  "and corresponding Names taken from a standard lists given "
                                  "by IPTC/NAA. If a standard reference code is used, these lists "
                                  "are the English language reference versions. "
                                  "This field is limited to 8 ASCII digit code."));

    // --------------------------------------------------------

    d->nameEdit->setValidator(subjectAsciiValidator);
    d->nameEdit->setWhatsThis(i18n("Enter here the Subject Name. English language is used "
                                   "if you selected a standard IPTC/NAA reference code. "
                                   "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->matterEdit->setValidator(subjectAsciiValidator);
    d->matterEdit->setWhatsThis(i18n("Enter here the Subject Matter Name. English language is used "
                                     "if you selected a standard IPTC/NAA reference code. "
                                     "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->detailEdit->setValidator(subjectAsciiValidator);
    d->detailEdit->setWhatsThis(i18n("Enter here the Subject Detail Name. English language is used "
                                     "if you selected a standard IPTC/NAA reference code. "
                                     "This field is limited to 64 ASCII characters."));

    d->note->setText(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"));
}

IPTCSubjects::~IPTCSubjects()
{
}

void IPTCSubjects::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->oldSubjects = exiv2Iface.getIptcSubjects();

    d->subjectsBox->clear();
    d->subjectsCheck->setChecked(false);
    if (!d->oldSubjects.isEmpty())
    {
        d->subjectsBox->insertItems(0, d->oldSubjects);
        d->subjectsCheck->setChecked(true);
    }
    blockSignals(false);
    slotSubjectsToggled(d->subjectsCheck->isChecked());
}

void IPTCSubjects::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QStringList newSubjects;

    for (int i = 0 ; i < d->subjectsBox->count(); i++)
    {
        QListWidgetItem *item = d->subjectsBox->item(i);
        newSubjects.append(item->text());
    }

    if (d->subjectsCheck->isChecked())
        exiv2Iface.setIptcSubjects(d->oldSubjects, newSubjects);
    else
        exiv2Iface.setIptcSubjects(d->oldSubjects, QStringList());

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
