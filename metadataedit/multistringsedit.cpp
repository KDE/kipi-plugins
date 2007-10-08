/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-08
 * Description : a widget to edit a tag with multiple string entries.
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

#include "multistringsedit.h"
#include "multistringsedit.moc"

namespace KIPIMetadataEditPlugin
{

class MultiStringsEditPriv
{
public:

    MultiStringsEditPriv()
    {
        addValueButton = 0;
        delValueButton = 0;
        repValueButton = 0;
        valueBox       = 0;
        valueCheck     = 0;
        valueEdit      = 0;
    }

    QStringList  oldValues;

    QPushButton *addValueButton;
    QPushButton *delValueButton;
    QPushButton *repValueButton;

    QCheckBox   *valueCheck;

    KLineEdit   *valueEdit;

    KListWidget *valueBox;
};

MultiStringsEdit::MultiStringsEdit(QWidget* parent, const QString& title,
                                   const QString& desc, bool ascii, int size)
                : QWidget(parent)
{
    d = new MultiStringsEditPriv;
    QGridLayout *grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->valueCheck = new QCheckBox(title, this);    

    d->valueEdit  = new KLineEdit(this);
    d->valueEdit->setClearButtonShown(true);
    QString whatsThis = desc;

    if (ascii || size != -1)
    {
        whatsThis.append(i18n("This field is limited to:"));
    }

    if (ascii)
    {
        d->valueEdit->setValidator(asciiValidator);
        whatsThis.append(i18n("<p>Printable ASCII characters set.</p>"));
    }

    if (size != -1)
    {
        d->valueEdit->setMaxLength(size);
        whatsThis.append(i18n("<p>Size of %1 characters.</p>", size));
    }

    d->valueEdit->setWhatsThis(whatsThis);

    d->valueBox   = new KListWidget(this);
    d->valueBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->valueBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
    
    d->addValueButton = new QPushButton(this);
    d->delValueButton = new QPushButton(this);
    d->repValueButton = new QPushButton(this);
    d->addValueButton->setIcon(SmallIcon("edit-add"));
    d->delValueButton->setIcon(SmallIcon("edit-delete"));
    d->repValueButton->setIcon(SmallIcon("view-refresh"));
    d->delValueButton->setEnabled(false);
    d->repValueButton->setEnabled(false);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->valueCheck, 0, 0, 1, 1 );
    grid->addWidget(d->addValueButton, 0, 1, 1, 1);
    grid->addWidget(d->delValueButton, 0, 2, 1, 1);
    grid->addWidget(d->repValueButton, 0, 3, 1, 1);
    grid->addWidget(d->valueEdit, 1, 0, 1, 4);
    grid->addWidget(d->valueBox, 0, 4, 2, 1);
    grid->setColumnStretch(0, 10);                     
    grid->setColumnStretch(4, 10);                     
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

MultiStringsEdit::~MultiStringsEdit()
{
    delete d;
}

void MultiStringsEdit::slotDeleteValue()
{
    QListWidgetItem *item = d->valueBox->currentItem();
    if (!item) return;
    d->valueBox->takeItem(d->valueBox->row(item));
    delete item;
}

void MultiStringsEdit::slotReplaceValue()
{
    QString newValue = d->valueEdit->text();
    if (newValue.isEmpty()) return;

    if (!d->valueBox->selectedItems().isEmpty())
    {
        d->valueBox->selectedItems()[0]->setText(newValue);
        d->valueEdit->clear();
    }
}

void MultiStringsEdit::slotSelectionChanged()
{
    if (!d->valueBox->selectedItems().isEmpty())
    {
        d->valueEdit->setText(d->valueBox->selectedItems()[0]->text());
        d->delValueButton->setEnabled(true);
        d->repValueButton->setEnabled(true);
    }
    else
    {
        d->delValueButton->setEnabled(false);
        d->repValueButton->setEnabled(false);
    }
}

void MultiStringsEdit::slotAddValue()
{
    QString newValue = d->valueEdit->text();
    if (newValue.isEmpty()) return;

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

void MultiStringsEdit::setValues(const QStringList& values)
{
    blockSignals(true);
    d->oldValues = values;

    d->valueBox->clear();
    d->valueCheck->setChecked(false);
    if (!d->oldValues.isEmpty())
    {
        d->valueBox->insertItems(0, d->oldValues);
        d->valueCheck->setChecked(true);
    }
    d->valueEdit->setEnabled(d->valueCheck->isChecked());
    d->valueBox->setEnabled(d->valueCheck->isChecked());
    d->addValueButton->setEnabled(d->valueCheck->isChecked());
    d->delValueButton->setEnabled(d->valueCheck->isChecked());

    blockSignals(false);
}

bool MultiStringsEdit::getValues(QStringList& newValues)
{
    for (int i = 0 ; i < d->valueBox->count(); i++)
    {
        QListWidgetItem *item = d->valueBox->item(i);
        newValues.append(item->text());
    }
    return d->valueCheck->isChecked();
}

}  // namespace KIPIMetadataEditPlugin
