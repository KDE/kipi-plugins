/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-09
 * Description : RAW decoding interface
 *
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
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

#include "rawdecodingiface.h"
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

// LibKExiv2 includes

#include <libkexiv2/version.h>

// LibKDcraw includes

#include <libkdcraw/version.h>

// Local includes

#include "pluginsversion.h"
#include "kpwriteimage.h"

namespace KIPIRawConverterPlugin
{

RawDecodingIface::RawDecodingIface()
                : KDcrawIface::KDcraw()
{
    m_updateFileTimeStamp = false;
}

RawDecodingIface::~RawDecodingIface()
{
}

void RawDecodingIface::setUpdateFileTimeStamp(bool b)
{
    m_updateFileTimeStamp = b;
}

bool RawDecodingIface::decodeHalfRAWImage(const QString& filePath,
                                          QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                                          const KDcrawIface::RawDecodingSettings& rawDecodingSettings)
{
    int width, height, rgbmax;
    QByteArray imageData;
    if (!KDcrawIface::KDcraw::decodeHalfRAWImage(filePath, rawDecodingSettings,
                                                 imageData, width, height, rgbmax))
        return false;

    return (loadedFromDcraw(filePath, destPath, outputFileFormat,
                            imageData, width, height, rgbmax, rawDecodingSettings));
}

bool RawDecodingIface::decodeRAWImage(const QString& filePath, 
                                      QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                                      const KDcrawIface::RawDecodingSettings& rawDecodingSettings)
{
    int width, height, rgbmax;
    QByteArray imageData;
    if (!KDcrawIface::KDcraw::decodeRAWImage(filePath, rawDecodingSettings,
                                             imageData, width, height, rgbmax))
        return false;

    return (loadedFromDcraw(filePath, destPath, outputFileFormat,
                            imageData, width, height, rgbmax, rawDecodingSettings));
}

// ----------------------------------------------------------------------------------

bool RawDecodingIface::loadedFromDcraw(const QString& filePath, 
                                       QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                                       const QByteArray& imageData, int width, int height, int rgbmax, 
                                       const KDcrawIface::RawDecodingSettings& rawDecodingSettings)
{
    bool sixteenBits = rawDecodingSettings.sixteenBitsImage;
    uchar* sptr      = (uchar*)imageData.data();
    float factor     = 65535.0 / rgbmax;
    uchar tmp8[2];
    unsigned short tmp16[3];

    // Set RGB color components.
    for (int i = 0 ; i < width * height ; i++)
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

#if KDCRAW_VERSION < 0x000400
            tmp16[0] = (unsigned short)((sptr[4]*256 + sptr[5]) * factor);      // Blue
            tmp16[1] = (unsigned short)((sptr[2]*256 + sptr[3]) * factor);      // Green
            tmp16[2] = (unsigned short)((sptr[0]*256 + sptr[1]) * factor);      // Red
#else
            tmp16[0] = (unsigned short)((sptr[5]*256 + sptr[4]) * factor);      // Blue
            tmp16[1] = (unsigned short)((sptr[3]*256 + sptr[2]) * factor);      // Green
            tmp16[2] = (unsigned short)((sptr[1]*256 + sptr[0]) * factor);      // Red
#endif

            memcpy(&sptr[0], &tmp16[0], 6);

            sptr += 6;
        }
    }

#if KDCRAW_VERSION < 0x000400

    // Special case: RAW decoded image is a linear-histogram image with 16 bits color depth.
    // No auto white balance and no gamma adjustemnts are performed. Image is a black hole.
    // We need to reproduce all dcraw 8 bits color depth adjustements here.

    if (sixteenBits && 
        rawDecodingSettings.outputColorSpace != KDcrawIface::RawDecodingSettings::RAWCOLOR)
    {
        // Compute histogram.

        unsigned short* image = (unsigned short*)imageData.data();
        int histogram[3][65536];
        memset(histogram, 0, sizeof(histogram));
        for (int i = 0 ; i < width * height ; i++)
        {
            for (int c = 0 ; c < 3 ; c++)
                histogram[c][image[c]]++;
            image += 3;
        }

        // Search 99th percentile white level.

        int perc, val, total;
        float white=0.0, r;
        unsigned short lut[65536];

        perc = (int)(width * height * 0.01);
        kDebug() << "White Level: " << perc ;
        for (int c =0  ; c < 3 ; c++)
        {
            total = 0;
            for (val = 65535 ; val > 256 ; --val)
                if ((total += histogram[c][val]) > perc) 
                    break;

            if (white < val) white = (float)val;
        }

        white *= 1.0 / rawDecodingSettings.brightness;
        kDebug() << "White Point: " << white ;

        // Compute the Gamma lut accordingly.

        for (int i=0; i < 65536; i++) 
        {
            r = i / white;
            val = (int)(65536.0 * (r <= 0.018 ? r*4.5 : pow(r,0.45)*1.099-0.099));
            if (val > 65535) val = 65535;
            lut[i] = val;
        }

        //  Apply Gamma lut to the whole image.

        unsigned short *im = (unsigned short *)imageData.data();
        for (int i = 0; i < width*height; i++)
        {
            im[0] = lut[im[0]];      // Blue
            im[1] = lut[im[1]];      // Green
            im[2] = lut[im[2]];      // Red
            im += 3;
        }
    }

