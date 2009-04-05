/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-16
 * Description : XMP categories settings page.
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

#include "xmpcategories.h"
#include "xmpcategories.moc"

// Qt includes

#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>

// KDE includes

#include <kdialog.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klistwidget.h>
#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

namespace KIPIMetadataEditPlugin
{

class XMPCategoriesPriv
{
public:

    XMPCategoriesPriv()
    {
        addSubCategoryButton = 0;
        delSubCategoryButton = 0;
        repSubCategoryButton = 0;
        subCategoriesBox     = 0;
        subCategoriesCheck   = 0;
        categoryCheck        = 0;
        categoryEdit         = 0;
        subCategoryEdit      = 0;
    }

    QStringList  oldSubCategories;

    QPushButton *addSubCategoryButton;
    QPushButton *delSubCategoryButton;
    QPushButton *repSubCategoryButton;

    QCheckBox   *subCategoriesCheck;
    QCheckBox   *categoryCheck;

    KLineEdit   *categoryEdit;
    KLineEdit   *subCategoryEdit;

    KListWidget *subCategoriesBox;
};

XMPCategories::XMPCategories(QWidget* parent)
             : QWidget(parent), d(new XMPCategoriesPriv)
{
    QGridLayout *grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->categoryCheck = new QCheckBox(i18n("Identify subject of content (3 chars max):"), this);
    d->categoryEdit  = new KLineEdit(this);
    d->categoryEdit->setClearButtonShown(true);
    d->categoryEdit->setMaxLength(3);
    d->categoryEdit->setWhatsThis(i18n("Set here the category of content. This field is limited "
                                       "to 3 characters."));

    d->subCategoriesCheck = new QCheckBox(i18n("Supplemental categories:"), this);

    d->subCategoryEdit = new KLineEdit(this);
    d->subCategoryEdit->setClearButtonShown(true);
    d->subCategoryEdit->setWhatsThis(i18n("Enter here a new supplemental category of content."));

    d->subCategoriesBox = new KListWidget(this);
    d->subCategoriesBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    d->addSubCategoryButton = new QPushButton( i18n("&Add"), this);
    d->delSubCategoryButton = new QPushButton( i18n("&Delete"), this);
    d->repSubCategoryButton = new QPushButton( i18n("&Replace"), this);
    d->addSubCategoryButton->setIcon(SmallIcon("list-add"));
    d->delSubCategoryButton->setIcon(SmallIcon("edit-delete"));
    d->repSubCategoryButton->setIcon(SmallIcon("view-refresh"));
    d->delSubCategoryButton->setEnabled(false);
    d->repSubCategoryButton->setEnabled(false);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->categoryCheck,           0, 0, 1, 2);
    grid->addWidget(d->categoryEdit,            0, 2, 1, 1);
    grid->addWidget(d->subCategoriesCheck,      1, 0, 1, 3);
    grid->addWidget(d->subCategoryEdit,         2, 0, 1, 3);
    grid->addWidget(d->subCategoriesBox,        3, 0, 5, 3);
    grid->addWidget(d->addSubCategoryButton,    3, 3, 1, 1);
    grid->addWidget(d->delSubCategoryButton,    4, 3, 1, 1);
    grid->addWidget(d->repSubCategoryButton,    5, 3, 1, 1);
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(6, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->categoryEdit, SLOT(setEnabled(bool)));

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->subCategoriesBox, SLOT(setEnabled(bool)));

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->subCategoriesCheck, SLOT(setEnabled(bool)));

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->subCategoryEdit, SLOT(setEnabled(bool)));

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->addSubCategoryButton, SLOT(setEnabled(bool)));

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->delSubCategoryButton, SLOT(setEnabled(bool)));

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->repSubCategoryButton, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->subCategoryEdit, SLOT(setEnabled(bool)));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->subCategoriesBox, SLOT(setEnabled(bool)));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->addSubCategoryButton, SLOT(setEnabled(bool)));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->delSubCategoryButton, SLOT(setEnabled(bool)));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->repSubCategoryButton, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->subCategoriesBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotCategorySelectionChanged()));

    connect(d->addSubCategoryButton, SIGNAL(clicked()),
            this, SLOT(slotAddCategory()));

    connect(d->delSubCategoryButton, SIGNAL(clicked()),
            this, SLOT(slotDelCategory()));

    connect(d->repSubCategoryButton, SIGNAL(clicked()),
            this, SLOT(slotRepCategory()));

    // --------------------------------------------------------

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addSubCategoryButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->delSubCategoryButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repSubCategoryButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->categoryEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

