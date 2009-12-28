/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ENFUSESETTINGS_H
#define ENFUSESETTINGS_H

// Qt includes

#include <QWidget>

// KDE includes

#include <kconfig.h>

namespace KIPIExpoBlendingPlugin
{

class EnfuseSettings
{
public:

    EnfuseSettings()
    {
        hardMask   = false;
        autoLevels = true;
        ciecam02   = false;
        levels     = 20;
        exposure   = 1.0;
        saturation = 0.2;
        contrast   = 0.0;
    }

    ~EnfuseSettings()
    {
    }

    QString hasCommentString() const
    {
        // This string must still in English.
        // There will be recorded in comments metadata.
        return QString("Enfuse Settings: "
                       "Hardmask: %1 "
                       "AutoLevels: %2 "
                       "CIECAM02: %3 "
                       "Level: %4 "
                       "Exposure: %5 "
                       "Saturation: %6 "
                       "Contrast: %7")
                       .arg(hardMask)
                       .arg(autoLevels)
                       .arg(ciecam02)
                       .arg(levels)
                       .arg(exposure)
                       .arg(saturation)
                       .arg(contrast);
    }

    bool  autoLevels;
    bool  hardMask;
    bool  ciecam02;

    int   levels;

    float exposure;
    float saturation;
    float contrast;
};

// ------------------------------------------------------------------------

class EnfuseSettingsWidgetPriv;

class EnfuseSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    EnfuseSettingsWidget(QWidget *parent);
    ~EnfuseSettingsWidget();

    void           setSettings(const EnfuseSettings& settings);
    EnfuseSettings settings() const;

    void           setDefaultSettings();

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

private:

    EnfuseSettingsWidgetPriv* const d;
};

} // namespace KIPIExpoBlendingPlugin

#endif /* ENFUSESETTINGS_H */
