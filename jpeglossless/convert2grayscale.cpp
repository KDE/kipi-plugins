/* ============================================================
 * File  : convert2grayscale.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-10-14
 * Description : batch images grayscale conversion
 *
 * Copyright 2003 by Gilles Caulier

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <klocale.h>
#include <kdebug.h>
#include <kurl.h>
#include <kimageeffect.h>

#include <qimage.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <qfile.h>
#include <qfileinfo.h>

#include "convert2grayscale.h"
#include "utils.h"

#define XMD_H

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <jpeglib.h>
#include "transupp.h"
}

namespace KIPIJPEGLossLessPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool image2GrayScale(const QString& src, const QString& TmpFolder, QString& err)
{
    QFileInfo fi(src);
    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) {
        err = i18n("Error in opening input file");
        return false;
    }

    // Generate temporary filename 
    QString tmp = TmpFolder + "convert2grayscale-" + fi.fileName();

    if (isJPEG(src))
    {
        if (!image2GrayScaleJPEG(src, tmp, err))
            return false;
    }
    else
    {
        if (!image2GrayScaleQImage(src, tmp, err))
            return false;
    }

    // Move back to original file
    if (!MoveFile(tmp, src)) {
        err = i18n("Cannot update source image");
        return false;
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool image2GrayScaleJPEG(const QString& src, const QString& dest, QString& err)
{
    JCOPY_OPTION copyoption = JCOPYOPT_ALL;
    jpeg_transform_info transformoption;

    transformoption.transform = JXFORM_NONE;
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
        kdError() << "Image2GrayScale: Error in opening input file" << endl;
        err = i18n("Error in opening input file");
        return false;
    }

    output_file = fopen(QFile::encodeName(dest), "wb");
    if (!output_file)
    {
        fclose(input_file);
        kdError() << "Image2GrayScale: Error in opening output file" << endl;
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

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool image2GrayScaleQImage(const QString& src, const QString& dest,
                           QString& err)
{
    QImage image(src);
    if (image.isNull()) {
        err = i18n("Error in opening input file");
        return false;
    }

    image = KImageEffect::toGray(image);

    if (QString(QImageIO::imageFormat(src)).upper() == QString("TIFF")) {
        QImageToTiff(image, dest);
    }
    else
        image.save(dest, QImageIO::imageFormat(src));

    return true;
}

}  // NameSpace KIPIJPEGLossLessPlugin
