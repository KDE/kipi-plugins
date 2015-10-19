/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-19-09
 * Description : Kipi-Plugins shared library.
 *               Interface to write image data to common picture format.
 *
 * Copyright (C) 2007-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

//#define ENABLE_DEBUG_MESSAGES 1

#include "kpwriteimage.h"

// C ANSI includes

extern "C"
{
#ifndef _WIN32
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif
#include <sys/types.h>
#include <tiffvers.h>
}

// Qt includes

#include <QByteArray>
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>

// Local includes

#include "kpversion.h"
#include "kpmetasettings.h"
#include "kipiplugins_debug.h"

namespace KIPIPlugins
{

class KPWriteImage::Private
{
public:

    Private()
    {
        sixteenBit     = false;
        hasAlpha       = false;
        width          = 0;
        height         = 0;
        cancel         = 0;
        kipipluginsVer = QStringLiteral("Kipi-plugins v.%1").arg(kipipluginsVersion());
    }

    bool*                cancel;

    bool                 sixteenBit;
    bool                 hasAlpha;

    uint                 width;
    uint                 height;

    QByteArray           data;         // BGR(A) image data
                                       // data[0] = blue, data[1] = green, data[2] = red, data[3] = alpha.
    QByteArray           iccProfile;   // ICC color profile data.

    QString              kipipluginsVer;

    KPMetadata           meta;
};

KPWriteImage::KPWriteImage()
    : d(new Private)
{
}

KPWriteImage::~KPWriteImage()
{
    delete d;
}

int KPWriteImage::bytesDepth() const
{
    if (d->sixteenBit)
    {
        if (d->hasAlpha)
            return 8;
        else
            return 6;
    }

    if (d->hasAlpha)
        return 4;

    return 3;
}

void KPWriteImage::setCancel(bool* const cancel)
{
    d->cancel = cancel;
}

bool KPWriteImage::cancel() const
{
    if (d->cancel)
        return *d->cancel;

    return false;
}

void KPWriteImage::setImageData(const QByteArray& data, uint width, uint height,
                                bool  sixteenBit, bool hasAlpha,
                                const QByteArray& iccProfile,
                                const KPMetadata& metadata)
{
    d->data       = data;
    d->width      = width;
    d->height     = height;
    d->sixteenBit = sixteenBit;
    d->hasAlpha   = hasAlpha;
    d->iccProfile = iccProfile;
    d->meta       = metadata;
    d->meta.setSettings(metadata.settings());
}

bool KPWriteImage::write2TIFF(const QString& destPath)
{
    uint32 w          = d->width;
    uint32 h          = d->height;
    uchar* const data = (uchar*)d->data.data();

    // TIFF error handling. If an errors/warnings occurs during reading,
    // libtiff will call these methods

    TIFFSetWarningHandler(kipi_tiff_warning);
    TIFFSetErrorHandler(kipi_tiff_error);

    // Open target file

    TIFF* const tif = TIFFOpen((const char*)(QFile::encodeName(destPath)).constData(), "w");

    if (!tif)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Failed to open TIFF file for writing" ;
        return false;
    }

    int bitsDepth = d->sixteenBit ? 16 : 8;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,          w);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH,         h);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,         PHOTOMETRIC_RGB);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG,        PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_ORIENTATION,         ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT,      RESUNIT_NONE);
    TIFFSetField(tif, TIFFTAG_COMPRESSION,         COMPRESSION_ADOBE_DEFLATE);
    TIFFSetField(tif, TIFFTAG_ZIPQUALITY,          9);
    // NOTE : this tag values aren't defined in libtiff 3.6.1. '2' is PREDICTOR_HORIZONTAL.
    //        Use horizontal differencing for images which are
    //        likely to be continuous tone. The TIFF spec says that this
    //        usually leads to better compression.
    //        See this url for more details:
    //        http://www.awaresystems.be/imaging/tiff/tifftags/predictor.html
    TIFFSetField(tif, TIFFTAG_PREDICTOR,           2);

    uint16 sampleinfo[1];

    if (d->hasAlpha)
    {
        sampleinfo[0] = EXTRASAMPLE_ASSOCALPHA;
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 4);
        TIFFSetField(tif, TIFFTAG_EXTRASAMPLES,    1, sampleinfo);
    }
    else
    {
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    }

    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,       (uint16)bitsDepth);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,        TIFFDefaultStripSize(tif, 0));

    // Store Iptc data.
    QByteArray ba2 = d->meta.getIptc(true);
