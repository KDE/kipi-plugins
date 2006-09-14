/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net> 
 * Date   : 2006-12-09
 * Description : dcraw interface (tested with dcraw 8.x releases)
 *
 * Copyright 2006 by Gilles Caulier
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

#ifndef DCRAWIFACE_H
#define DCRAWIFACE_H

// Qt Includes.

#include <qcstring.h>
#include <qstring.h>
#include <qimage.h>

// Local includes.

#include "rawdecodingsettings.h"

namespace KIPIRawConverterPlugin
{

class DcrawIface
{

public:

    DcrawIface();
    ~DcrawIface();

    /** To cancel 'decodeHalfRAWImage' and 'decodeRAWImage' methods running 
        in a separate thread.
    */
    void cancel();

    /** Get the embedded preview image in RAW file
    */
    bool loadDcrawPreview(QImage& image, const QString& path);

    /** Get the camera model witch have taken RAW file
    */ 
    bool rawFileIdentify(QString& identify, const QString& path);

    /** Extract a small size of decode RAW data in 8 bits/color/pixels 
        using sRGB color space.
    */
    bool decodeHalfRAWImage(const QString& filePath, QString& destPath,
                            RawDecodingSettings rawDecodingSettings);

    /** Extract a full size of RAW data in 8 bits/color/pixels using 
        sRGB color space.
    */
    bool decodeRAWImage(const QString& filePath, QString& destPath,
                        RawDecodingSettings rawDecodingSettings);

private:

    QByteArray getICCProfilFromFile(RawDecodingSettings::OutputColorSpace colorSpace);

private:

    bool m_cancel;
};

}  // namespace KIPIRawConverterPlugin

#endif /* DCRAWIFACE_H */
