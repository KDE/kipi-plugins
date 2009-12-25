/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-13
 * Description : a widget to provide options to save image.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SAVESETTINGSWIDGET_H
#define SAVESETTINGSWIDGET_H

// Qt includes

#include <QWidget>

// KDE includes

#include <kconfig.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class SaveSettingsWidgetPriv;

class KIPIPLUGINS_EXPORT SaveSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    enum OutputFormat
    {
        OUTPUT_PNG = 0,
        OUTPUT_TIFF,
        OUTPUT_JPEG,
        OUTPUT_PPM
    };

    enum ConflictRule
    {
        OVERWRITE = 0,
        ASKTOUSER
    };

public:

    SaveSettingsWidget(QWidget *parent, bool conflicRules=true);
    ~SaveSettingsWidget();

    SaveSettingsWidget::OutputFormat fileFormat();
    ConflictRule conflictRule();

    QString extension();
    QString typeMime();

    void setFileFormat(SaveSettingsWidget::OutputFormat f);
    void setConflictRule(ConflictRule r);

    void setDefaultSettings();

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

public Q_SLOTS:

    void slotPopulateImageFormat(bool sixteenBits);

Q_SIGNALS:

    void signalSaveFormatChanged();

private:

    SaveSettingsWidgetPriv* const d;
};

} // namespace KIPIPlugins

#endif /* SAVESETTINGSWIDGET_H */
