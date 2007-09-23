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

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>

// C Ansi includes.

extern "C"
{
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <jpeglib.h>
#include <tiffio.h>
#include <tiffvers.h>
#include "iccjpeg.h"
}

// Qt Includes.

#include <qcstring.h>
#include <qfileinfo.h>

// KDE includes.

#include <kstandarddirs.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/dcrawbinary.h>

// Local includes.

#include "pluginsversion.h"
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

QByteArray RawDecodingIface::getICCProfilFromFile(KDcrawIface::RawDecodingSettings::OutputColorSpace colorSpace)
{    
    QString filePath;
    KGlobal::dirs()->addResourceType("profiles", KGlobal::dirs()->kde_default("data") + 
                                     "kipiplugin_rawconverter/profiles");

    switch(colorSpace)
    {
        case KDcrawIface::RawDecodingSettings::SRGB:
        {
            filePath = KGlobal::dirs()->findResourceDir("profiles", "srgb.icm");
            filePath.append("srgb.icm");
            break;
        }
        case KDcrawIface::RawDecodingSettings::ADOBERGB:
        {
            filePath = KGlobal::dirs()->findResourceDir("profiles", "adobergb.icm");
            filePath.append("adobergb.icm");
            break;
        }
        case KDcrawIface::RawDecodingSettings::WIDEGAMMUT:
        {
            filePath = KGlobal::dirs()->findResourceDir("profiles", "widegamut.icm");
            filePath.append("widegamut.icm");
            break;
        }
        case KDcrawIface::RawDecodingSettings::PROPHOTO:
        {
            filePath = KGlobal::dirs()->findResourceDir("profiles", "prophoto.icm");
            filePath.append("prophoto.icm");
            break;
        }
        default:
            break;
    }

    if ( filePath.isEmpty() ) 
        return QByteArray();

    QFile file(filePath);
    if ( !file.open(IO_ReadOnly) ) 
        return QByteArray();
    
    QByteArray data(file.size());
    QDataStream stream( &file );
    stream.readRawBytes(data.data(), data.size());
    file.close();
    return data;
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
                            imageData, width, height));
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
                            imageData, width, height));
}

// ----------------------------------------------------------------------------------

