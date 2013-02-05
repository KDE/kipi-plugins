/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-01
 * Description : a widget that holds all storage settings
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "storagesettingsbox.moc"

// Qt includes

#include <QGridLayout>
#include <QCheckBox>

// KDE includes

#include <klineedit.h>
#include <klocale.h>
#include <kcombobox.h>

// Local includes

#include "removeredeyeswindow.h"

namespace KIPIRemoveRedEyesPlugin
{

struct StorageSettingsBox::Private
{
    Private() :
        keywordCB(0),
        saveMethodCB(0),
        keywordLineEdit(0),
        saveMethodLineEdit(0)
    {
    }

    QCheckBox* keywordCB;

    KComboBox* saveMethodCB;

    KLineEdit* keywordLineEdit;
    KLineEdit* saveMethodLineEdit;
};

StorageSettingsBox::StorageSettingsBox(QWidget* const parent)
    : QGroupBox(parent), d(new Private)
{
    setTitle(i18n("Storage Settings"));

    QString whatsThis = i18n("<p>These settings control the storage of the corrected images. "
                             "There are three modes to choose from:</p>"
                             "<p><ul>"
                             "<li><b>Subfolder:</b> The corrected images will be saved in a subfolder "
                             "under the current album path.</li>"
                             "<li><b>Prefix:</b> A custom prefix will be added to the corrected image.</li>"
                             "<li><b>Suffix:</b> A custom suffix will be added to the corrected image.</li>"
                             "<li><b>Overwrite:</b> All original images will be replaced.</li>"
                             "</ul></p>"
                             "<p>Each of the three modes allows you to add an optional keyword to the image "
                             "metadata.</p>");
    setWhatsThis(whatsThis);

    // ----------------------------------------------------------------

    d->saveMethodCB = new KComboBox;
    d->saveMethodCB->insertItem(Subfolder, i18n("Save in Subfolder"));
    d->saveMethodCB->insertItem(Prefix,    i18n("Add Prefix"));
    d->saveMethodCB->insertItem(Suffix,    i18n("Add Suffix"));
    d->saveMethodCB->insertItem(Overwrite, i18n("Overwrite"));

    d->saveMethodLineEdit = new KLineEdit;
    d->saveMethodLineEdit->setToolTip(i18n("Enter the name of the subfolder, prefix or suffix to use for the corrected images."));

    d->keywordCB = new QCheckBox(i18n("Add &metadata keyword"));
    d->keywordCB->setChecked(false);
    d->keywordCB->setToolTip(i18n("If checked, a custom keyword will be applied to the image metadata."));

    d->keywordLineEdit = new KLineEdit;
    d->keywordLineEdit->setToolTip(i18n("Enter the name of the custom keyword here..."));

    // ----------------------------------------------------------------

    QGridLayout* const correctionGroupLayout = new QGridLayout;
    correctionGroupLayout->addWidget(d->saveMethodCB,       0, 0, 1, 1);
    correctionGroupLayout->addWidget(d->saveMethodLineEdit, 0, 2, 1, 1);
    correctionGroupLayout->addWidget(d->keywordCB,          1, 0, 1, 1);
    correctionGroupLayout->addWidget(d->keywordLineEdit,    1, 2, 1, 1);
    setLayout(correctionGroupLayout);

    // ----------------------------------------------------------------

    connect(d->saveMethodCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(buttonClicked(int)));

    connect(d->keywordCB, SIGNAL(toggled(bool)),
            this, SLOT(keywordToggled(bool)));

    // ----------------------------------------------------------------

    setStorageMode(Subfolder);
    keywordToggled(false);
}

StorageSettingsBox::~StorageSettingsBox()
{
    delete d;
}

void StorageSettingsBox::buttonClicked(int mode)
{
    d->saveMethodLineEdit->setEnabled(true);

    if (mode == Overwrite)
    {
        d->saveMethodLineEdit->setEnabled(false);
    }

    emit settingsChanged();
}

int StorageSettingsBox::storageMode() const
{
    return d->saveMethodCB->currentIndex();
}

void StorageSettingsBox::setStorageMode(int mode)
{
    d->saveMethodCB->setCurrentIndex(mode);
    buttonClicked(mode);
}

QString StorageSettingsBox::extra() const
{
    return d->saveMethodLineEdit->text();
}

void StorageSettingsBox::setExtra(const QString& extra)
{
    d->saveMethodLineEdit->setText(extra);
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
