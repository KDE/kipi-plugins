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

#include "rawdecodingiface.moc"

// C++ includes

#include <cstdio>
#include <cmath>

// Qt includes

#include <QByteArray>
#include <QDateTime>
#include <QFileInfo>

// KDE includes

#include <kdebug.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/version.h>

// Local includes

#include "kpversion.h"
#include "kpwriteimage.h"
#include "kpmetadata.h"

namespace KIPIRawConverterPlugin
{

RawDecodingIface::RawDecodingIface()
    : KDcraw()
{
}

RawDecodingIface::~RawDecodingIface()
{
}

bool RawDecodingIface::decodeHalfRAWImage(const QString& filePath,
                                          QString& destPath, KPSaveSettingsWidget::OutputFormat outputFileFormat,
                                          const RawDecodingSettings& rawDecodingSettings)
{
    int width, height, rgbmax;
    QByteArray imageData;

    if (!KDcraw::decodeHalfRAWImage(filePath, rawDecodingSettings, imageData, width, height, rgbmax))
        return false;

    return (loadedFromDecoder(filePath, destPath, outputFileFormat,
                              imageData, width, height, rgbmax, rawDecodingSettings));
}

bool RawDecodingIface::decodeRAWImage(const QString& filePath, 
                                      QString& destPath, KPSaveSettingsWidget::OutputFormat outputFileFormat,
                                      const RawDecodingSettings& rawDecodingSettings)
{
    int width, height, rgbmax;
    QByteArray imageData;

    if (!KDcraw::decodeRAWImage(filePath, rawDecodingSettings, imageData, width, height, rgbmax))
        return false;

    return (loadedFromDecoder(filePath, destPath, outputFileFormat,
                              imageData, width, height, rgbmax, rawDecodingSettings));
}

// ----------------------------------------------------------------------------------

bool RawDecodingIface::loadedFromDecoder(const QString& filePath, 
                                         QString& destPath, KPSaveSettingsWidget::OutputFormat outputFileFormat,
                                         const QByteArray& imageData, int width, int height, int rgbmax, 
                                         const RawDecodingSettings& rawDecodingSettings)
{
    bool sixteenBits = rawDecodingSettings.sixteenBitsImage;
    uchar* sptr      = (uchar*)imageData.data();
    float factor     = 65535.0 / rgbmax;
    uchar tmp8[2];
    unsigned short tmp16[3];

    // Set RGB color components.
    for (int i = 0 ; i < (width * height) ; ++i)
    {
        if (!sixteenBits)   // 8 bits color depth image.
        {
            // Swap Red and Blue
            tmp8[0] = sptr[2];
            tmp8[1] = sptr[0];
            sptr[0] = tmp8[0];
            sptr[2] = tmp8[1];

            sptr += 3;
        }
        else                // 16 bits color depth image.
        {
            // Swap Red and Blue and re-ajust color component values

            tmp16[0] = (unsigned short)((sptr[5]*256 + sptr[4]) * factor);      // Blue
            tmp16[1] = (unsigned short)((sptr[3]*256 + sptr[2]) * factor);      // Green
            tmp16[2] = (unsigned short)((sptr[1]*256 + sptr[0]) * factor);      // Red

            memcpy(&sptr[0], &tmp16[0], 6);

            sptr += 6;
        }
    }

    // Use a QImage instance to write IPTC preview and Exif thumbnail
    // and adapt color component order to KPWriteImage data format (RGB ==> BGR)

    QImage img(width, height, QImage::Format_ARGB32);
    uint*  dptr            = reinterpret_cast<uint*>(img.bits());
    uchar* sptr8           = (uchar*)imageData.data();
    unsigned short* sptr16 = reinterpret_cast<unsigned short*>(sptr8);

    for (int i = 0 ; i < (width * height) ; ++i)
    {
        if (!sixteenBits)   // 8 bits color depth image.
        {
            *dptr++  = qRgba(sptr8[2], sptr8[1], sptr8[0], 0xFF);
            sptr8   += 3;
        }
        else                // 16 bits color depth image.
        {
            *dptr++ = qRgba((uchar)((sptr16[2] * 255UL)/65535UL),
                            (uchar)((sptr16[1] * 255UL)/65535UL),
                            (uchar)((sptr16[0] * 255UL)/65535UL),
                            0xFF);
            sptr16 += 3;
        }
    }

    QImage iptcPreview   = img.scaled(1280, 1024, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage exifThumbnail = iptcPreview.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // -- Write image data into destination file -------------------------------

    QByteArray prof = KPWriteImage::getICCProfilFromFile(m_rawDecodingSettings.outputColorSpace);
    QString soft = QString("Kipi Raw Converter v.%1").arg(kipiplugins_version);
    QFileInfo fi(filePath);
    KUrl url(filePath);
    destPath = fi.absolutePath() + QString("/") + ".kipi-rawconverter-tmp-" 
                                 + QString::number(QDateTime::currentDateTime().toTime_t()) + QString(url.fileName());

    // Metadata restoration and update.
    KPMetadata meta;

    meta.load(filePath);
    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    meta.setImageDimensions(QSize(width, height));
    meta.setExifThumbnail(exifThumbnail);

    // Update Iptc preview.
    // NOTE: see bug #130525. a JPEG segment is limited to 64K. If the IPTC byte array is
    // bigger than 64K duing of image preview tag size, the target JPEG image will be
    // broken. Note that IPTC image preview tag is limited to 256K!!!
    // There is no limitation with TIFF and PNG about IPTC byte array size.
    if (outputFileFormat != KPSaveSettingsWidget::OUTPUT_JPEG)
        meta.setImagePreview(iptcPreview);

    meta.setExifTagString("Exif.Image.DocumentName", fi.fileName());
    meta.setXmpTagString("Xmp.tiff.Make", meta.getExifTagString("Exif.Image.Make"));
    meta.setXmpTagString("Xmp.tiff.Model", meta.getExifTagString("Exif.Image.Model"));

    // the image has already been rotated after being read from the raw format,
    // therefore reset the EXIF-tag:
    meta.setImageOrientation(KPMetadata::ORIENTATION_NORMAL);

    KPWriteImage wImageIface;
    wImageIface.setImageData(imageData, width, height, sixteenBits, false, prof, meta);
    wImageIface.setCancel(&m_cancel);

    switch(outputFileFormat)
    {
        case KPSaveSettingsWidget::OUTPUT_JPEG:
        {
            if (!wImageIface.write2JPEG(destPath)) return false;
            break;
        }
        case KPSaveSettingsWidget::OUTPUT_PNG:
        {
            if (!wImageIface.write2PNG(destPath)) return false;
            break;
        }
        case KPSaveSettingsWidget::OUTPUT_TIFF:
        {
            if (!wImageIface.write2TIFF(destPath)) return false;
            break;
        }
        case KPSaveSettingsWidget::OUTPUT_PPM:
        {
            if (!wImageIface.write2PPM(destPath)) return false;
            break;
        }
        default:
        {
            kDebug() << "Invalid output file format" ;
            return false;
        }
    }

    if (m_cancel)
    {
        if (::remove(QFile::encodeName(destPath)) != 0)
            kDebug() << "Cannot remove " << destPath;

        return false;
    }

    return true;
}

}  // namespace KIPIRawConverterPlugin