bool RawDecodingIface::loadedFromDcraw(const QString& filePath, 
                                       QString& destPath, SaveSettingsWidget::OutputFormat outputFileFormat,
                                       const QByteArray& imageData, int width, int height)
{
    // -- Use a QImage instance to write IPTC preview and Exif thumbnail -------

    QImage img(width, height, 32);
    uchar* dptr = img.bits();
    uchar* sptr = (uchar*)imageData.data();

    // Set RGB color components.
    for (int i = 0 ; i < width * height ; i++)
    {
        dptr[0] = sptr[2];
        dptr[1] = sptr[1];
        dptr[2] = sptr[0];
        dptr[3] = 0xFF;
        dptr += 4;
        sptr += 3;
    }

    QImage iptcPreview   = img.scale(800, 600, QImage::ScaleMin);
    QImage exifThumbnail = iptcPreview.scale(160, 120, QImage::ScaleMin);

    // -- Write image data into destination file -------------------------------

    QByteArray ICCColorProfile = getICCProfilFromFile(m_rawDecodingSettings.outputColorSpace);
    QString soft = QString("Kipi Raw Converter v.%1").arg(kipiplugins_version);
    QFileInfo fi(filePath);
    destPath = fi.dirPath(true) + QString("/") + ".kipi-rawconverter-tmp-" 
                                + QString::number(::time(0));

    // Metadata restoration and update.
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.load(filePath);
    exiv2Iface.setImageProgramId(QString("Kipi Raw Converter"), QString(kipiplugins_version));
    exiv2Iface.setImageDimensions(QSize(width, height));
    exiv2Iface.setExifThumbnail(exifThumbnail);
    exiv2Iface.setImagePreview(iptcPreview);
    exiv2Iface.setExifTagString("Exif.Image.DocumentName", fi.fileName());

    switch(outputFileFormat)
    {
        case SaveSettingsWidget::OUTPUT_JPEG:
        {
            FILE* f = 0;
            f = fopen(QFile::encodeName(destPath), "wb");
    
            if (!f) 
            {
                qDebug("Failed to open JPEG file for writing");
                return false;
            }
    
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr       jerr;
    
            int      row_stride;
            JSAMPROW row_pointer[1];

            // Init JPEG compressor.    
            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);
            jpeg_stdio_dest(&cinfo, f);
            cinfo.image_width      = width;
            cinfo.image_height     = height;
            cinfo.input_components = 3;
            cinfo.in_color_space   = JCS_RGB;
            jpeg_set_defaults(&cinfo);

            // B.K.O #149578: set encoder horizontal and vertical chroma subsampling 
            // factor to 2x1, 1x1, 1x1 (4:2:2) : Medium subsampling.
            // See this page for details: http://en.wikipedia.org/wiki/Chroma_subsampling 
            cinfo.comp_info[0].h_samp_factor = 2;
            cinfo.comp_info[0].v_samp_factor = 1;
            cinfo.comp_info[1].h_samp_factor = 1;
            cinfo.comp_info[1].v_samp_factor = 1;
            cinfo.comp_info[2].h_samp_factor = 1;
            cinfo.comp_info[2].v_samp_factor = 1;

            jpeg_set_quality(&cinfo, 100, true);
            jpeg_start_compress(&cinfo, true);

            // Write ICC color profil.
            if (!ICCColorProfile.isEmpty())
                write_icc_profile (&cinfo, (JOCTET *)ICCColorProfile.data(), ICCColorProfile.size());

            // Write image data
            row_stride = cinfo.image_width * 3;
            while (!m_cancel && (cinfo.next_scanline < cinfo.image_height))
            {
                row_pointer[0] = (uchar*)imageData.data() + (cinfo.next_scanline * row_stride);
                jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }
            
            jpeg_finish_compress(&cinfo);
            jpeg_destroy_compress(&cinfo);
            fclose(f);

            exiv2Iface.save(destPath);
            break;
        }

        case SaveSettingsWidget::OUTPUT_PNG:
        {
            FILE* f = 0;
            f = fopen(QFile::encodeName(destPath), "wb");
    
            if (!f) 
            {
                qDebug("Failed to open PNG file for writing");
                return false;
            }
    
            png_color_8 sig_bit;
            png_bytep   row_ptr;
            png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            png_infop info_ptr  = png_create_info_struct(png_ptr);
            png_init_io(png_ptr, f);
            png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
                        PNG_COLOR_TYPE_RGB,        PNG_INTERLACE_NONE, 
                        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
            sig_bit.red   = 8;
            sig_bit.green = 8;
            sig_bit.blue  = 8;
            sig_bit.alpha = 8;
            png_set_sBIT(png_ptr, info_ptr, &sig_bit);
            png_set_compression_level(png_ptr, 9);

            // Write ICC profil.
            if (!ICCColorProfile.isEmpty())
            {
                png_set_iCCP(png_ptr, info_ptr, "icc", PNG_COMPRESSION_TYPE_BASE, 
                             ICCColorProfile.data(), ICCColorProfile.size());
            }    

            QString libpngver(PNG_HEADER_VERSION_STRING);
            libpngver.replace('\n', ' ');
            soft.append(QString(" (%1)").arg(libpngver));
            png_text text;
            text.key  = "Software";
            text.text = (char *)soft.ascii();
            text.compression = PNG_TEXT_COMPRESSION_zTXt;
            png_set_text(png_ptr, info_ptr, &(text), 1);

            // Store Exif data.
            QByteArray ba = exiv2Iface.getExif();
            const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
            QByteArray profile = QByteArray(ba.size() + sizeof(ExifHeader));
            memcpy(profile.data(), ExifHeader, sizeof(ExifHeader));
            memcpy(profile.data()+sizeof(ExifHeader), ba.data(), ba.size());
            writeRawProfile(png_ptr, info_ptr, "exif", profile.data(), (png_uint_32) profile.size());

            // Store Iptc data.
            QByteArray ba2 = exiv2Iface.getIptc();
            writeRawProfile(png_ptr, info_ptr, "iptc", ba2.data(), (png_uint_32) ba2.size());

            png_write_info(png_ptr, info_ptr);
            png_set_shift(png_ptr, &sig_bit);
            png_set_packing(png_ptr);
            unsigned char* ptr = (unsigned char*)imageData.data();
    
            for (int y = 0; !m_cancel && (y < height); y++)
            {
                row_ptr = (png_bytep) ptr;
                png_write_rows(png_ptr, &row_ptr, 1);
                ptr += (width * 3);
            }
    
            png_write_end(png_ptr, info_ptr);
            png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
            png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
            fclose(f);
            break;
        }

        case SaveSettingsWidget::OUTPUT_TIFF:
        {
            TIFF          *tif=0;
            unsigned char *data=0;
            int            y;
            int            w;
            
            tif = TIFFOpen(QFile::encodeName(destPath), "wb");
    
            if (!tif) 
            {
                qDebug("Failed to open TIFF file for writing");
                return false;
            }
    
            TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,      width);
            TIFFSetField(tif, TIFFTAG_IMAGELENGTH,     height);
            TIFFSetField(tif, TIFFTAG_ORIENTATION,     ORIENTATION_TOPLEFT);
            TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
            TIFFSetField(tif, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
            TIFFSetField(tif, TIFFTAG_COMPRESSION,     COMPRESSION_ADOBE_DEFLATE);
            TIFFSetField(tif, TIFFTAG_ZIPQUALITY,      9);
            // NOTE : this tag values aren't defined in libtiff 3.6.1. '2' is PREDICTOR_HORIZONTAL.
            //        Use horizontal differencing for images which are
            //        likely to be continuous tone. The TIFF spec says that this
            //        usually leads to better compression.
            //        See this url for more details:
            //        http://www.awaresystems.be/imaging/tiff/tifftags/predictor.html
            TIFFSetField(tif, TIFFTAG_PREDICTOR,       2); 
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_RGB);
            w = TIFFScanlineSize(tif);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,    TIFFDefaultStripSize(tif, 0));


            // Store Exif data.
            // TODO

            // Store Iptc data.
            QByteArray ba2 = exiv2Iface.getIptc(true);
