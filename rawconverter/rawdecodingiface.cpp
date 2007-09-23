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
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

// C++ includes.

#include <cstdio> 

// Qt Includes.

#include <QByteArray>
#include <QDateTime>
#include <QFileInfo>

// KDE includes.

#include <kstandarddirs.h>

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/dcrawbinary.h>

// Local includes.

#include "pluginsversion.h"
#include "kpwriteimage.h"
#include "rawdecodingiface.h"
#include "rawdecodingiface.moc"

namespace KIPIRawConverterPlugin
{

RawDecodingIface::RawDecodingIface()
                : KDcrawIface::KDcraw()
{
}

RawDecodingIface::~RawDecodingIface()
{
}

bool RawDecodingIface::decodeHalfRAWImage(const QString& filePath, 
                                          QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                                          KDcrawIface::RawDecodingSettings rawDecodingSettings)
{
    int width, height, rgbmax;
    QByteArray imageData;
    if (!KDcrawIface::KDcraw::decodeHalfRAWImage(filePath, rawDecodingSettings, 
                                                 imageData, width, height, rgbmax))
        return false;

    return (loadedFromDcraw(filePath, destPath, outputFileFormat, 
                            imageData, width, height, rgbmax, rawDecodingSettings.sixteenBitsImage));
}

bool RawDecodingIface::decodeRAWImage(const QString& filePath, 
                                      QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                                      KDcrawIface::RawDecodingSettings rawDecodingSettings)
{
    int width, height, rgbmax;
    QByteArray imageData;
    if (!KDcrawIface::KDcraw::decodeRAWImage(filePath, rawDecodingSettings, 
                                             imageData, width, height, rgbmax))
        return false;

    return (loadedFromDcraw(filePath, destPath, outputFileFormat, 
                            imageData, width, height, rgbmax, rawDecodingSettings.sixteenBitsImage));
}

// ----------------------------------------------------------------------------------

bool RawDecodingIface::loadedFromDcraw(const QString& filePath, 
                                       QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                                       const QByteArray& imageData, int width, int height, int rgbmax, bool sixteenBits)
{
    // Use a QImage instance to write IPTC preview and Exif thumbnail
    // and adapt color component order to KPWriteImage data format (RGB ==> BGR)

    QImage img(width, height, QImage::Format_ARGB32);
    uint*  dptr  = (uint*)img.bits();
    uchar* sptr  = (uchar*)imageData.data();
    float factor = 65535.0 / rgbmax;
    uchar tmp8[2];
    unsigned short tmp16[3];

    // Set RGB color components.
    for (int i = 0 ; i < width * height ; i++)
    {
        if (!sixteenBits)
        {
            *dptr++ = qRgba(sptr[0], sptr[1], sptr[2], 0xFF);

            // Swap Red and Blue
            tmp8[0] = sptr[2];
            tmp8[1] = sptr[0];
            sptr[0] = tmp8[0];
            sptr[2] = tmp8[1];

            sptr += 3;
        }
        else
        {
            *dptr++ = qRgba((uchar)(((sptr[0]*256 + sptr[1]) * factor * 255UL)/65535UL),
                            (uchar)(((sptr[2]*256 + sptr[3]) * factor * 255UL)/65535UL),
                            (uchar)(((sptr[4]*256 + sptr[5]) * factor * 255UL)/65535UL),
                            0xFF);

            // Swap Red and Blue and re-ajust color component values?
            tmp16[0] = (unsigned short)((sptr[4]*256 + sptr[5]) * factor);      // Blue
            tmp16[1] = (unsigned short)((sptr[2]*256 + sptr[3]) * factor);      // Green
            tmp16[2] = (unsigned short)((sptr[0]*256 + sptr[1]) * factor);      // Red

            memcpy(&sptr[0], &tmp16[0], 6);

            sptr += 6;
        }
    }

    QImage iptcPreview   = img.scaled(800, 600, Qt::KeepAspectRatio);
    QImage exifThumbnail = iptcPreview.scaled(160, 120, Qt::KeepAspectRatio);

    // -- Write image data into destination file -------------------------------

    QByteArray prof = KIPIPlugins::KPWriteImage::getICCProfilFromFile(m_rawDecodingSettings.outputColorSpace);
    QString soft = QString("Kipi Raw Converter v.%1").arg(kipiplugins_version);
    QFileInfo fi(filePath);
    destPath = fi.absolutePath() + QString("/") + ".kipi-rawconverter-tmp-" 
                                 + QString::number(QDateTime::currentDateTime().toTime_t());

    // Metadata restoration and update.
    KExiv2Iface::KExiv2 meta;
    meta.load(filePath);
    meta.setImageProgramId(QString("Kipi Raw Converter"), QString(kipiplugins_version));
    meta.setImageDimensions(QSize(width, height));
    meta.setExifThumbnail(exifThumbnail);
    meta.setImagePreview(iptcPreview);
    meta.setExifTagString("Exif.Image.DocumentName", fi.fileName());

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
            if (sixteenBits) return false;

            FILE* f = fopen(QFile::encodeName(destPath), "wb");
            if (!f) 
            {
                qDebug("Failed to open ppm file for writing");
                return false;
            }
    
            fprintf(f, "P6\n%d %d\n255\n", width, height);
            fwrite(imageData.data(), 1, width*height*3, f);
            fclose(f);
            break;
        }
        default:
        {
            qDebug("Invalid output file format");
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
