/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : IPTC subjects settings page.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qvalidator.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klistbox.h>
#include <klineedit.h>
#include <kactivelabel.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "iptcsubjects.h"
#include "iptcsubjects.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCSubjectsPriv
{
public:

    IPTCSubjectsPriv()
    {
        addSubjectButton = 0;
        delSubjectButton = 0;
        subjectsBox      = 0;
        subjectsCheck    = 0;
        subjectEdit      = 0;
    }

    QStringList  oldSubjects;

    QPushButton *addSubjectButton;
    QPushButton *delSubjectButton;

    QCheckBox   *subjectsCheck;

    KLineEdit   *subjectEdit;

    KListBox    *subjectsBox;
};

IPTCSubjects::IPTCSubjects(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCSubjectsPriv;
    QGridLayout *grid = new QGridLayout(parent, 5, 2, 0, KDialog::spacingHint());
    grid->setAlignment( Qt::AlignTop );

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->subjectsCheck = new QCheckBox(i18n("Use structured definition of the subject matter:"), parent);    

    d->subjectEdit   = new KLineEdit(parent);
    d->subjectEdit->setValidator(asciiValidator);
    d->subjectEdit->setMaxLength(236);
    QWhatsThis::add(d->subjectEdit, i18n("<p>Enter here a new subject. "
                    "This field is limited to 236 ASCII characters."));

    d->subjectsBox   = new KListBox(parent);
    d->subjectsBox->setVScrollBarMode(QScrollView::AlwaysOn);
    
    d->addSubjectButton = new QPushButton( i18n("&Add"), parent);
    d->delSubjectButton = new QPushButton( i18n("&Delete"), parent);
    d->delSubjectButton->setEnabled(false);

    grid->addMultiCellWidget(d->subjectsCheck, 0, 0, 0, 1);
    grid->addMultiCellWidget(d->subjectEdit, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->subjectsBox, 2, 5, 0, 0);
    grid->addMultiCellWidget(d->addSubjectButton, 2, 2, 1, 1);
    grid->addMultiCellWidget(d->delSubjectButton, 3, 3, 1, 1);

    // --------------------------------------------------------

    KActiveLabel *note = new KActiveLabel(i18n("<b>Note: These informations are used to set "
                   "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> tags contents. "
                   "Take a care than IPTC text tags only support the printable "
                   "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b>"
                   "characters set and limit strings size. "
                   "Use contextual help for details.</b>"), parent);

    grid->addMultiCellWidget(note, 4, 4, 1, 1);
    grid->setColStretch(0, 10);                     
    grid->setRowStretch(5, 10);      
                                         
    // --------------------------------------------------------

    connect(d->subjectsBox, SIGNAL(selectionChanged()),
            this, SLOT(slotSubjectSelectionChanged()));
    
    connect(d->addSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotAddSubject()));
    
    connect(d->delSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotDelSubject()));

    // --------------------------------------------------------

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->subjectEdit, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->subjectsBox, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->addSubjectButton, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->delSubjectButton, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->delSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
}

IPTCSubjects::~IPTCSubjects()
{
    delete d;
}

void IPTCSubjects::slotDelSubject()
{
    int index = d->subjectsBox->currentItem();
    if (index == -1)
        return;

    QListBoxItem* item = d->subjectsBox->item(index);
    if (!item) return;
    delete item;
}

void IPTCSubjects::slotSubjectSelectionChanged()
{
    if (d->subjectsBox->currentItem() != -1)
        d->delSubjectButton->setEnabled(true);
    else
        d->delSubjectButton->setEnabled(false);
}

void IPTCSubjects::slotAddSubject()
{
    QString newSubject = d->subjectEdit->text();
    if (newSubject.isEmpty()) return;

    bool found = false;
    for (QListBoxItem *item = d->subjectsBox->firstItem();
         item; item = item->next()) 
    {
        if (newSubject == item->text()) 
        {
            found = true;
            break;
        }
    }

    if (!found)
        d->subjectsBox->insertItem(newSubject);
}

void IPTCSubjects::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->oldSubjects = exiv2Iface.getImageSubjects();

    d->subjectsBox->clear();
    d->subjectsCheck->setChecked(false);
    if (!d->oldSubjects.isEmpty())
    {
        d->subjectsBox->insertStringList(d->oldSubjects);
        d->subjectsCheck->setChecked(true);
    }
    d->subjectEdit->setEnabled(d->subjectsCheck->isChecked());
    d->subjectsBox->setEnabled(d->subjectsCheck->isChecked());
    d->addSubjectButton->setEnabled(d->subjectsCheck->isChecked());
    d->delSubjectButton->setEnabled(d->subjectsCheck->isChecked());

    blockSignals(false);
}

void IPTCSubjects::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QStringList newSubjects;    

    for (QListBoxItem *item = d->subjectsBox->firstItem();
         item; item = item->next()) 
        newSubjects.append(item->text());

    if (d->subjectsCheck->isChecked())
        exiv2Iface.setImageSubjects(d->oldSubjects, newSubjects);
    else
        exiv2Iface.setImageSubjects(d->oldSubjects, QStringList());

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

