/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-11-01
 * Description : a widget that holds all storage settings
 *
 * Copyright (C) 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "storagesettingsbox.h"
#include "storagesettingsbox.moc"

// Qt includes.

#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>

// KDE includes.

#include <klineedit.h>
#include <klocale.h>

// Local includes.

#include "removeredeyeswindow.h"

namespace KIPIRemoveRedEyesPlugin
{

class StorageSettingsBoxPriv
{
public:

    StorageSettingsBoxPriv()
    {
        storageGroup                = 0;
        prefixLineEdit              = 0;
        subfolderLineEdit           = 0;
    }

    QButtonGroup*       storageGroup;

    KLineEdit*          prefixLineEdit;
    KLineEdit*          subfolderLineEdit;
};

StorageSettingsBox::StorageSettingsBox(QWidget* parent)
                  : QGroupBox(parent),
                    d(new StorageSettingsBoxPriv)
{
    setTitle(i18n("Storage Settings"));
    d->storageGroup = new QButtonGroup(this);
    d->storageGroup->setExclusive(true);

    d->subfolderLineEdit = new KLineEdit;
    d->subfolderLineEdit->setToolTip(i18n("Enter the name of the subfolder here."));

    QRadioButton* subfolderMode = new QRadioButton(i18n("Save files in subfolder"));
    subfolderMode->setToolTip(i18n("If checked, the corrected files will be saved "
            "in a subfolder under the current image album path."));
    d->storageGroup->addButton(subfolderMode, Subfolder);

    QRadioButton* prefixMode = new QRadioButton(i18n("Append custom string to files"));
    prefixMode->setToolTip(i18n("If checked, a custom string will be added " "to the corrected file."));
    d->storageGroup->addButton(prefixMode, Prefix);

    QRadioButton* overwriteMode = new QRadioButton(i18n("Overwrite original files"));
    overwriteMode->setToolTip(i18n("If checked, all original images will be replaced."));
    d->storageGroup->addButton(overwriteMode, Overwrite);

    d->prefixLineEdit = new KLineEdit;
    d->prefixLineEdit->setToolTip(i18n("Enter the name of the prefix here..."));

    QGridLayout* correctionGroupLayout = new QGridLayout;
    correctionGroupLayout->addWidget(subfolderMode,         0, 0, 1, 1);
    correctionGroupLayout->addWidget(d->subfolderLineEdit,  0, 2, 1, 1);
    correctionGroupLayout->addWidget(prefixMode,            1, 0, 1, 1);
    correctionGroupLayout->addWidget(d->prefixLineEdit,     1, 2, 1, 1);
    correctionGroupLayout->addWidget(overwriteMode,         2, 0, 1,-1);
    setLayout(correctionGroupLayout);

    setStorageMode(Prefix);

    connect(d->storageGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonClicked(int)));
//
//    connect(d->subfolderLineEdit, SIGNAL(textChanged (const QString &)),
//            this, SIGNAL(settingsChanged()));
//
//    connect(d->prefixLineEdit, SIGNAL(textChanged (const QString &)),
//            this, SIGNAL(settingsChanged()));
}

StorageSettingsBox::~StorageSettingsBox()
{
    delete d;
}

void StorageSettingsBox::buttonClicked(int mode)
{
    switch (mode)
    {
        case Subfolder:
            d->subfolderLineEdit->setVisible(true);
            d->subfolderLineEdit->setEnabled(true);
            d->prefixLineEdit->setVisible(false);
            d->prefixLineEdit->setEnabled(false);
            break;

        case Prefix:
            d->prefixLineEdit->setVisible(true);
            d->prefixLineEdit->setEnabled(true);
            d->subfolderLineEdit->setVisible(false);
            d->subfolderLineEdit->setEnabled(false);
            break;

        case Overwrite:
            d->subfolderLineEdit->setVisible(false);
            d->subfolderLineEdit->setEnabled(false);
            d->prefixLineEdit->setVisible(false);
            d->prefixLineEdit->setEnabled(false);
            break;
    }

    emit settingsChanged();
}

int StorageSettingsBox::storageMode() const
{
    return d->storageGroup->checkedId();
}

void StorageSettingsBox::setStorageMode(int mode)
{
    d->storageGroup->button(mode)->setChecked(true);
    buttonClicked(mode);
}

QString StorageSettingsBox::prefix() const
{
    return d->prefixLineEdit->text();
}

void StorageSettingsBox::setPrefix(const QString& prefix)
{
    d->prefixLineEdit->setText(prefix);
}

QString StorageSettingsBox::subfolder() const
{
    return d->subfolderLineEdit->text();
}

void StorageSettingsBox::setSubfolder(const QString& subfolder)
{
    d->subfolderLineEdit->setText(subfolder);
}

} // namespace KIPIRemoveRedEyesPlugin
