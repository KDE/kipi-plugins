/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-31
 * Description : a widget to display simple settings
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

#ifndef HAARSETTINGSWIDGET_H
#define HAARSETTINGSWIDGET_H

// Qt includes

#include <QWidget>

namespace KIPIRemoveRedEyesPlugin
{

struct SettingsTabPriv;
class HaarSettings;

class HaarSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    enum SettingsMode
    {
        Simple = 0,
        Advanced
    };

    HaarSettingsWidget(QWidget* parent = 0);
    ~HaarSettingsWidget();

    void         loadSettings(HaarSettings&);
    HaarSettings readSettings();
    HaarSettings readSettingsForSave();

private Q_SLOTS:

    void settingsModeChanged();

private:

    void updateSettings();
    void setSettingsMode(SettingsMode mode);

private:

    SettingsTabPriv* const d;
};

}

#endif // HAARSETTINGSWIDGET_H