#endif

    // Use a QImage instance to write IPTC preview and Exif thumbnail
    // and adapt color component order to KPWriteImage data format (RGB ==> BGR)

    QImage img(width, height, QImage::Format_ARGB32);
    uint*  dptr            = (uint*)img.bits();
    uchar* sptr8           = (uchar*)imageData.data();
    unsigned short* sptr16 = (unsigned short*)imageData.data();

    for (int i = 0 ; i < width * height ; i++)
    {
        if (!sixteenBits)   // 8 bits color depth image.
        {
            *dptr++ = qRgba(sptr8[2], sptr8[1], sptr8[0], 0xFF);
            sptr8 += 3;
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

    QByteArray prof = KIPIPlugins::KPWriteImage::getICCProfilFromFile(m_rawDecodingSettings.outputColorSpace);
    QString soft = QString("Kipi Raw Converter v.%1").arg(kipiplugins_version);
    QFileInfo fi(filePath);
    destPath = fi.absolutePath() + QString("/") + ".kipi-rawconverter-tmp-" 
                                 + QString::number(QDateTime::currentDateTime().toTime_t());

    // Metadata restoration and update.
    KExiv2Iface::KExiv2 meta;

#if KEXIV2_VERSION >= 0x000600
    meta.setUpdateFileTimeStamp(m_updateFileTimeStamp);
#endif

    meta.load(filePath);
    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    meta.setImageDimensions(QSize(width, height));
    meta.setExifThumbnail(exifThumbnail);

    // Update Iptc preview.
    // NOTE: see B.K.O #130525. a JPEG segment is limited to 64K. If the IPTC byte array is
    // bigger than 64K duing of image preview tag size, the target JPEG image will be
    // broken. Note that IPTC image preview tag is limited to 256K!!!
    // There is no limitation with TIFF and PNG about IPTC byte array size.
    if (outputFileFormat != SaveSettingsWidget::OUTPUT_JPEG)
        meta.setImagePreview(iptcPreview);

    meta.setExifTagString("Exif.Image.DocumentName", fi.fileName());
    meta.setXmpTagString("Xmp.tiff.Make", meta.getExifTagString("Exif.Image.Make"));
    meta.setXmpTagString("Xmp.tiff.Model", meta.getExifTagString("Exif.Image.Model"));

    // the image has already been rotated after being read from the raw format,
    // therefore reset the EXIF-tag:
    meta.setImageOrientation(KExiv2Iface::KExiv2::ORIENTATION_NORMAL);

    KIPIPlugins::KPWriteImage wImageIface;
    wImageIface.setImageData(imageData, width, height, sixteenBits, false, prof, meta);
    wImageIface.setCancel(&m_cancel);

    switch(outputFileFormat)
    {
        case SaveSettingsWidget::OUTPUT_JPEG:
        {
            if (!wImageIface.write2JPEG(destPath)) return false;
            break;
        }
        case SaveSettingsWidget::OUTPUT_PNG:
        {
            if (!wImageIface.write2PNG(destPath)) return false;
            break;
        }
        case SaveSettingsWidget::OUTPUT_TIFF:
        {
            if (!wImageIface.write2TIFF(destPath)) return false;
            break;
        }
        case SaveSettingsWidget::OUTPUT_PPM:
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
        ::remove(QFile::encodeName(destPath));
        return false;
    }

    return true;
}

}  // namespace KIPIRawConverterPlugin