XMPCategories::~XMPCategories()
{
    delete d;
}

void XMPCategories::slotDelCategory()
{
    QListWidgetItem *item = d->subCategoriesBox->currentItem();
    if (!item) return;
    d->subCategoriesBox->takeItem(d->subCategoriesBox->row(item));
    delete item;
}

void XMPCategories::slotRepCategory()
{
    QString newCategory = d->subCategoryEdit->text();
    if (newCategory.isEmpty()) return;

    if (!d->subCategoriesBox->selectedItems().isEmpty())
    {
        d->subCategoriesBox->selectedItems()[0]->setText(newCategory);
        d->subCategoryEdit->clear();
    }
}

void XMPCategories::slotCategorySelectionChanged()
{
    if (!d->subCategoriesBox->selectedItems().isEmpty())
    {
        d->subCategoryEdit->setText(d->subCategoriesBox->selectedItems()[0]->text());
        d->delSubCategoryButton->setEnabled(true);
        d->repSubCategoryButton->setEnabled(true);
    }
    else
    {
        d->delSubCategoryButton->setEnabled(false);
        d->repSubCategoryButton->setEnabled(false);
    }
}

void XMPCategories::slotAddCategory()
{
    QString newCategory = d->subCategoryEdit->text();
    if (newCategory.isEmpty()) return;

    bool found = false;
    for (int i = 0 ; i < d->subCategoriesBox->count(); i++)
    {
        QListWidgetItem *item = d->subCategoriesBox->item(i);
        if (newCategory == item->text())
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->subCategoriesBox->insertItem(d->subCategoriesBox->count(), newCategory);
        d->subCategoryEdit->clear();
    }
}

void XMPCategories::readMetadata(QByteArray& xmpData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);
    QString data;

    // In first we handle all sub-categories.

    d->subCategoriesBox->clear();
    d->subCategoriesCheck->setChecked(false);
    d->oldSubCategories = exiv2Iface.getXmpSubCategories();
    if (!d->oldSubCategories.isEmpty())
    {
        d->subCategoriesBox->insertItems(0, d->oldSubCategories);
        d->subCategoriesCheck->setChecked(true);
    }

    // And in second, the main category because all sub-categories status depand of this one.

    d->categoryEdit->clear();
    d->categoryCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.photoshop.Category", false);
    if (!data.isNull())
    {
        d->categoryEdit->setText(data);
        d->categoryCheck->setChecked(true);
    }
    d->categoryEdit->setEnabled(d->categoryCheck->isChecked());
    d->subCategoriesCheck->setEnabled(d->categoryCheck->isChecked());
    d->subCategoryEdit->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->subCategoriesBox->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->addSubCategoryButton->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->delSubCategoryButton->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());

    blockSignals(false);
}

void XMPCategories::applyMetadata(QByteArray& xmpData)
{
    QStringList newCategories;
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);

    if (d->categoryCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.photoshop.Category", d->categoryEdit->text());
    else
        exiv2Iface.removeXmpTag("Xmp.photoshop.Category");

    for (int i = 0 ; i < d->subCategoriesBox->count(); i++)
    {
        QListWidgetItem *item = d->subCategoriesBox->item(i);
        newCategories.append(item->text());
    }

    // We remove in first all existing sub-categories.
    exiv2Iface.removeXmpTag("Xmp.photoshop.SupplementalCategories");

    // And add new list if necessary.
    if (d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked())
        exiv2Iface.setXmpSubCategories(newCategories);

    xmpData = exiv2Iface.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
