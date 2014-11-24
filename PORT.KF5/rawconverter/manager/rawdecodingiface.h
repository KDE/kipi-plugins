/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-09
 * Description : RAW decoding interface
 *
 * Copyright (C) 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef RAW_DECODING_IFACE_H
#define RAW_DECODING_IFACE_H

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>
#include <libkdcraw/rawdecodingsettings.h>

// Local includes

#include "kpsavesettingswidget.h"

using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIRawConverterPlugin
{

class RawDecodingIface : public KDcraw
{
    Q_OBJECT

public:

    RawDecodingIface();
    ~RawDecodingIface();

public:

    /** Extract a small size of decode RAW data in 8 bits/color/pixels
        using sRGB color space.
    */
    bool decodeHalfRAWImage(const QString& filePath,
                            QString& destPath, KPSaveSettingsWidget::OutputFormat outputFileFormat,
                            const RawDecodingSettings& rawDecodingSettings);

    /** Extract a full size of RAW data in 8 bits/color/pixels using
        sRGB color space.
    */
    bool decodeRAWImage(const QString& filePath,
                        QString& destPath, KPSaveSettingsWidget::OutputFormat outputFileFormat,
                        const RawDecodingSettings& rawDecodingSettings);

private:

    bool loadedFromDecoder(const QString& filePath,
                           QString& destPath, KPSaveSettingsWidget::OutputFormat outputFileFormat,
                           const QByteArray& imageData, int width, int height, int rgbmax,
                           const RawDecodingSettings& rawDecodingSettings);
};

}  // namespace KIPIRawConverterPlugin

#endif /* RAW_DECODING_IFACE_H */
