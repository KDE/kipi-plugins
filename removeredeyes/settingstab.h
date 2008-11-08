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

#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

// Qt includes.

#include <QWidget>

namespace KIPIRemoveRedEyesPlugin
{

class SettingsTabPriv;
class RemovalSettings;

class SettingsTab : public QWidget
{
    Q_OBJECT

public:

    enum SettingsMode
    {
        Simple = 0,
        Advanced
    };

    SettingsTab(QWidget* parent = 0);
    virtual ~SettingsTab();

    void loadSettings(RemovalSettings);
    RemovalSettings readSettings();
    RemovalSettings readSettingsForSave();

private slots:

    void prepareSettings();
    void settingsModeChanged();

private:

    void applySettings();
    void updateSettings();
    void setSettingsMode(SettingsMode mode);

private:

    SettingsTabPriv* const d;
};

}

#endif // SETTINGSTAB_H
