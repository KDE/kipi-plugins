/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-10-31
 * Description : a widget to display advanced settings
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

#include "advancedsettings.h"
#include "advancedsettings.moc"

// Qt includes.

#include <QGridLayout>

// KDE includes.

#include <kstandarddirs.h>

// Local includes.

#include "blobsettingsbox.h"
#include "classifiersettingsbox.h"
#include "removalsettings.h"
#include "storagesettingsbox.h"

namespace KIPIRemoveRedEyesPlugin
{

class AdvancedSettingsPriv
{

public:

    AdvancedSettingsPriv()
    {
        blobSettingsBox         = 0;
        classifierSettingsBox   = 0;
        settings                = 0;
        storageSettingsBox      = 0;
    }

    BlobSettingsBox*        blobSettingsBox;
    ClassifierSettingsBox*  classifierSettingsBox;
    RemovalSettings*        settings;
    StorageSettingsBox*     storageSettingsBox;
};

AdvancedSettings::AdvancedSettings(QWidget* parent)
                : QWidget(parent),
                  d(new AdvancedSettingsPriv)
{
    d->settings                 = new RemovalSettings;
    d->blobSettingsBox          = new BlobSettingsBox;
    d->classifierSettingsBox    = new ClassifierSettingsBox;
    d->storageSettingsBox       = new StorageSettingsBox;

    QGridLayout* mainLayout     = new QGridLayout;
    mainLayout->addWidget(d->storageSettingsBox,    0, 0, 1, 2);
    mainLayout->addWidget(d->classifierSettingsBox, 1, 0, 1, 1);
    mainLayout->addWidget(d->blobSettingsBox,       1, 1, 1, 1);
    mainLayout->setRowStretch(2, 10);
    setLayout(mainLayout);

    connect(d->storageSettingsBox, SIGNAL(settingsChanged()),
            this, SLOT(storageSettingsChanged()));
}

AdvancedSettings::~AdvancedSettings()
{
//    delete d->settings;
    delete d;
}

void AdvancedSettings::prepareSettings()
{
    d->settings->useStandardClassifier  = d->classifierSettingsBox->useStandardClassifier();
    if (!d->classifierSettingsBox->useStandardClassifier())
        d->settings->classifierFile = d->classifierSettingsBox->classifierUrl();
    d->settings->neighborGroups         = d->classifierSettingsBox->neighborGroups();
    d->settings->scaleFactor            = d->classifierSettingsBox->scalingFactor();

    d->settings->minBlobsize            = d->blobSettingsBox->minBlobSize();
    d->settings->minRoundness           = d->blobSettingsBox->minRoundness();
    storageSettingsChanged();
}

void AdvancedSettings::loadSettings(RemovalSettings* newSettings)
{
    d->settings = newSettings;
    applySettings();
}

void AdvancedSettings::applySettings()
{
    d->storageSettingsBox->setPrefix(d->settings->prefixName);
    d->storageSettingsBox->setSubfolder(d->settings->subfolderName);
    d->storageSettingsBox->setStorageMode(d->settings->storageMode);

    d->blobSettingsBox->setMinBlobSize(d->settings->minBlobsize);
    d->blobSettingsBox->setMinRoundness(d->settings->minRoundness);

    d->classifierSettingsBox->setScalingFactor(d->settings->scaleFactor);
    d->classifierSettingsBox->setNeighborGroups(d->settings->neighborGroups);
    d->classifierSettingsBox->setUseStandardClassifier(d->settings->useStandardClassifier);
    d->classifierSettingsBox->setClassifierUrl(d->settings->classifierFile);
}

void AdvancedSettings::storageSettingsChanged()
{
    d->settings->storageMode            = d->storageSettingsBox->storageMode();
    d->settings->subfolderName          = d->storageSettingsBox->subfolder();
    d->settings->prefixName             = d->storageSettingsBox->prefix();
}

RemovalSettings* AdvancedSettings::readSettings()
{
    prepareSettings();
    return d->settings;
}

} // namespace KIPIRemoveRedEyesPlugin
