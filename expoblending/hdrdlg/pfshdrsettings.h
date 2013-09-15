/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-09-04
 * Description : a tool to blend bracketed images/create HDR images.
 *
 * Copyright (C) 2013 by Soumajyoti Sarkar <ergy dot ergy at gmail dot com>
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

#ifndef PFSHDRSETTINGS_H
#define PFSHDRSETTINGS_H

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

class PfsHdrSettings
{
public:

    PfsHdrSettings()
    {
        outputFormat    = KPSaveSettingsWidget::OUTPUT_PNG;
    }

    ~PfsHdrSettings()
    {}

    QString inputImagesList() const
    {
        QString ret;

        foreach(const KUrl url, inputUrls)
            ret.append(url.fileName() + " ; ");

        ret.truncate(ret.length()-3);
        return ret;
    }

    QString                            targetFileName;

    KUrl::List                         inputUrls;
    KUrl                               previewUrl;

    KPSaveSettingsWidget::OutputFormat outputFormat;
};

} // namespace KIPIExpoBlendingPlugin

#endif /* PFSHDRSETTINGS_H */
