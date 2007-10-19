/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-18
 * Description : a widget to edit a tag with multiple alternative
 *               language string entries.
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
#include <QPushButton>

// KDE includes.

#include <klistwidget.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdialog.h>
#include <ktextedit.h>
#include <klanguagebutton.h>
#include <kiconloader.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "metadatacheckbox.h"
#include "altlangstringedit.h"
#include "altlangstringedit.moc"

namespace KIPIMetadataEditPlugin
{

class AltLangStringsEditPriv
{
public:

    AltLangStringsEditPriv()
    {
        addValueButton = 0;
        delValueButton = 0;
        repValueButton = 0;
        valueBox       = 0;
        valueCheck     = 0;
        valueBox       = 0;
        languageBtn    = 0;
    }

    AltLangDataList   oldValues;

    QPushButton      *addValueButton;
    QPushButton      *delValueButton;
    QPushButton      *repValueButton;

    KListWidget      *valueBox;

    KLanguageButton  *languageBtn;

    KTextEdit        *valueEdit;

    MetadataCheckBox *valueCheck;
};

AltLangStringsEdit::AltLangStringsEdit(QWidget* parent, const QString& title, const QString& desc)
                  : QWidget(parent)
{
    d = new AltLangStringsEditPriv;

    QGridLayout *grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->valueCheck = new MetadataCheckBox(title, this);  

    d->addValueButton = new QPushButton(this);
    d->delValueButton = new QPushButton(this);
    d->repValueButton = new QPushButton(this);
    d->addValueButton->setIcon(SmallIcon("edit-add"));
    d->delValueButton->setIcon(SmallIcon("edit-delete"));
    d->repValueButton->setIcon(SmallIcon("view-refresh"));
    d->addValueButton->setWhatsThis(i18n("Add a new value to the list"));
    d->delValueButton->setWhatsThis(i18n("Remove the current selected value from the list"));
    d->repValueButton->setWhatsThis(i18n("Replace the current selected value from the list"));
    d->delValueButton->setEnabled(false);
    d->repValueButton->setEnabled(false);

    d->valueBox  = new KListWidget(this);
    d->valueBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->valueBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    d->languageBtn   = new KLanguageButton(this);
    d->languageBtn->insertLanguage("x-default", "Default");
    d->languageBtn->insertSeparator();
    d->languageBtn->setMaximumHeight( fontMetrics().height()+2 );
    QStringList list = KGlobal::locale()->allLanguagesList();
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
        d->languageBtn->insertLanguage(*it);

    d->valueEdit = new KTextEdit(this);
    d->valueEdit->setWhatsThis(desc);
    d->valueEdit->setMaximumHeight( fontMetrics().height()*3 ); // 3 lines used to edit text.

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->valueCheck, 0, 0, 1, 1 );
    grid->addWidget(d->addValueButton, 0, 2, 1, 1);
    grid->addWidget(d->delValueButton, 0, 3, 1, 1);
    grid->addWidget(d->repValueButton, 0, 4, 1, 1);
    grid->addWidget(new QLabel(i18n("Language:")), 1, 0, 1, 1);
    grid->addWidget(d->languageBtn, 1, 1, 1, 4);
    grid->addWidget(d->valueEdit, 2, 0, 1, 5);
    grid->addWidget(d->valueBox, 0, 5, 3, 1);
    grid->setColumnStretch(1, 10);   
    grid->setColumnStretch(5, 100);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());    
                                         
    // --------------------------------------------------------

    connect(d->valueBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSelectionChanged()));
    
    connect(d->addValueButton, SIGNAL(clicked()),
            this, SLOT(slotAddValue()));
    
    connect(d->delValueButton, SIGNAL(clicked()),
            this, SLOT(slotDeleteValue()));

    connect(d->repValueButton, SIGNAL(clicked()),
            this, SLOT(slotReplaceValue()));

    // --------------------------------------------------------

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalToggled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->valueEdit, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->addValueButton, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->delValueButton, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->repValueButton, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->valueBox, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addValueButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
    
    connect(d->delValueButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repValueButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
}

AltLangStringsEdit::~AltLangStringsEdit()
{
    delete d;
}

void AltLangStringsEdit::setValid(bool v) 
{
    d->valueCheck->setValid(v); 
}

bool AltLangStringsEdit::isValid() const 
{
    return d->valueCheck->isValid(); 
}

void AltLangStringsEdit::slotDeleteValue()
{
/*    QListWidgetItem *item = d->valueBox->currentItem();
    if (!item) return;
    d->valueBox->takeItem(d->valueBox->row(item));
    delete item;*/
}

void AltLangStringsEdit::slotReplaceValue()
{
/*    QString newValue = d->valueEdit->text();
    if (newValue.isEmpty()) return;

    if (!d->valueBox->selectedItems().isEmpty())
    {
        d->valueBox->selectedItems()[0]->setText(newValue);
        d->valueEdit->clear();
    }*/
}

void AltLangStringsEdit::slotSelectionChanged()
{
/*    if (!d->valueBox->selectedItems().isEmpty())
    {
        d->valueEdit->setText(d->valueBox->selectedItems()[0]->text());
        d->delValueButton->setEnabled(true);
        d->repValueButton->setEnabled(true);
    }
    else
    {
        d->delValueButton->setEnabled(false);
        d->repValueButton->setEnabled(false);
    }*/
}

void AltLangStringsEdit::slotAddValue()
{
    QString lang = d->languageBtn->current();
    QString text = d->valueEdit->toPlainText();
    if (text.isEmpty()) return;
    
    QString newValue = QString("[%1] %2").arg(lang).arg(text);

    bool found = false;
    for (int i = 0 ; i < d->valueBox->count(); i++)
    {
        QListWidgetItem *item = d->valueBox->item(i);
        if (newValue == item->text()) 
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->valueBox->insertItem(d->valueBox->count(), newValue);
        d->valueEdit->clear();
    }
}

void AltLangStringsEdit::setValues(const AltLangDataList& values)
{
    blockSignals(true);
    d->oldValues = values;

    d->valueBox->clear();
    d->valueCheck->setChecked(false);
    if (!d->oldValues.isEmpty())
    {
        for (AltLangDataList::Iterator it = d->oldValues.begin(); it != d->oldValues.end(); ++it)
        {
            QString newValue = QString("[%1] %2").arg((*it).lang).arg((*it).text);
            d->valueBox->insertItem(0, newValue);    
        }

        d->valueCheck->setChecked(true);
    }
    d->valueEdit->setEnabled(d->valueCheck->isChecked());
    d->valueBox->setEnabled(d->valueCheck->isChecked());
    d->addValueButton->setEnabled(d->valueCheck->isChecked());
    d->delValueButton->setEnabled(d->valueCheck->isChecked());

    blockSignals(false);
}

bool AltLangStringsEdit::getValues(AltLangDataList& oldValues, AltLangDataList& newValues)
{
    oldValues = d->oldValues;

    newValues.clear();
    for (int i = 0 ; i < d->valueBox->count(); i++)
    {
        QListWidgetItem *item = d->valueBox->item(i);
        QString lang = item->text().left(item->text().indexOf("] ")).right(1);
        QString text = item->text().right(item->text().indexOf("] ")+2);
        newValues.append(AltLangData(lang, text));
    }

    return d->valueCheck->isChecked();
}

}  // namespace KIPIMetadataEditPlugin
