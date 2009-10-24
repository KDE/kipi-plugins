/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-14
 * Description : batch images grayscale conversion
 *
 * Copyright (C) 2004-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2003-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#define XMD_H

#include "convert2grayscale.h"
#include "convert2grayscale.moc"

// C++ includes

#include <cstdio>

// C ANSI includes

extern "C" 
{
#include <sys/types.h>
#include <unistd.h>
#include <jpeglib.h>
}

// Qt includes

#include <QFile>
#include <QFileInfo>

// KDE includes

#include <kprocess.h>
#include <klocale.h>
#include <kurl.h>
#include <kdebug.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes

#include "utils.h"
#include "pluginsversion.h"
#include "transupp.h"

namespace KIPIJPEGLossLessPlugin
{

ImageGrayScale::ImageGrayScale()
              : QObject()
{
    m_tmpFile.setSuffix("kipiplugin-grayscale");
    m_tmpFile.setAutoRemove(true);
}

ImageGrayScale::~ImageGrayScale()
{
}

bool ImageGrayScale::image2GrayScale(const QString& src, QString& err, bool updateFileTimeStamp)
{
    QFileInfo fi(src);

    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) 
    {
        err = i18n("Error in opening input file");
        return false;
    }

    if ( !m_tmpFile.open() )
    {
        err = i18n("Error in opening temporary file");
        return false;
    }

    QString tmp = m_tmpFile.fileName();

    if (Utils::isRAW(src))
    {
        err = i18n("Cannot convert to gray scale RAW file");
        return false;
    }
    else if (Utils::isJPEG(src))
    {
        if (!image2GrayScaleJPEG(src, tmp, err, updateFileTimeStamp))
            return false;
    }
    else
    {
        // B.K.O #123499 : using Image Magick API here instead QT API 
        // else RAW/TIFF/PNG 16 bits image are broken!
        if (!image2GrayScaleImageMagick(src, tmp, err))
            return false;

        // We update metadata on new image.
        Utils tools(this);
        if (!tools.updateMetadataImageMagick(tmp, err))
            return false;
    }

    // Move back to original file
    if (!Utils::moveOneFile(tmp, src)) 
    {
        err = i18n("Cannot update source image");
        return false;
    }

    return true;
}

bool ImageGrayScale::image2GrayScaleJPEG(const QString& src, const QString& dest, 
                                         QString& err, bool updateFileTimeStamp)
{
    JCOPY_OPTION copyoption = JCOPYOPT_ALL;
    jpeg_transform_info transformoption;

    transformoption.transform       = JXFORM_NONE;
    transformoption.force_grayscale = true;
    transformoption.trim            = false;

    struct jpeg_decompress_struct srcinfo;
    struct jpeg_compress_struct dstinfo;
    struct jpeg_error_mgr jsrcerr;
    struct jpeg_error_mgr jdsterr;
    jvirt_barray_ptr * src_coef_arrays;
    jvirt_barray_ptr * dst_coef_arrays;

    // Initialize the JPEG decompression object with default error handling
    srcinfo.err = jpeg_std_error(&jsrcerr);
    jpeg_create_decompress(&srcinfo);

    // Initialize the JPEG compression object with default error handling
    dstinfo.err = jpeg_std_error(&jdsterr);
    jpeg_create_compress(&dstinfo);

    FILE *input_file;
    FILE *output_file;

    input_file = fopen(QFile::encodeName(src), "rb");
    if (!input_file)
    {
        kError() << "Image2GrayScale: Error in opening input file";
        err = i18n("Error in opening input file");
        return false;
    }

    output_file = fopen(QFile::encodeName(dest), "wb");
    if (!output_file)
    {
        fclose(input_file);
        kError() << "Image2GrayScale: Error in opening output file";
        err = i18n("Error in opening output file");
        return false;
    }

    // Open jpeglib stream
    jpeg_stdio_src(&srcinfo, input_file);

    // Setup decompression object to save desired markers in memory
    jcopy_markers_setup(&srcinfo, copyoption);

    // Decompression startup: read start of JPEG datastream to see what's there
    (void) jpeg_read_header(&srcinfo, true);

    // Request any required workspace
    jtransform_request_workspace(&srcinfo, &transformoption);

    // Read source file as DCT coefficients
    src_coef_arrays = jpeg_read_coefficients(&srcinfo);

    // Initialize destination compression parameters from source values
    jpeg_copy_critical_parameters(&srcinfo, &dstinfo);

    // Adjust output image parameters
    dst_coef_arrays = jtransform_adjust_parameters(&srcinfo, &dstinfo, src_coef_arrays, &transformoption);

    // Specify data destination for compression
    jpeg_stdio_dest(&dstinfo, output_file);

    // Do not write a JFIF header if previously the image did not contain it
    dstinfo.write_JFIF_header = false;

    // Start compressor (note no image data is actually written here)
    jpeg_write_coefficients(&dstinfo, dst_coef_arrays);

    // Copy to the output file any extra markers that we want to preserve (merging from src file with Qt tmp file)
    jcopy_markers_execute(&srcinfo, &dstinfo, copyoption);

    // Execute the actual jpeg transformations
    jtransform_execute_transformation(&srcinfo, &dstinfo, src_coef_arrays, &transformoption);

    // Finish compression and release memory
    jpeg_finish_compress(&dstinfo);
    jpeg_destroy_compress(&dstinfo);
    (void) jpeg_finish_decompress(&srcinfo);
    jpeg_destroy_decompress(&srcinfo);

    fclose(input_file);
    fclose(output_file);

    // And set finaly update the metadata to target file.

    KExiv2Iface::KExiv2 exiv2Iface;

#if KEXIV2_VERSION >= 0x000600
    exiv2Iface.setUpdateFileTimeStamp(updateFileTimeStamp);
#endif

    exiv2Iface.load(dest);

    QImage img(dest);
    QImage exifThumbnail = img.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    exiv2Iface.setExifThumbnail(exifThumbnail);
    exiv2Iface.save(dest);

    return true;
}

bool ImageGrayScale::image2GrayScaleImageMagick(const QString& src, const QString& dest, QString& err)
{
    KProcess process;
    process.clearProgram();
    process << "convert";
    process << "-type" << "Grayscale";
    process << src + QString("[0]") << dest;

    kDebug() << "ImageMagick Command line: " << process.program();

    process.start();

    if (!process.waitForFinished())
        return false;

    if (process.exitStatus() != QProcess::NormalExit)
        return false;

    switch (process.exitCode())
    {
        case 0:  // Process finished successfully !
        {
            return true;
            break;
        }
        case 15: //  process aborted !
        {
            return false;
            break;
        }
    }

    // Processing error !
    m_stdErr = process.readAllStandardError();
    err      = i18n("Cannot convert to gray scale: %1", m_stdErr.replace('\n', ' '));
    return false;
}

}  // namespace KIPIJPEGLossLessPlugin
