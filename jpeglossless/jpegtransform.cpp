/* ============================================================
 * File   : jpegtransform.cpp
 * Authors: Marcel Wiesweg
 *          Ralf Hoelzer
 * Date   : 2004-06-08
 * 
 * Copyright 2004 by Renchi Raju

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

#include <qstring.h>
#include <qwmatrix.h>

#include <kdebug.h>
#include <klocale.h>
#include <ktempfile.h>
#include <libkexif/kexifdata.h>

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <jpeglib.h>
#include "transupp.h"
#include "jpegtransform.h"
}


namespace JPEGLossLess
{

Matrix Matrix::none                   (1, 0, 0, 1);
Matrix Matrix::rotate90               (0,-1,1,0);
Matrix Matrix::rotate180              (-1,0,0,-1);
Matrix Matrix::rotate270              (0,1,-1,0);
Matrix Matrix::flipHorizontal         (-1,0,0,1);
Matrix Matrix::flipVertical           (1,0,0,-1);
Matrix Matrix::rotate90flipHorizontal (0,1,1,0);
Matrix Matrix::rotate90flipVertical   (0,-1,-1,0);


bool transformJPEG(const QString& src, const QString& destGiven,
                   Matrix &userAction, QString& err)
{
    //may be modified
    QString dest(destGiven);   
    
    JCOPY_OPTION copyoption = JCOPYOPT_ALL;
    jpeg_transform_info transformoption;

    transformoption.force_grayscale = false;
    transformoption.trim            = false;

    struct jpeg_decompress_struct srcinfo;
    struct jpeg_compress_struct dstinfo;
    struct jpeg_error_mgr jsrcerr, jdsterr;
    jvirt_barray_ptr * src_coef_arrays;
    jvirt_barray_ptr * dst_coef_arrays;

    Matrix exifAction, action;
    JXFORM_CODE flip,rotate;

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
        kdError() << "ImageRotate/ImageFlip: Error in opening input file" << endl;
        err = i18n("Error in opening input file");
        return false;
    }

    output_file = fopen(dest.latin1(), "wb");
    if (!output_file)
    {
        fclose(input_file);
        kdError() << "ImageRotate/ImageFlip: Error in opening output file" << endl;
        err = i18n("Error in opening output file");
        return false;
    }

    jpeg_stdio_src(&srcinfo, input_file);
    jcopy_markers_setup(&srcinfo, copyoption);

    (void) jpeg_read_header(&srcinfo, true);

    KExifData *exifData = new KExifData;
    exifData->readFromFile(src);

    getExifAction(exifAction, exifData->getImageOrientation());


    // Compose actions: first exif, then user
    action*=exifAction;
    action*=userAction;

    //Convert action into flip+rotate action
    convertTransform(action, flip, rotate);
    kdDebug() << "Transforming with option " << flip << " " << rotate <<endl;
    bool twoPass = (flip != JXFORM_NONE);

    //If twoPass is true, we need another file (src -> tempFile -> destGiven)
    if (twoPass) {
        KTempFile tempFile;
        tempFile.setAutoDelete(false);
        dest=tempFile.name();
    }

    output_file = fopen(dest.latin1(), "wb");
    if (!output_file)
    {
        fclose(input_file);
        kdError() << "ImageRotate/ImageFlip: Error in opening output file" << endl;
        err = i18n("Error in opening output file");
        return false;
    }

    // First rotate - execute even if rotate is JXFORM_NONE to apply new EXIF settings
    transformoption.transform=rotate;

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

    // Flip if needed
    if (twoPass) {
        // Initialize the JPEG decompression object with default error handling
        srcinfo.err = jpeg_std_error(&jsrcerr);
        jpeg_create_decompress(&srcinfo);

        // Initialize the JPEG compression object with default error handling
        dstinfo.err = jpeg_std_error(&jdsterr);
        jpeg_create_compress(&dstinfo);

        input_file = fopen(dest.latin1(), "rb");
        if (!input_file)
        {
            kdError() << "ImageRotate/ImageFlip: Error in opening input file" << endl;
            err = i18n("Error in opening input file");
            return false;
        }

        output_file = fopen(destGiven.latin1(), "wb");
        if (!output_file)
        {
            fclose(input_file);
            kdError() << "ImageRotate/ImageFlip: Error in opening output file" << endl;
            err = i18n("Error in opening output file");
            return false;
        }

        jpeg_stdio_src(&srcinfo, input_file);
        jcopy_markers_setup(&srcinfo, copyoption);

        (void) jpeg_read_header(&srcinfo, true);

        transformoption.transform=flip;
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

        //unlink temp file
        unlink(dest.latin1());
    }

    exifData->writeOrientation(destGiven, KExifData::NORMAL);

    delete exifData;

    return true;
}

/*
   Converts the mathematically correct description
   into the primitive operations that can be carried out losslessly.
*/
static void convertTransform(Matrix &action, JXFORM_CODE &flip, JXFORM_CODE &rotate) {
   flip=JXFORM_NONE;
   rotate=JXFORM_NONE;

   if (action==Matrix::rotate90) {
      rotate=JXFORM_ROT_90;
   } else if (action==Matrix::rotate180) {
      rotate=JXFORM_ROT_180;
   } else if (action==Matrix::rotate270) {
      rotate=JXFORM_ROT_270;
   } else if (action==Matrix::flipHorizontal) {
      flip=JXFORM_FLIP_H;
   } else if (action==Matrix::flipVertical) {
      flip=JXFORM_FLIP_V;
   } else if (action==Matrix::rotate90flipHorizontal) {
      //first rotate, then flip!
      rotate=JXFORM_ROT_90;
      flip=JXFORM_FLIP_H;
   } else if (action==Matrix::rotate90flipVertical) {
      //first rotate, then flip!
      rotate=JXFORM_ROT_90;
      flip=JXFORM_FLIP_V;
   }
}


static void getExifAction(Matrix &action, KExifData::ImageOrientation exifOrientation) {
    switch (exifOrientation) {
        case KExifData::NORMAL:
            break;

        case KExifData::HFLIP:
            action*=Matrix::flipHorizontal;
            break;

        case KExifData::ROT_180:
            action*=Matrix::rotate180;
            break;

        case KExifData::VFLIP:
            action*=Matrix::flipVertical;
            break;

        case KExifData::ROT_90_HFLIP:
            action*=Matrix::rotate90flipHorizontal;
            break;

        case KExifData::ROT_90:
            action*=Matrix::rotate90;
            break;

        case KExifData::ROT_90_VFLIP:
            action*=Matrix::rotate90flipVertical;
            break;

        case KExifData::ROT_270:
            action*=Matrix::rotate270;
            break;
        case KExifData::UNSPECIFIED:
            action*=Matrix::none;
            break;
    }
}



}