#if defined(TIFFTAG_PHOTOSHOP)
    TIFFSetField (tif, TIFFTAG_PHOTOSHOP, (uint32)ba2.size(), (uchar *)ba2.data());
#endif

    // Store Xmp data.
    QByteArray ba3 = d->meta.getXmp();
#if defined(TIFFTAG_XMLPACKET)
    TIFFSetField(tif, TIFFTAG_XMLPACKET, (uint32)ba3.size(), (uchar *)ba3.data());
#endif

    // Standard Exif ASCII tags (available with libtiff 3.6.1)

    tiffSetExifAsciiTag(tif, TIFFTAG_DOCUMENTNAME,     d->meta, "Exif.Image.DocumentName");
    tiffSetExifAsciiTag(tif, TIFFTAG_IMAGEDESCRIPTION, d->meta, "Exif.Image.ImageDescription");
    tiffSetExifAsciiTag(tif, TIFFTAG_MAKE,             d->meta, "Exif.Image.Make");
    tiffSetExifAsciiTag(tif, TIFFTAG_MODEL,            d->meta, "Exif.Image.Model");
    tiffSetExifAsciiTag(tif, TIFFTAG_DATETIME,         d->meta, "Exif.Image.DateTime");
    tiffSetExifAsciiTag(tif, TIFFTAG_ARTIST,           d->meta, "Exif.Image.Artist");
    tiffSetExifAsciiTag(tif, TIFFTAG_COPYRIGHT,        d->meta, "Exif.Image.Copyright");

    QString libtiffver(QStringLiteral(TIFFLIB_VERSION_STR));
    libtiffver.replace(QLatin1Char('\n'), QLatin1Char(' '));
    QString soft = d->kipipluginsVer;
    soft.append(QStringLiteral(" ( %1 )").arg(libtiffver));
    TIFFSetField(tif, TIFFTAG_SOFTWARE, (const char*)soft.toLatin1().data());

    // Write ICC profile.
    if (!d->iccProfile.isEmpty())
    {
#if defined(TIFFTAG_ICCPROFILE)
        TIFFSetField(tif, TIFFTAG_ICCPROFILE, (uint32)d->iccProfile.size(),
                     (uchar *)d->iccProfile.data());
#endif
    }

    // Write full image data in tiff directory IFD0

    uchar*  pixel=0;
    double  alpha_factor;
    uint32  x, y;
    uint8   r8, g8, b8, a8=0;
    uint16  r16, g16, b16, a16=0;
    int     i=0;

    uint8* const buf = (uint8 *)_TIFFmalloc(TIFFScanlineSize(tif));

    if (!buf)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Cannot allocate memory buffer for main TIFF image." ;
        TIFFClose(tif);
        return false;
    }

    for (y = 0; y < h; ++y)
    {
        if (cancel())
        {
            _TIFFfree(buf);
            TIFFClose(tif);
            return false;
        }

        i = 0;

        for (x = 0; x < w; ++x)
        {
            pixel = &data[((y * w) + x) * bytesDepth()];

            if ( d->sixteenBit )        // 16 bits image.
            {
                b16 = (uint16)(pixel[0]+256*pixel[1]);
                g16 = (uint16)(pixel[2]+256*pixel[3]);
                r16 = (uint16)(pixel[4]+256*pixel[5]);

                if (d->hasAlpha)
                {
                    // TIFF makes you pre-multiply the rgb components by alpha

                    a16          = (uint16)(pixel[6]+256*pixel[7]);
                    alpha_factor = ((double)a16 / 65535.0);
                    r16          = (uint16)(r16*alpha_factor);
                    g16          = (uint16)(g16*alpha_factor);
                    b16          = (uint16)(b16*alpha_factor);
                }

                // This might be endian dependent

                buf[i++] = (uint8)(r16);
                buf[i++] = (uint8)(r16 >> 8);
                buf[i++] = (uint8)(g16);
                buf[i++] = (uint8)(g16 >> 8);
                buf[i++] = (uint8)(b16);
                buf[i++] = (uint8)(b16 >> 8);

                if (d->hasAlpha)
                {
                    buf[i++] = (uint8)(a16) ;
                    buf[i++] = (uint8)(a16 >> 8) ;
                }
            }
            else                            // 8 bits image.
            {
                b8 = (uint8)pixel[0];
                g8 = (uint8)pixel[1];
                r8 = (uint8)pixel[2];

                if (d->hasAlpha)
                {
                    // TIFF makes you pre-multiply the rgb components by alpha

                    a8           = (uint8)(pixel[3]);
                    alpha_factor = ((double)a8 / 255.0);
                    r8           = (uint8)(r8*alpha_factor);
                    g8           = (uint8)(g8*alpha_factor);
                    b8           = (uint8)(b8*alpha_factor);
                }

                // This might be endian dependent

                buf[i++] = r8;
                buf[i++] = g8;
                buf[i++] = b8;

                if (d->hasAlpha)
                    buf[i++] = a8;
            }
        }

        if (!TIFFWriteScanline(tif, buf, y, 0))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Cannot write main TIFF image to target file." ;
            _TIFFfree(buf);
            TIFFClose(tif);
            return false;
        }
    }

    _TIFFfree(buf);
    TIFFWriteDirectory(tif);

    // Write thumbnail in tiff directory IFD1

    QImage thumb = d->meta.getExifThumbnail(false);

    if (!thumb.isNull())
    {
        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,      (uint32)thumb.width());
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH,     (uint32)thumb.height());
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_RGB);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_ORIENTATION,     ORIENTATION_TOPLEFT);
        TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT,  RESUNIT_NONE);
        TIFFSetField(tif, TIFFTAG_COMPRESSION,     COMPRESSION_NONE);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,    TIFFDefaultStripSize(tif, 0));

        uchar* pixelThumb      = 0;
        uchar* const dataThumb = thumb.bits();
        uint8* const bufThumb  = (uint8 *) _TIFFmalloc(TIFFScanlineSize(tif));

        if (!bufThumb)
        {
            qCDebug(KIPIPLUGINS_LOG) << "Cannot allocate memory buffer for TIFF thumbnail.";
            TIFFClose(tif);
            return false;
        }

        for (y = 0 ; y < uint32(thumb.height()) ; ++y)
        {
            i = 0;

            for (x = 0 ; x < uint32(thumb.width()) ; ++x)
            {
                pixelThumb = &dataThumb[((y * thumb.width()) + x) * 4];

                // This might be endian dependent
                bufThumb[i++] = (uint8)pixelThumb[2];
                bufThumb[i++] = (uint8)pixelThumb[1];
                bufThumb[i++] = (uint8)pixelThumb[0];
            }

            if (!TIFFWriteScanline(tif, bufThumb, y, 0))
            {
                qCDebug(KIPIPLUGINS_LOG) << "Cannot write TIFF thumbnail to target file." ;
                _TIFFfree(bufThumb);
                TIFFClose(tif);
                return false;
            }
        }

        _TIFFfree(bufThumb);
    }

    TIFFClose(tif);

    // Store metadata (Exiv2 0.18 support tiff writing mode)
    d->meta.save(destPath);

    return true;
}


