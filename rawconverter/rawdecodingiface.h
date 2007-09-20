/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-09
 * Description : RAW decoding interface
 *
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C Ansi includes.

extern "C"
{
#include <png.h>
}

// C++ includes.

#include <cstdarg>

// Qt Includes.

#include <qstring.h>
#include <qobject.h>
#include <qimage.h>

// LibKDcraw includes.

#include <libkdcraw/kdcraw.h>
#include <libkdcraw/rawdecodingsettings.h>

// Local includes.

#include "savesettingswidget.h"

namespace KIPIRawConverterPlugin
{

class RawDecodingIface : public KDcrawIface::KDcraw
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
                            QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                            KDcrawIface::RawDecodingSettings rawDecodingSettings);

    /** Extract a full size of RAW data in 8 bits/color/pixels using 
        sRGB color space.
    */
    bool decodeRAWImage(const QString& filePath, 
                        QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                        KDcrawIface::RawDecodingSettings rawDecodingSettings);

private:

    QByteArray getICCProfilFromFile(KDcrawIface::RawDecodingSettings::OutputColorSpace colorSpace);

    bool loadedFromDcraw(const QString& filePath, 
                         QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                         QByteArray imageData, int width, int height);

    void writeRawProfile(png_struct *ping, png_info *ping_info, char *profile_type, 
                         char *profile_data, png_uint_32 length);

    size_t concatenateString(char *destination, const char *source, const size_t length);
    size_t copyString(char *destination, const char *source, const size_t length);
    long   formatString(char *string, const size_t length, const char *format,...);
    long   formatStringList(char *string, const size_t length, const char *format, va_list operands);
};

}  // namespace KIPIRawConverterPlugin

#endif /* RAW_DECODING_IFACE_H */
