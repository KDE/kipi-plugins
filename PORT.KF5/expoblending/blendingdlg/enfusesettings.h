/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <klocale.h>
#include <kurl.h>

// Locale includes

#include "kpsavesettingswidget.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class EnfuseSettings
{
public:

    EnfuseSettings()
    {
        hardMask        = false;
        autoLevels      = true;
        ciecam02        = false;
        levels          = 20;
        exposure        = 1.0;
        saturation      = 0.2;
        contrast        = 0.0;
        outputFormat    = KPSaveSettingsWidget::OUTPUT_PNG;
    }

    ~EnfuseSettings()
    {
    }

    QString asCommentString() const
    {
        QString ret;

        ret.append(i18n("Hardmask: %1",   hardMask ? i18n("enabled") : i18n("disabled")));
        ret.append("\n");
        ret.append(i18n("CIECAM02: %1",   ciecam02 ? i18n("enabled") : i18n("disabled")));
        ret.append("\n");
        ret.append(i18n("Levels: %1",     autoLevels ? i18n("auto") : QString::number(levels)));
        ret.append("\n");
        ret.append(i18n("Exposure: %1",   QString::number(exposure)));
        ret.append("\n");
        ret.append(i18n("Saturation: %1", QString::number(saturation)));
        ret.append("\n");
        ret.append(i18n("Contrast: %1",   QString::number(contrast)));

        return ret;
    }

    QString inputImagesList() const
    {
        QString ret;

        foreach(const KUrl url, inputUrls)
            ret.append(url.fileName() + " ; ");

        ret.truncate(ret.length()-3);
        return ret;
    }

    bool                               autoLevels;
    bool                               hardMask;
    bool                               ciecam02;

    int                                levels;

    double                             exposure;
    double                             saturation;
    double                             contrast;

    QString                            targetFileName;

    KUrl::List                         inputUrls;
    KUrl                               previewUrl;

    KPSaveSettingsWidget::OutputFormat outputFormat;
};

// ------------------------------------------------------------------------


class EnfuseSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    EnfuseSettingsWidget(QWidget* const parent);
    ~EnfuseSettingsWidget();

    void           setSettings(const EnfuseSettings& settings);
    EnfuseSettings settings() const;

    void           resetToDefault();

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

private:

    class EnfuseSettingsWidgetPriv;
    EnfuseSettingsWidgetPriv* const d;
};

} // namespace KIPIExpoBlendingPlugin

#endif /* ENFUSESETTINGS_H */
