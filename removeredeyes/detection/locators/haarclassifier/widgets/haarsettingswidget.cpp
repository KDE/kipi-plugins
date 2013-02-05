/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-10-31
 * Description : a widget to display simple settings
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

#include "haarsettingswidget.moc"

// Qt includes

#include <QGridLayout>
#include <QPushButton>
#include <QStackedWidget>

// KDE includes

#include <klocale.h>

// Local includes

#include "advancedsettings.h"
#include "simplesettings.h"
#include "haarsettings.h"
#include "storagesettingsbox.h"
#include "unprocessedsettingsbox.h"

namespace KIPIRemoveRedEyesPlugin
{

struct HaarSettingsWidget::Private
{
    Private() :
        simpleCorrectionMode(true),
        settingsSwitcherBtn(0),
        settingsStack(0),
        advancedSettings(0),
        simpleSettings(0)
    {
    }

    bool              simpleCorrectionMode;

    QPushButton*      settingsSwitcherBtn;
    QStackedWidget*   settingsStack;

    AdvancedSettings* advancedSettings;
    HaarSettings      settings;
    SimpleSettings*   simpleSettings;
};

HaarSettingsWidget::HaarSettingsWidget(QWidget* const parent)
    : QWidget(parent), d(new Private)
{
    d->simpleCorrectionMode = true;
    d->settingsSwitcherBtn  = new QPushButton;

    // settings stack widget ----------------------------------------------------------

    d->simpleSettings   = new SimpleSettings;
    d->advancedSettings = new AdvancedSettings;

    d->settingsStack    = new QStackedWidget;
    d->settingsStack->insertWidget(Simple,   d->simpleSettings);
    d->settingsStack->insertWidget(Advanced, d->advancedSettings);
    d->settingsStack->setCurrentIndex(Simple);

    // Set layouts --------------------------------------------------------------

    QGridLayout* const mainLayout = new QGridLayout;
    mainLayout->addWidget(d->settingsStack,       0, 0, 1, 1);
    mainLayout->addWidget(d->settingsSwitcherBtn, 2, 0, 1, 1);
    mainLayout->setRowStretch(1, 10);
    setLayout(mainLayout);

    connect(d->settingsSwitcherBtn, SIGNAL(clicked()),
            this, SLOT(settingsModeChanged()));
}

HaarSettingsWidget::~HaarSettingsWidget()
{
    delete d;
}

void HaarSettingsWidget::loadSettings(HaarSettings& newSettings)
{
    d->settings = newSettings;
    d->simpleSettings->loadSettings(d->settings);
    d->advancedSettings->loadSettings(d->settings);
    setSettingsMode(Simple);
}

HaarSettings HaarSettingsWidget::readSettings()
{
    updateSettings();
    return d->settings;
}

HaarSettings HaarSettingsWidget::readSettingsForSave()
{
    d->settings            = d->advancedSettings->readSettings();
    d->settings.simpleMode = d->simpleSettings->simpleMode();
    return d->settings;
}

void HaarSettingsWidget::setSettingsMode(SettingsMode mode)
{
    switch (mode)
    {
        case Simple:
            d->settingsSwitcherBtn->setText(i18n("&Advanced Mode"));
            d->settingsStack->setCurrentIndex(Simple);
            d->simpleCorrectionMode = true;
            break;

        case Advanced:
            d->settingsSwitcherBtn->setText(i18n("&Simple Mode"));
            d->settingsStack->setCurrentIndex(Advanced);
            d->simpleCorrectionMode = false;
            break;
    }
}

void HaarSettingsWidget::settingsModeChanged()
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

void HaarSettingsWidget::updateSettings()
{
    if (d->simpleCorrectionMode)
    {
        d->settings = d->simpleSettings->readSettings();
    }
    else
    {
        d->settings = d->advancedSettings->readSettings();
    }
}

} // namespace KIPIRemoveRedEyesPlugin
