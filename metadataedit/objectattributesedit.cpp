/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-08
 * Description : a widget to edit Application2 ObjectAttribute
 *               Iptc tag.
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "objectattributesedit.h"
#include "objectattributesedit.moc"

// Qt includes

#include <QCheckBox>
#include <QPushButton>
#include <QValidator>
#include <QGridLayout>

// KDE includes

#include <kcombobox.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klistwidget.h>
#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/squeezedcombobox.h>

// Local includes

#include "metadatacheckbox.h"

using namespace KDcrawIface;

namespace KIPIMetadataEditPlugin
{

class ObjectAttributesEditPriv
{
public:

    ObjectAttributesEditPriv()
    {
        addValueButton = 0;
        delValueButton = 0;
        repValueButton = 0;
        valueBox       = 0;
        valueCheck     = 0;
        valueEdit      = 0;
        dataList       = 0;
    }

    QStringList                    oldValues;

    QPushButton                   *addValueButton;
    QPushButton                   *delValueButton;
    QPushButton                   *repValueButton;

    KLineEdit                     *valueEdit;

    KListWidget                   *valueBox;

    MetadataCheckBox              *valueCheck;

    SqueezedComboBox              *dataList;
};

ObjectAttributesEdit::ObjectAttributesEdit(QWidget* parent, bool ascii, int size)
                    : QWidget(parent), d(new ObjectAttributesEditPriv)
{
    QGridLayout *grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->valueCheck = new MetadataCheckBox(i18n("Attribute:"), this);

    d->addValueButton = new QPushButton(this);
    d->delValueButton = new QPushButton(this);
    d->repValueButton = new QPushButton(this);
    d->addValueButton->setIcon(SmallIcon("list-add"));
    d->delValueButton->setIcon(SmallIcon("edit-delete"));
    d->repValueButton->setIcon(SmallIcon("view-refresh"));
    d->addValueButton->setWhatsThis(i18n("Add a new value to the list"));
    d->delValueButton->setWhatsThis(i18n("Remove the current selected value from the list"));
    d->repValueButton->setWhatsThis(i18n("Replace the current selected value from the list"));
    d->delValueButton->setEnabled(false);
    d->repValueButton->setEnabled(false);

    d->valueBox = new KListWidget(this);
    d->valueBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored);
    d->valueBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // --------------------------------------------------------

    d->dataList = new SqueezedComboBox(this);
    d->dataList->model()->sort(0);
    d->dataList->setWhatsThis(i18n("Select here the editorial attribute of content."));
    d->dataList->addSqueezedItem(QString("001 - ") + i18n("Current"));
    d->dataList->addSqueezedItem(QString("002 - ") + i18n("Analysis"));
    d->dataList->addSqueezedItem(QString("003 - ") + i18n("Archive material"));
    d->dataList->addSqueezedItem(QString("004 - ") + i18n("Background"));
    d->dataList->addSqueezedItem(QString("005 - ") + i18n("Feature"));
    d->dataList->addSqueezedItem(QString("006 - ") + i18n("Forecast"));
    d->dataList->addSqueezedItem(QString("007 - ") + i18n("History"));
    d->dataList->addSqueezedItem(QString("008 - ") + i18n("Obituary"));
    d->dataList->addSqueezedItem(QString("009 - ") + i18n("Opinion"));
    d->dataList->addSqueezedItem(QString("010 - ") + i18n("Polls & Surveys"));
    d->dataList->addSqueezedItem(QString("011 - ") + i18nc("Content type", "Profile"));
    d->dataList->addSqueezedItem(QString("012 - ") + i18n("Results Listings & Table"));
    d->dataList->addSqueezedItem(QString("013 - ") + i18n("Side bar & Supporting information"));
    d->dataList->addSqueezedItem(QString("014 - ") + i18n("Summary"));
    d->dataList->addSqueezedItem(QString("015 - ") + i18n("Transcript & Verbatim"));
    d->dataList->addSqueezedItem(QString("016 - ") + i18n("Interview"));
    d->dataList->addSqueezedItem(QString("017 - ") + i18n("From the Scene"));
    d->dataList->addSqueezedItem(QString("018 - ") + i18n("Retrospective"));
    d->dataList->addSqueezedItem(QString("019 - ") + i18n("Statistics"));
    d->dataList->addSqueezedItem(QString("020 - ") + i18n("Update"));
    d->dataList->addSqueezedItem(QString("021 - ") + i18n("Wrap-up"));
    d->dataList->addSqueezedItem(QString("022 - ") + i18n("Press Release"));

