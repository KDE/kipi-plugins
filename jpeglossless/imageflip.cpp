/* ============================================================
 * File  : imageflip.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-10-14
 * Description : batch image flip
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
#include <libkexif/kexifdata.h>

#include <qimage.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>

#include "imageflip.h"
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

namespace JPEGLossLess
{

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool flip(const QString& src, FlipAction action,
          const QString& TmpFolder, QString& err)
{
    QFileInfo fi(src);
    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) {
        err = i18n("Error in opening input file");
        return false;
    }

    /* Generate temporary filename */
    QString tmp = TmpFolder + "imageflip-" + fi.fileName();

    if (isJPEG(src))
    {
        if (!flipJPEG(src, tmp, action, err))
            return false;
    }
    else
    {
        if (!flipQImage(src, tmp, action, err))
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
    
bool flipJPEG(const QString& src, const QString& dest,
              FlipAction action, QString& err)
{
    JCOPY_OPTION copyoption = JCOPYOPT_ALL;
    jpeg_transform_info transformoption;

    switch(action)
    {
    case (FlipHorizontal):
    {
        transformoption.transform = JXFORM_FLIP_H;
        break;
    }
    case (FlipVertical):
    {
        transformoption.transform = JXFORM_FLIP_V;
        break;
    }
    default:
    {
        kdError() << "ImageFlip: Nonstandard flip action" << endl;
        err = i18n("Nonstandard flip action");
        return false;
    }
    }

    transformoption.force_grayscale = false;
    transformoption.trim            = false;

    struct jpeg_decompress_struct srcinfo;
    struct jpeg_compress_struct dstinfo;
    struct jpeg_error_mgr jsrcerr, jdsterr;
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

    input_file = fopen(src.latin1(), "rb");
    if (!input_file)
    {
        kdError() << "ImageFlip: Error in opening input file" << endl;
        err = i18n("Error in opening input file");
        return false;        
    }

    output_file = fopen(dest.latin1(), "wb");
    if (!output_file)
    {
        fclose(input_file);
        kdError() << "ImageFlip: Error in opening output file" << endl;
        err = i18n("Error in opening output file");
        return false;
    }

    jpeg_stdio_src(&srcinfo, input_file);
    jcopy_markers_setup(&srcinfo, copyoption);

    (void) jpeg_read_header(&srcinfo, true);

    jtransform_request_workspace(&srcinfo, &transformoption);

     // Read source file as DCT coefficients
    src_coef_arrays = jpeg_read_coefficients(&srcinfo);

    // Initialize destination compression parameters from source values
    jpeg_copy_critical_parameters(&srcinfo, &dstinfo);

    dst_coef_arrays = jtransform_adjust_parameters(&srcinfo,
                                                   &dstinfo,
                                                   src_coef_arrays,
                                                   &transformoption);

    // Specify data destination for compression
    jpeg_stdio_dest(&dstinfo, output_file);
    
    // Do not write a JFIF header if previously the image did not contain it
    dstinfo.write_JFIF_header = false;

    // Start compressor (note no image data is actually written here)
    jpeg_write_coefficients(&dstinfo, dst_coef_arrays);

    // Copy to the output file any extra markers that we want to preserve
    jcopy_markers_execute(&srcinfo, &dstinfo, copyoption);

    jtransform_execute_transformation(&srcinfo,
                                      &dstinfo,
                                      src_coef_arrays,
                                      &transformoption);

    // Finish compression and release memory
    jpeg_finish_compress(&dstinfo);
    jpeg_destroy_compress(&dstinfo);
    (void) jpeg_finish_decompress(&srcinfo);
    jpeg_destroy_decompress(&srcinfo);

    fclose(input_file);
    fclose(output_file);

    // set the EXIF orientation to normal (top-left), assuming the user
    // flipped the image to its correct orientation. 
    KExifData *exifData = new KExifData;
    exifData->writeOrientation(dest, KExifData::NORMAL);
    delete exifData;
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////


bool flipQImage(const QString& src, const QString& dest,
               FlipAction action, QString& err)
{
    bool vertical;
    
    switch(action)
    {
    case (FlipHorizontal):
    {
        vertical = false;        
        break;
    }
    case (FlipVertical):
    {
        vertical = true;
        break;
    }
    default:
    {
        kdError() << "ImageFlip: Nonstandard flip action" << endl;
        err = i18n("Nonstandard flip action");
        return false;
    }
    }

    QImage image(src);
    if (image.isNull()) {
        err = i18n("Error in opening input file");
        return false;
    }

    if (vertical) {

        // Vertical mirror
        int bpl = image.bytesPerLine();
        int y1, y2;
        for (y1 = 0, y2 = image.height()-1; y1 < y2; y1++, y2--) {
            uchar* a1 = image.scanLine(y1);
            uchar* a2 = image.scanLine(y2);
            for (int x = bpl; x > 0; x--) {
                uchar c = *a1;
                *a1++ = *a2;
                *a2++ = c;
            }
        }
    }
    else {

        // Horizontal mirror
        if (image.depth() != 32)
            image = image.convertDepth(32);
        for (int y = image.height()-1; y >= 0; y--) {
            uint* a1 = (uint *)image.scanLine(y);
            uint* a2 = a1+image.width()-1;
            while (a1 < a2) {
                uint c = *a1;
                *a1++ = *a2;
                *a2-- = c;
            }
        }
    }

    if (QString(QImageIO::imageFormat(src)).upper() == QString("TIFF")) {
        QImageToTiff(image, dest);
    }
    else
        image.save(dest, QImageIO::imageFormat(src));

    return true;
}


}
