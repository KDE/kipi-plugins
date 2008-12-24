/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-31
 * Description : a widget to display simple settings
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

#include "settingstab.h"
#include "settingstab.moc"

// Qt includes.

#include <QGridLayout>
#include <QPushButton>
#include <QStackedWidget>

// KDE includes.

#include <klocale.h>

// Local includes.

#include "advancedsettings.h"
#include "simplesettings.h"
#include "removalsettings.h"
#include "storagesettingsbox.h"
#include "unprocessedsettingsbox.h"

namespace KIPIRemoveRedEyesPlugin
{

class SettingsTabPriv
{

public:

    SettingsTabPriv()
    {
        storageSettingsBox  = 0;

        advancedSettings    = 0;
        simpleSettings      = 0;

        settingsSwitcherBtn = 0;
        settingsStack       = 0;
    }

    bool                        simpleCorrectionMode;

    QPushButton*                settingsSwitcherBtn;
    QStackedWidget*             settingsStack;

    AdvancedSettings*           advancedSettings;
    RemovalSettings             settings;
    SimpleSettings*             simpleSettings;
    StorageSettingsBox*         storageSettingsBox;
    UnprocessedSettingsBox*     unprocessedSettingsBox;
};

SettingsTab::SettingsTab(QWidget* parent)
              : QWidget(parent),
                d(new SettingsTabPriv)
{
    d->simpleCorrectionMode     = true;
    d->settingsSwitcherBtn      = new QPushButton;
    d->storageSettingsBox       = new StorageSettingsBox;
    d->unprocessedSettingsBox   = new UnprocessedSettingsBox;

    // settings stack widget ----------------------------------------------------------

    d->simpleSettings       = new SimpleSettings;
    d->advancedSettings     = new AdvancedSettings;

    d->settingsStack = new QStackedWidget;
    d->settingsStack->insertWidget(Simple, d->simpleSettings);
    d->settingsStack->insertWidget(Advanced, d->advancedSettings);
    d->settingsStack->setCurrentIndex(Simple);

    // Set layouts --------------------------------------------------------------

    QGridLayout* settingsTabLayout = new QGridLayout;
    settingsTabLayout->addWidget(d->settingsStack,          0, 0, 1, 1);
    settingsTabLayout->setRowStretch(1, 10);

    QGridLayout* storageLayout = new QGridLayout;
    storageLayout->addWidget(d->storageSettingsBox,     0, 0, 1, 1);
    storageLayout->addWidget(d->unprocessedSettingsBox, 1, 0, 1, 1);
    storageLayout->setRowStretch(2, 10);

    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addLayout(settingsTabLayout,        0, 0, 1, 2);
    mainLayout->addLayout(storageLayout,            0, 2, 1, 2);
    mainLayout->addWidget(d->settingsSwitcherBtn,   2, 0, 1, 1);
    mainLayout->setRowStretch(1, 10);
    setLayout(mainLayout);

    connect(d->settingsSwitcherBtn, SIGNAL(clicked()),
            this, SLOT(settingsModeChanged()));

    connect(d->settingsSwitcherBtn, SIGNAL(clicked()),
            this, SIGNAL(settingsChanged()));

    connect(d->simpleSettings, SIGNAL(settingsChanged()),
            this, SIGNAL(settingsChanged()));

    connect(d->advancedSettings, SIGNAL(settingsChanged()),
            this, SIGNAL(settingsChanged()));
}

SettingsTab::~SettingsTab()
{
    delete d;
}

void SettingsTab::prepareSettings()
{
    d->settings.storageMode            = d->storageSettingsBox->storageMode();
    d->settings.unprocessedMode        = d->unprocessedSettingsBox->handleMode();
    d->settings.subfolderName          = d->storageSettingsBox->subfolder();
    d->settings.suffixName             = d->storageSettingsBox->suffix();
    d->settings.addKeyword             = d->storageSettingsBox->addKeyword();
    d->settings.keywordName            = d->storageSettingsBox->keyword();
}

void SettingsTab::loadSettings(RemovalSettings newSettings)
{
    d->settings = newSettings;
    d->simpleSettings->loadSettings(d->settings);
    d->advancedSettings->loadSettings(d->settings);
    applySettings();
    setSettingsMode(Simple);
}

void SettingsTab::applySettings()
{
    d->storageSettingsBox->setSuffix(d->settings.suffixName);
    d->storageSettingsBox->setSubfolder(d->settings.subfolderName);
    d->storageSettingsBox->setStorageMode(d->settings.storageMode);
    d->storageSettingsBox->setAddKeyword(d->settings.addKeyword);
    d->storageSettingsBox->setKeyword(d->settings.keywordName);
    d->unprocessedSettingsBox->setHandleMode(d->settings.unprocessedMode);
}

RemovalSettings SettingsTab::readSettings()
{
    if (d->simpleCorrectionMode)
        d->settings = d->simpleSettings->readSettings();
    else
        d->settings = d->advancedSettings->readSettings();

    prepareSettings();
    return d->settings;
}

RemovalSettings SettingsTab::readSettingsForSave()
{
    d->settings = d->advancedSettings->readSettings();
    d->settings.simpleMode = d->simpleSettings->simpleMode();
    prepareSettings();
    return d->settings;
}

void SettingsTab::setSettingsMode(SettingsMode mode)
{
    switch (mode)
    {
        case Simple:
            d->settingsSwitcherBtn->setText(i18n("Advanced Mode"));
            d->settingsStack->setCurrentIndex(Simple);
            d->simpleCorrectionMode = true;
            break;

        case Advanced:
            d->settingsSwitcherBtn->setText(i18n("Simple Mode"));
            d->settingsStack->setCurrentIndex(Advanced);
            d->simpleCorrectionMode = false;
            break;
    }
}

void SettingsTab::settingsModeChanged()
{
    switch (d->settingsStack->currentIndex())
    {
        case Simple:
            setSettingsMode(Advanced);
            break;

        case Advanced:
            setSettingsMode(Simple);
            break;
    }
}

void SettingsTab::updateSettings()
{
    if (d->simpleCorrectionMode)
        d->settings = d->simpleSettings->readSettings();
    else
        d->settings = d->advancedSettings->readSettings();
}

} // namespace KIPIRemoveRedEyesPlugin