    // --------------------------------------------------------

    d->valueEdit = new KLineEdit(this);
    d->valueEdit->setClearButtonShown(true);
    QString whatsThis = i18n("Set here the editorial attribute description of "
                             "content. This field is limited to 64 ASCII characters.");

    if (ascii || size != -1)
    {
        whatsThis.append(i18n(" This field is limited to:"));
    }

    if (ascii)
    {
        d->valueEdit->setValidator(asciiValidator);
        whatsThis.append(i18n("<p>Printable ASCII characters.</p>"));
    }

    if (size != -1)
    {
        d->valueEdit->setMaxLength(size);
        whatsThis.append(i18np("<p>1 character.</p>","<p>%1 characters.</p>", size));
    }

    d->valueEdit->setWhatsThis(whatsThis);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->valueCheck,      0, 0, 1, 1);
    grid->addWidget(d->addValueButton,  0, 1, 1, 1);
    grid->addWidget(d->delValueButton,  0, 2, 1, 1);
    grid->addWidget(d->repValueButton,  0, 3, 1, 1);
    grid->addWidget(d->valueBox,        0, 4, 4, 1);
    grid->addWidget(d->dataList,        1, 0, 1, 4);
    grid->addWidget(d->valueEdit,       2, 0, 1, 4);
    grid->setRowStretch(3, 10);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(4, 100);
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
            d->dataList, SLOT(setEnabled(bool)));

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

ObjectAttributesEdit::~ObjectAttributesEdit()
{
    delete d;
}

void ObjectAttributesEdit::slotDeleteValue()
{
    QListWidgetItem *item = d->valueBox->currentItem();
    if (!item) return;
    d->valueBox->takeItem(d->valueBox->row(item));
    delete item;
}

void ObjectAttributesEdit::slotReplaceValue()
{
    QString newValue = d->dataList->itemHighlighted().left(3);
    newValue.append(QString(":%1").arg(d->valueEdit->text()));

    if (!d->valueBox->selectedItems().isEmpty())
        d->valueBox->selectedItems()[0]->setText(newValue);
}

void ObjectAttributesEdit::slotSelectionChanged()
{
    if (!d->valueBox->selectedItems().isEmpty())
    {
        bool ok   = false;
        int index = d->valueBox->selectedItems()[0]->text().section(":", 0, 0).toInt(&ok);
        if (ok)
        {
            d->dataList->setCurrentIndex(index-1);
            d->valueEdit->setText(d->valueBox->selectedItems()[0]->text().section(":", -1));
            d->delValueButton->setEnabled(true);
            d->repValueButton->setEnabled(true);
            return;
        }
    }

    d->delValueButton->setEnabled(false);
    d->repValueButton->setEnabled(false);
}

void ObjectAttributesEdit::slotAddValue()
{
    QString newValue = d->dataList->itemHighlighted().left(3);
    newValue.append(QString(":%1").arg(d->valueEdit->text()));

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
        d->valueBox->insertItem(d->valueBox->count(), newValue);
}

void ObjectAttributesEdit::setValues(const QStringList& values)
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
    d->dataList->setEnabled(d->valueCheck->isChecked());
    d->valueBox->setEnabled(d->valueCheck->isChecked());
    d->addValueButton->setEnabled(d->valueCheck->isChecked());
    d->delValueButton->setEnabled(d->valueCheck->isChecked());

    blockSignals(false);
}

bool ObjectAttributesEdit::getValues(QStringList& oldValues, QStringList& newValues)
{
    oldValues = d->oldValues;

    newValues.clear();
    for (int i = 0 ; i < d->valueBox->count(); i++)
    {
        QListWidgetItem *item = d->valueBox->item(i);
        newValues.append(item->text());
    }

    return d->valueCheck->isChecked();
}

void ObjectAttributesEdit::setValid(bool v)
{
    d->valueCheck->setValid(v);
}

bool ObjectAttributesEdit::isValid() const
{
    return d->valueCheck->isValid();
}

}  // namespace KIPIMetadataEditPlugin
