/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-15
 * Description : IPTC categories settings page.
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
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

// Local includes.

#include "exiv2iface.h"
#include "iptccategories.h"
#include "iptccategories.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCCategoriesPriv
{
public:

    IPTCCategoriesPriv()
    {
        addSubCategoryButton = 0;
        delSubCategoryButton = 0;
        subCategoriesBox     = 0;
        subCategoriesCheck   = 0;
        categoryCheck        = 0;
        categoryEdit         = 0;
        subCategoryEdit      = 0;
    }

    QStringList  oldSubCategories;

    QPushButton *addSubCategoryButton;
    QPushButton *delSubCategoryButton;

    QCheckBox   *subCategoriesCheck;
    QCheckBox   *categoryCheck;

    KLineEdit   *categoryEdit;
    KLineEdit   *subCategoryEdit;

    KListBox    *subCategoriesBox;
};

IPTCCategories::IPTCCategories(QWidget* parent)
              : QWidget(parent)
{
    d = new IPTCCategoriesPriv;
    QGridLayout *grid = new QGridLayout(parent, 6, 1, 0, KDialog::spacingHint());
    grid->setAlignment( Qt::AlignTop );

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->categoryCheck = new QCheckBox(i18n("Identify subject of content (3 chars max):"), parent);    
    d->categoryEdit  = new KLineEdit(parent);
    d->categoryEdit->setValidator(asciiValidator);
    d->categoryEdit->setMaxLength(3);
    QWhatsThis::add(d->categoryEdit, i18n("<p>Set here the category of content. This field is limited "
                                         "to 3 ASCII characters."));

    d->subCategoriesCheck = new QCheckBox(i18n("Supplemental categories:"), parent);    

    d->subCategoryEdit = new KLineEdit(parent);
    d->subCategoryEdit->setValidator(asciiValidator);
    d->subCategoryEdit->setMaxLength(32);
    QWhatsThis::add(d->subCategoryEdit, i18n("<p>Enter here a new supplemental category of content. "
                    "This field is limited to 32 ASCII characters."));

    d->subCategoriesBox = new KListBox(parent);
    d->subCategoriesBox->setVScrollBarMode(QScrollView::AlwaysOn);
    
    d->addSubCategoryButton = new QPushButton( i18n("&Add"), parent);
    d->delSubCategoryButton = new QPushButton( i18n("&Delete"), parent);
    d->delSubCategoryButton->setEnabled(false);

    grid->addMultiCellWidget(d->categoryCheck, 0, 0, 0, 1);
    grid->addMultiCellWidget(d->categoryEdit, 0, 0, 1, 1);
    grid->addMultiCellWidget(d->subCategoriesCheck, 1, 1, 0, 1);
    grid->addMultiCellWidget(d->subCategoryEdit, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->subCategoriesBox, 3, 6, 0, 0);
    grid->addMultiCellWidget(d->addSubCategoryButton, 3, 3, 1, 1);
    grid->addMultiCellWidget(d->delSubCategoryButton, 4, 4, 1, 1);

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: IPTC text tags only support printable "
                                       "ASCII characters set.</b>"), parent);
    grid->addMultiCellWidget(iptcNote, 5, 5, 1, 1);
    grid->setColStretch(0, 10);                     
    grid->setRowStretch(6, 10);      
                                         
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
            d->subCategoriesBox, SLOT(setEnabled(bool)));

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->addSubCategoryButton, SLOT(setEnabled(bool)));

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            d->delSubCategoryButton, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->subCategoryEdit, SLOT(setEnabled(bool)));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->subCategoriesBox, SLOT(setEnabled(bool)));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->addSubCategoryButton, SLOT(setEnabled(bool)));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            d->delSubCategoryButton, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->subCategoriesBox, SIGNAL(selectionChanged()),
            this, SLOT(slotCategorySelectionChanged()));
    
    connect(d->addSubCategoryButton, SIGNAL(clicked()),
            this, SLOT(slotAddCategory()));
    
    connect(d->delSubCategoryButton, SIGNAL(clicked()),
            this, SLOT(slotDelCategory()));

    // --------------------------------------------------------

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addSubCategoryButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
    
    connect(d->delSubCategoryButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->categoryEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

IPTCCategories::~IPTCCategories()
{
    delete d;
}

void IPTCCategories::slotDelCategory()
{
    int index = d->subCategoriesBox->currentItem();
    if (index == -1)
        return;

    QListBoxItem* item = d->subCategoriesBox->item(index);
    if (!item) return;
    delete item;
}

void IPTCCategories::slotCategorySelectionChanged()
{
    if (d->subCategoriesBox->currentItem() != -1)
        d->delSubCategoryButton->setEnabled(true);
    else
        d->delSubCategoryButton->setEnabled(false);
}

void IPTCCategories::slotAddCategory()
{
    QString newCategory = d->subCategoryEdit->text();
    if (newCategory.isEmpty()) return;

    bool found = false;
    for (QListBoxItem *item = d->subCategoriesBox->firstItem();
         item; item = item->next()) 
    {
        if (newCategory == item->text()) 
        {
            found = true;
            break;
        }
    }

    if (!found)
        d->subCategoriesBox->insertItem(newCategory);
}

void IPTCCategories::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QString data;

    data = exiv2Iface.getIptcTagString("Iptc.Application2.Category", false);    
    if (!data.isNull())
    {
        d->categoryEdit->setText(data);
        d->categoryCheck->setChecked(true);
    }
    d->categoryEdit->setEnabled(d->categoryCheck->isChecked());
    d->subCategoriesCheck->setEnabled(d->categoryCheck->isChecked());

    d->oldSubCategories = exiv2Iface.getImageSubCategories();

    if (!d->oldSubCategories.isEmpty())
    {
        d->subCategoriesBox->insertStringList(d->oldSubCategories);
        d->subCategoriesCheck->setChecked(true);
    }

    d->subCategoryEdit->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->subCategoriesBox->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->addSubCategoryButton->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->delSubCategoryButton->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());

    blockSignals(false);
}

void IPTCCategories::applyMetadata(QByteArray& iptcData)
{
    QStringList newCategories;    
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->categoryCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.Category", d->categoryEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.Category");

    for (QListBoxItem *item = d->subCategoriesBox->firstItem();
         item; item = item->next()) 
        newCategories.append(item->text());

    if (d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked())
        exiv2Iface.setImageSubCategories(d->oldSubCategories, newCategories);
    else
        exiv2Iface.setImageSubCategories(d->oldSubCategories, QStringList());

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

