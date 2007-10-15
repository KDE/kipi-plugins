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
        iprEdit          = 0;
        refEdit          = 0;
        nameEdit         = 0;
        matterEdit       = 0;
        detailEdit       = 0;
        iprLabel         = 0;
        refLabel         = 0;
        nameLabel        = 0;
        matterLabel      = 0;
        detailLabel      = 0;
    }

    QStringList  oldSubjects;

    QPushButton *addSubjectButton;
    QPushButton *delSubjectButton;
    QPushButton *repSubjectButton;

    QCheckBox   *subjectsCheck;

    QLabel      *iprLabel;
    QLabel      *refLabel;
    QLabel      *nameLabel;
    QLabel      *matterLabel;
    QLabel      *detailLabel;

    KLineEdit   *iprEdit;
    KLineEdit   *refEdit;
    KLineEdit   *nameEdit;
    KLineEdit   *matterEdit;
    KLineEdit   *detailEdit;

    KListWidget *subjectsBox;
};

IPTCSubjects::IPTCSubjects(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCSubjectsPriv;
    QGridLayout *grid = new QGridLayout(this);

    // Subject string only accept printable Ascii char excepted these one:
    // - '*' (\x2A)
    // - ':' (\x3A)
    // - '?' (\x3F)
    QRegExp subjectAsciiRx("[\x20-\x29\x2B-\x39\x3B-\x3E\x40-\x7F]+$");
    QValidator *subjectAsciiValidator = new QRegExpValidator(subjectAsciiRx, this);

    // Subject Reference Number only accept digit.
    QRegExp refDigitRx("^[0-9]{8}$");
    QValidator *refValidator = new QRegExpValidator(refDigitRx, this);

    // --------------------------------------------------------

    QLabel *codeDesc = new QLabel(i18n("Subject codes are defined at "
           "<b><a href='http://www.iptc.org/NewsCodes'>www.iptc.org/NewsCodes</a></b></b>"), this);
    codeDesc->setOpenExternalLinks(true);
    codeDesc->setWordWrap(true);

    d->subjectsCheck = new QCheckBox(i18n("Use structured definition of the subject matter:"), this);    

    d->iprEdit = new KLineEdit(this);
    d->iprEdit->setClearButtonShown(true);
    d->iprEdit->setValidator(subjectAsciiValidator);
    d->iprEdit->setMaxLength(32);
    d->iprEdit->setWhatsThis(i18n("<p>Enter here the Informative Provider Reference. "
                                  "This field is limited to 32 ASCII characters."));

    d->refEdit = new KLineEdit(this);
    d->refEdit->setClearButtonShown(true);
    d->refEdit->setValidator(refValidator);
    d->refEdit->setMaxLength(8);
    d->refEdit->setWhatsThis(i18n("<p>Enter here the Subject Reference Number. "
                                  "This field is limited to 8 ASCII digit code."));

    d->nameEdit = new KLineEdit(this);
    d->nameEdit->setClearButtonShown(true);
    d->nameEdit->setValidator(subjectAsciiValidator);
    d->nameEdit->setMaxLength(64);
    d->nameEdit->setWhatsThis(i18n("<p>Enter here the Subject Name. "
                                   "This field is limited to 64 ASCII characters."));

    d->matterEdit = new KLineEdit(this);
    d->matterEdit->setClearButtonShown(true);
    d->matterEdit->setValidator(subjectAsciiValidator);
    d->matterEdit->setMaxLength(64);
    d->matterEdit->setWhatsThis(i18n("<p>Enter here the Subject Matter Name. "
                                     "This field is limited to 64 ASCII characters."));

    d->detailEdit = new KLineEdit(this);
    d->detailEdit->setClearButtonShown(true);
    d->detailEdit->setValidator(subjectAsciiValidator);
    d->detailEdit->setMaxLength(64);
    d->detailEdit->setWhatsThis(i18n("<p>Enter here the Subject Detail Name. "
                                     "This field is limited to 64 ASCII characters."));

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

    d->iprLabel    = new QLabel(i18n("<b>I.P.R</b>"), this);
    d->refLabel    = new QLabel(i18n("<b>Reference</b>"), this);
    d->nameLabel   = new QLabel(i18n("<b>Name</b>"), this);
    d->matterLabel = new QLabel(i18n("<b>Matter</b>"), this);
    d->detailLabel = new QLabel(i18n("<b>Detail</b>"), this);
    d->iprLabel->setAlignment(Qt::AlignCenter);
    d->refLabel->setAlignment(Qt::AlignCenter);
    d->nameLabel->setAlignment(Qt::AlignCenter);
    d->matterLabel->setAlignment(Qt::AlignCenter);
    d->detailLabel->setAlignment(Qt::AlignCenter);

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
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(codeDesc, 0, 0, 1, 10);
    grid->addWidget(d->subjectsCheck, 1, 0, 1, 10);
    grid->addWidget(d->iprLabel, 2, 0, 1, 1);
    grid->addWidget(d->iprEdit, 3, 0, 1, 1);
    grid->addWidget(new QLabel(":", this), 3, 1, 1, 1);
    grid->addWidget(d->refLabel, 2, 2, 1, 1);
    grid->addWidget(d->refEdit, 3, 2, 1, 1);
    grid->addWidget(new QLabel(":", this), 3, 3, 1, 1);
    grid->addWidget(d->nameLabel,  2, 4, 1, 1);
    grid->addWidget(d->nameEdit, 3, 4, 1, 1);
    grid->addWidget(new QLabel(":", this), 3, 5, 1, 1);
    grid->addWidget(d->matterLabel, 2, 6, 1, 1);
    grid->addWidget(d->matterEdit, 3, 6, 1, 1);
    grid->addWidget(new QLabel(":", this), 3, 7, 1, 1);
    grid->addWidget(d->detailLabel, 2, 8, 1, 1);
    grid->addWidget(d->detailEdit, 3, 8, 1, 1);
    grid->addWidget(d->subjectsBox, 4, 0, 5, 9);
    grid->addWidget(d->addSubjectButton, 4, 9, 1, 1);
    grid->addWidget(d->delSubjectButton, 5, 9, 1, 1);
    grid->addWidget(d->repSubjectButton, 6, 9, 1, 1);
    grid->addWidget(note, 7, 9, 1, 1);
    grid->setColumnStretch(9, 1);                     
    grid->setRowStretch(8, 10);  
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
            d->iprEdit, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->iprLabel, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->refEdit, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->refLabel, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->nameEdit, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->nameLabel, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->matterEdit, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->matterLabel, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->detailEdit, SLOT(setEnabled(bool)));

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            d->detailLabel, SLOT(setEnabled(bool)));

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

