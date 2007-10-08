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

#include <QLayout>
#include <QLabel>
#include <QValidator>
#include <QCheckBox>
#include <QPushButton>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klistwidget.h>
#include <klineedit.h>
#include <kiconloader.h>

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
        repSubjectButton = 0;
        subjectsBox      = 0;
        subjectsCheck    = 0;
        subjectEdit      = 0;
    }

    QStringList  oldSubjects;

    QPushButton *addSubjectButton;
    QPushButton *delSubjectButton;
    QPushButton *repSubjectButton;

    QCheckBox   *subjectsCheck;

    KLineEdit   *subjectEdit;

    KListWidget *subjectsBox;
};

IPTCSubjects::IPTCSubjects(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCSubjectsPriv;
    QGridLayout *grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->subjectsCheck = new QCheckBox(i18n("Use structured definition of the subject matter:"), this);    

    d->subjectEdit   = new KLineEdit(this);
    d->subjectEdit->setClearButtonShown(true);
    d->subjectEdit->setValidator(asciiValidator);
    d->subjectEdit->setMaxLength(236);
    d->subjectEdit->setWhatsThis(i18n("<p>Enter here a new subject. "
                                      "This field is limited to 236 ASCII characters."));

    QLabel *codeDesc = new QLabel(i18n("Subject codes are defined at "
           "<b><a href='http://www.iptc.org/NewsCodes'>www.iptc.org/NewsCodes</a></b></b>"), this);
    codeDesc->setOpenExternalLinks(true);
    codeDesc->setWordWrap(true);

    d->subjectsBox   = new KListWidget(this);
    d->subjectsBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    d->addSubjectButton = new QPushButton( i18n("&Add"), this);
    d->delSubjectButton = new QPushButton( i18n("&Delete"), this);
    d->repSubjectButton = new QPushButton( i18n("&Replace"), this);
    d->addSubjectButton->setIcon(SmallIcon("edit-add"));
    d->delSubjectButton->setIcon(SmallIcon("edit-delete"));
    d->repSubjectButton->setIcon(SmallIcon("view-refresh"));
    d->delSubjectButton->setEnabled(false);
    d->repSubjectButton->setEnabled(false);

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"), this);
    note->setMaximumWidth(150);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->subjectsCheck, 0, 0, 1, 2 );
    grid->addWidget(d->subjectEdit, 1, 0, 1, 2);
    grid->addWidget(codeDesc, 2, 0, 1, 2);
    grid->addWidget(d->subjectsBox, 3, 0, 5, 1);
    grid->addWidget(d->addSubjectButton, 3, 1, 1, 1);
    grid->addWidget(d->delSubjectButton, 4, 1, 1, 1);
    grid->addWidget(d->repSubjectButton, 5, 1, 1, 1);
    grid->addWidget(note, 6, 1, 1, 1);
    grid->setColumnStretch(0, 10);                     
    grid->setRowStretch(7, 10);  
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());    
                                         
    // --------------------------------------------------------

    connect(d->subjectsBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSubjectSelectionChanged()));
    
    connect(d->addSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotAddSubject()));
    
    connect(d->delSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotDelSubject()));

    connect(d->repSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotRepSubject()));

    // --------------------------------------------------------

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->subjectEdit, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->subjectsBox, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->addSubjectButton, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->delSubjectButton, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->repSubjectButton, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->delSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
}

IPTCSubjects::~IPTCSubjects()
{
    delete d;
}

void IPTCSubjects::slotDelSubject()
{
    QListWidgetItem *item = d->subjectsBox->currentItem();
    if (!item) return;
    d->subjectsBox->takeItem(d->subjectsBox->row(item));
    delete item;
}

void IPTCSubjects::slotRepSubject()
{
    QString newSubject = d->subjectEdit->text();
    if (newSubject.isEmpty()) return;

    if (!d->subjectsBox->selectedItems().isEmpty())
    {
        d->subjectsBox->selectedItems()[0]->setText(newSubject);
        d->subjectEdit->clear();
    }
}

void IPTCSubjects::slotSubjectSelectionChanged()
{
    if (!d->subjectsBox->selectedItems().isEmpty())
    {
        d->subjectEdit->setText(d->subjectsBox->selectedItems()[0]->text());
        d->delSubjectButton->setEnabled(true);
        d->repSubjectButton->setEnabled(true);
    }
    else
    {
        d->delSubjectButton->setEnabled(false);
        d->repSubjectButton->setEnabled(false);
    }
}

void IPTCSubjects::slotAddSubject()
{
    QString newSubject = d->subjectEdit->text();
    if (newSubject.isEmpty()) return;

    bool found = false;
    for (int i = 0 ; i < d->subjectsBox->count(); i++)
    {
        QListWidgetItem *item = d->subjectsBox->item(i);
        if (newSubject == item->text()) 
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->subjectsBox->insertItem(d->subjectsBox->count(), newSubject);
        d->subjectEdit->clear();
    }
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
