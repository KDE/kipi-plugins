/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-11-01
 * Description : a widget that holds all storage settings
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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
#include <QCheckBox>

// KDE includes.

#include <klineedit.h>
#include <klocale.h>

// Local includes.

#include "removeredeyeswindow.h"

namespace KIPIRemoveRedEyesPlugin
{

struct StorageSettingsBoxPriv
{
    StorageSettingsBoxPriv()
    {
        storageGroup        = 0;
        suffixLineEdit      = 0;
        subfolderLineEdit   = 0;
    }

    QButtonGroup*   storageGroup;

    QCheckBox*      keywordCB;

    KLineEdit*      keywordLineEdit;
    KLineEdit*      suffixLineEdit;
    KLineEdit*      subfolderLineEdit;
};

StorageSettingsBox::StorageSettingsBox(QWidget* parent)
                  : QGroupBox(parent),
                    d(new StorageSettingsBoxPriv)
{
    setTitle(i18n("Storage Settings"));

    QString whatsThis = i18n("<p>These settings control the storage of the corrected images. "
            "There are three modes to choose from:</p>"
            "<p><ul>"
            "<li><b>Subfolder:</b> The corrected images will be saved in a subfolder "
            "under the current album path.</li>"
            "<li><b>Suffix:</b> A custom suffix will be added to the corrected image.</li>"
            "<li><b>Overwrite:</b> All original images will be replaced.</li>"
            "</ul></p>"
            "<p>Each of the three modes allows you to add an optional keyword to the image "
            "metadata.</p>");
    setWhatsThis(whatsThis);

    // ----------------------------------------------------------------

    d->storageGroup = new QButtonGroup(this);
    d->storageGroup->setExclusive(true);

    d->subfolderLineEdit = new KLineEdit;
    d->subfolderLineEdit->setToolTip(i18n("Enter the name of the subfolder here."));

    // ----------------------------------------------------------------

    QRadioButton* subfolderMode = new QRadioButton(i18n("Save files in a sub&folder"));
    subfolderMode->setToolTip(i18n("If checked, the corrected files will be saved "
                                   "in a subfolder under the current image album path."));
    d->storageGroup->addButton(subfolderMode, Subfolder);

    QRadioButton* suffixMode = new QRadioButton(i18n("Add s&uffix to filename"));
    suffixMode->setToolTip(i18n("If checked, a custom suffix will be added to the corrected file."));
    d->storageGroup->addButton(suffixMode, Suffix);

    QRadioButton* overwriteMode = new QRadioButton(i18n("&Overwrite original files"));
    overwriteMode->setToolTip(i18n("If checked, all original images will be replaced."));
    d->storageGroup->addButton(overwriteMode, Overwrite);

    d->suffixLineEdit = new KLineEdit;
    d->suffixLineEdit->setToolTip(i18n("Enter the name of the suffix here..."));

    // ----------------------------------------------------------------

    d->keywordCB       = new QCheckBox(i18n("Add &metadata keyword"));
    d->keywordCB->setChecked(false);
    d->keywordCB->setToolTip(i18n("If checked, a custom keyword will be applied to the image metadata."));

    d->keywordLineEdit = new KLineEdit;
    d->keywordLineEdit->setToolTip(i18n("Enter the name of the custom keyword here..."));

    // ----------------------------------------------------------------

    QGridLayout* correctionGroupLayout = new QGridLayout;
    correctionGroupLayout->addWidget(subfolderMode,         0, 0, 1, 1);
    correctionGroupLayout->addWidget(d->subfolderLineEdit,  0, 2, 1, 1);
    correctionGroupLayout->addWidget(suffixMode,            1, 0, 1, 1);
    correctionGroupLayout->addWidget(d->suffixLineEdit,     1, 2, 1, 1);
    correctionGroupLayout->addWidget(overwriteMode,         2, 0, 1,-1);
    correctionGroupLayout->addWidget(d->keywordCB,          3, 0, 1, 1);
    correctionGroupLayout->addWidget(d->keywordLineEdit,    3, 2, 1, 1);
    setLayout(correctionGroupLayout);

    // ----------------------------------------------------------------

    connect(d->storageGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonClicked(int)));

    connect(d->keywordCB, SIGNAL(toggled(bool)),
            this, SLOT(keywordToggled(bool)));

    // ----------------------------------------------------------------

    setStorageMode(Suffix);
    keywordToggled(false);
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
            d->subfolderLineEdit->setEnabled(true);
            d->suffixLineEdit->setEnabled(false);
            break;

        case Suffix:
            d->suffixLineEdit->setEnabled(true);
            d->subfolderLineEdit->setEnabled(false);
            break;

        case Overwrite:
            d->subfolderLineEdit->setEnabled(false);
            d->suffixLineEdit->setEnabled(false);
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

QString StorageSettingsBox::suffix() const
{
    return d->suffixLineEdit->text();
}

void StorageSettingsBox::setSuffix(const QString& suffix)
{
    d->suffixLineEdit->setText(suffix);
}

QString StorageSettingsBox::subfolder() const
{
    return d->subfolderLineEdit->text();
}

void StorageSettingsBox::setSubfolder(const QString& subfolder)
{
    d->subfolderLineEdit->setText(subfolder);
}

void StorageSettingsBox::keywordToggled(bool checked)
{
    d->keywordLineEdit->setEnabled(checked);
}

QString StorageSettingsBox::keyword() const
{
    return d->keywordLineEdit->text();
}

void StorageSettingsBox::setKeyword(const QString& keyword)
{
    d->keywordLineEdit->setText(keyword);
}

bool StorageSettingsBox::addKeyword() const
{
    return d->keywordCB->isChecked();
}

void StorageSettingsBox::setAddKeyword(bool checked)
{
    d->keywordCB->setChecked(checked);
    d->keywordLineEdit->setEnabled(checked);
}

} // namespace KIPIRemoveRedEyesPlugin
