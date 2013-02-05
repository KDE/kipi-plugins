/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-10-31
 * Description : a widget to display advanced settings
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

#include "advancedsettings.moc"

// Qt includes

#include <QGridLayout>

// Local includes

#include "blobsettingsbox.h"
#include "classifiersettingsbox.h"
#include "haarsettings.h"

namespace KIPIRemoveRedEyesPlugin
{

struct AdvancedSettings::Private
{
    Private()
    {
        blobSettingsBox       = 0;
        classifierSettingsBox = 0;
    }

    BlobSettingsBox*        blobSettingsBox;
    ClassifierSettingsBox*  classifierSettingsBox;
    HaarSettings            settings;
};

AdvancedSettings::AdvancedSettings(QWidget* parent)
    : QWidget(parent), d(new Private)
{
    d->blobSettingsBox       = new BlobSettingsBox;
    d->classifierSettingsBox = new ClassifierSettingsBox;

    // -----------------------------------------------------

    QGridLayout* const mainLayout = new QGridLayout;
    mainLayout->addWidget(d->classifierSettingsBox, 0, 0, 1, 1);
    mainLayout->addWidget(d->blobSettingsBox,       1, 0, 1, 1);
    mainLayout->setRowStretch(2, 10);
    setLayout(mainLayout);

    // -----------------------------------------------------

    connect(d->blobSettingsBox, SIGNAL(settingsChanged()),
            this, SIGNAL(settingsChanged()));

    connect(d->classifierSettingsBox, SIGNAL(settingsChanged()),
            this, SIGNAL(settingsChanged()));
}

AdvancedSettings::~AdvancedSettings()
{
    delete d;
}

void AdvancedSettings::prepareSettings()
{
    d->settings.useStandardClassifier = d->classifierSettingsBox->useStandardClassifier();
    d->settings.useSimpleMode         = false;

    if (d->classifierSettingsBox->useStandardClassifier())
    {
        d->settings.classifierFile = STANDARD_CLASSIFIER;
    }
    else
    {
        d->settings.classifierFile = d->classifierSettingsBox->classifierUrl();
    }

    d->settings.neighborGroups = d->classifierSettingsBox->neighborGroups();
    d->settings.scaleFactor    = d->classifierSettingsBox->scalingFactor();
    d->settings.minBlobsize    = d->blobSettingsBox->minBlobSize();
    d->settings.minRoundness   = d->blobSettingsBox->minRoundness();
}

void AdvancedSettings::loadSettings(HaarSettings& newSettings)
{
    d->settings = newSettings;
    applySettings();
}

void AdvancedSettings::applySettings()
{
    d->blobSettingsBox->setMinBlobSize(d->settings.minBlobsize);
    d->blobSettingsBox->setMinRoundness(d->settings.minRoundness);

    d->classifierSettingsBox->setScalingFactor(d->settings.scaleFactor);
    d->classifierSettingsBox->setNeighborGroups(d->settings.neighborGroups);
    d->classifierSettingsBox->setUseStandardClassifier(d->settings.useStandardClassifier);
    d->classifierSettingsBox->setClassifierUrl(d->settings.classifierFile);
}

HaarSettings AdvancedSettings::readSettings()
{
    prepareSettings();
    return d->settings;
}

} // namespace KIPIRemoveRedEyesPlugin