#if defined(TIFFTAG_PHOTOSHOP)
            TIFFSetField (tif, TIFFTAG_PHOTOSHOP,      (uint32)ba2.size(), (uchar *)ba2.data());
#endif

            QString libtiffver(TIFFLIB_VERSION_STR);
            libtiffver.replace('\n', ' ');
            soft.append(QString(" ( %1 )").arg(libtiffver));
            TIFFSetField(tif, TIFFTAG_SOFTWARE,        (const char*)soft.ascii());

            // Write ICC profil.
            if (!ICCColorProfile.isEmpty())
            {
#if defined(TIFFTAG_ICCPROFILE)    
                TIFFSetField(tif, TIFFTAG_ICCPROFILE, (uint32)ICCColorProfile.size(), 
                             (uchar *)ICCColorProfile.data());
#endif      
            }    

            // Write image data
            for (y = 0; !m_cancel && (y < height); y++)
            {
                data = (unsigned char*)imageData.data() + (y * width * 3);
                TIFFWriteScanline(tif, data, y, 0);
            }
    
            TIFFClose(tif);
            break;
        }

        case SaveSettingsWidget::OUTPUT_PPM:
        {
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

//----------------------------------------------------------------------------------

void RawDecodingIface::writeRawProfile(png_struct *ping, png_info *ping_info, char *profile_type, 
                                 char *profile_data, png_uint_32 length)
{
    png_textp      text;
    
    register long  i;
    
    uchar         *sp;
    
    png_charp      dp;
    
    png_uint_32    allocated_length, description_length;

    const uchar hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    
    qDebug("Writing Raw profile: type=%s, length=%i", profile_type, (int)length);
    
    text               = (png_textp) png_malloc(ping, (png_uint_32) sizeof(png_text));
    description_length = strlen((const char *) profile_type);
    allocated_length   = (png_uint_32) (length*2 + (length >> 5) + 20 + description_length);
    
    text[0].text   = (png_charp) png_malloc(ping, allocated_length);
    text[0].key    = (png_charp) png_malloc(ping, (png_uint_32) 80);
    text[0].key[0] = '\0';
    
    concatenateString(text[0].key, "Raw profile type ", 4096);
    concatenateString(text[0].key, (const char *) profile_type, 62);
    
    sp = (uchar*)profile_data;
    dp = text[0].text;
    *dp++='\n';
    
    copyString(dp, (const char *) profile_type, allocated_length);
    
    dp += description_length;
    *dp++='\n';
    
    formatString(dp, allocated_length-strlen(text[0].text), "%8lu ", length);
    
    dp += 8;
    
    for (i=0; i < (long) length; i++)
    {
        if (i%36 == 0)
            *dp++='\n';

        *(dp++)=(char) hex[((*sp >> 4) & 0x0f)];
        *(dp++)=(char) hex[((*sp++ ) & 0x0f)]; 
    }

    *dp++='\n';
    *dp='\0';
    text[0].text_length = (png_size_t) (dp-text[0].text);
    text[0].compression = -1;

    if (text[0].text_length <= allocated_length)
        png_set_text(ping, ping_info,text, 1);

    png_free(ping, text[0].text);
    png_free(ping, text[0].key);
    png_free(ping, text);
}

size_t RawDecodingIface::concatenateString(char *destination, const char *source, const size_t length)
{
    register char       *q;
    
    register const char *p;
    
    register size_t      i;
    
    size_t               count;
  
    if ( !destination || !source || length == 0 )
        return 0;

    p = source;
    q = destination;
    i = length;

    while ((i-- != 0) && (*q != '\0'))
        q++;

    count = (size_t) (q-destination);
    i     = length-count;

    if (i == 0)
        return(count+strlen(p));

    while (*p != '\0')
    {
        if (i != 1)
        {
            *q++=(*p);
            i--;
        }
        p++;
    }

    *q='\0';
    
    return(count+(p-source));
}

size_t RawDecodingIface::copyString(char *destination, const char *source, const size_t length)
{
    register char       *q;
    
    register const char *p;
    
    register size_t      i;
        
    if ( !destination || !source || length == 0 )
        return 0;

    p = source;
    q = destination;
    i = length;

    if ((i != 0) && (--i != 0))
    {
        do
        {
            if ((*q++=(*p++)) == '\0')
                break;
        } 
        while (--i != 0);
    }

    if (i == 0)
    {
        if (length != 0)
            *q='\0';
  
        while (*p++ != '\0') {
            ;
        }
    }
    
    return((size_t) (p-source-1));
}

long RawDecodingIface::formatString(char *string, const size_t length, const char *format,...)
{
    long n;
    
    va_list operands;
    
    va_start(operands,format);
    n = (long) formatStringList(string, length, format, operands);
    va_end(operands);
    return(n);
}

long RawDecodingIface::formatStringList(char *string, const size_t length, const char *format, va_list operands)
{
    int n = vsnprintf(string, length, format, operands);
    
    if (n < 0)
        string[length-1] = '\0';
    
    return((long) n);
}

}  // namespace KIPIRawConverterPlugin
