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

#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

// Qt includes.

#include <QWidget>

namespace KIPIRemoveRedEyesPlugin
{

class AdvancedSettingsPriv;
class RemovalSettings;

class AdvancedSettings : public QWidget
{
    Q_OBJECT

public:

    AdvancedSettings(QWidget* parent = 0);
    virtual ~AdvancedSettings();

    void loadSettings(RemovalSettings);
    RemovalSettings readSettings();

private slots:

    void prepareSettings();

private:

    void applySettings();

private:

    AdvancedSettingsPriv* const d;
};

}

#endif /* ADVANCEDSETTINGS_H */