QString IPTCSubjects::buildSubject() const
{
    QString subject = d->iprEdit->text();
    subject.append(":");
    subject.append(d->refEdit->text());
    subject.append(":");
    subject.append(d->nameEdit->text());
    subject.append(":");
    subject.append(d->matterEdit->text());
    subject.append(":");
    subject.append(d->detailEdit->text());
    return subject;
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
    QString newSubject = buildSubject();
    if (newSubject.isEmpty()) return;

    if (!d->subjectsBox->selectedItems().isEmpty())
    {
        d->subjectsBox->selectedItems()[0]->setText(newSubject);
        d->iprEdit->clear();
        d->refEdit->clear();
        d->nameEdit->clear();
        d->matterEdit->clear();
        d->detailEdit->clear();
    }
}

void IPTCSubjects::slotSubjectSelectionChanged()
{
    if (!d->subjectsBox->selectedItems().isEmpty())
    {
        QString subject = d->subjectsBox->selectedItems()[0]->text();
        d->iprEdit->setText(subject.section(':', 0, 0));
        d->refEdit->setText(subject.section(':', 1, 1));
        d->nameEdit->setText(subject.section(':', 2, 2));
        d->matterEdit->setText(subject.section(':', 3, 3));
        d->detailEdit->setText(subject.section(':', 4, 4));
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
    QString newSubject = buildSubject();
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
        d->iprEdit->clear();
        d->refEdit->clear();
        d->nameEdit->clear();
        d->matterEdit->clear();
        d->detailEdit->clear();
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
    d->iprEdit->setEnabled(d->subjectsCheck->isChecked());
    d->refEdit->setEnabled(d->subjectsCheck->isChecked());
    d->nameEdit->setEnabled(d->subjectsCheck->isChecked());
    d->matterEdit->setEnabled(d->subjectsCheck->isChecked());
    d->detailEdit->setEnabled(d->subjectsCheck->isChecked());
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