void KPWriteImage::tiffSetExifAsciiTag(TIFF* const tif, ttag_t tiffTag,
                                       const KPMetadata& meta,
                                       const char* exifTagName)
{
    QByteArray tag = meta.getExifTagData(exifTagName);

    if (!tag.isEmpty())
    {
        QByteArray str(tag.data(), tag.size());
        TIFFSetField(tif, tiffTag, (const char*)(str.constData()));
    }
}

void KPWriteImage::tiffSetExifDataTag(TIFF* const tif, ttag_t tiffTag,
                                      const KPMetadata& meta,
                                      const char* exifTagName)
{
    QByteArray tag = meta.getExifTagData(exifTagName);

    if (!tag.isEmpty())
    {
        TIFFSetField (tif, tiffTag, (uint32)tag.size(), (char *)tag.data());
    }
}

// To manage Errors/Warnings handling provide by libtiff

void KPWriteImage::kipi_tiff_warning(const char* module, const char* format, va_list warnings)
{
#ifdef ENABLE_DEBUG_MESSAGES
    char message[4096];
    vsnprintf(message, 4096, format, warnings);
    qCDebug(KIPIPLUGINS_LOG) << module << "::" << message ;
#else
    Q_UNUSED(module);
    Q_UNUSED(format);
    Q_UNUSED(warnings);
#endif
}

void KPWriteImage::kipi_tiff_error(const char* module, const char* format, va_list errors)
{
#ifdef ENABLE_DEBUG_MESSAGES
    char message[4096];
    vsnprintf(message, 4096, format, errors);
    qCDebug(KIPIPLUGINS_LOG) << module << "::" << message ;
#else
    Q_UNUSED(module);
    Q_UNUSED(format);
    Q_UNUSED(errors);
#endif
}

}  // namespace KIPIPlugins
