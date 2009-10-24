/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-06-08
 * Description : Lossless JPEG files transformations.
 *
 * Copyright (C) 2004 by  Ralf Hoelzer <kde at ralfhoelzer.com>
 * Copyright (C) 2004-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

// C++ includes

#include <cstdio>
#include <cstdlib>

// C ANSI includes

extern "C"
{
#include <sys/types.h>
#include <unistd.h>
#include <setjmp.h>
#include <jpeglib.h>
}

// Qt includes

#include <QFile>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <ktemporaryfile.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>

// Local includes

#include "pluginsversion.h"
#include "transupp.h"
#include "jpegtransform.h"

namespace KIPIJPEGLossLessPlugin
{

const Matrix Matrix::none                   ( 1,  0,  0,  1);
const Matrix Matrix::rotate90               ( 0, -1,  1,  0);
const Matrix Matrix::rotate180              (-1,  0,  0, -1);
const Matrix Matrix::rotate270              ( 0,  1, -1,  0);
const Matrix Matrix::flipHorizontal         (-1,  0,  0,  1);
const Matrix Matrix::flipVertical           ( 1,  0,  0, -1);
const Matrix Matrix::rotate90flipHorizontal ( 0,  1,  1,  0);
const Matrix Matrix::rotate90flipVertical   ( 0, -1, -1,  0);

// To manage Errors/Warnings handling provide by libjpeg

//#define ENABLE_DEBUG_MESSAGES

struct jpegtransform_jpeg_error_mgr : public jpeg_error_mgr
{
    jmp_buf setjmp_buffer;
};

static void jpegtransform_jpeg_error_exit(j_common_ptr cinfo);
static void jpegtransform_jpeg_emit_message(j_common_ptr cinfo, int msg_level);
static void jpegtransform_jpeg_output_message(j_common_ptr cinfo);

static void jpegtransform_jpeg_error_exit(j_common_ptr cinfo)
{
    jpegtransform_jpeg_error_mgr* myerr = (jpegtransform_jpeg_error_mgr*) cinfo->err;

    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);

#ifdef ENABLE_DEBUG_MESSAGES
    kDebug() << buffer;
#endif

    longjmp(myerr->setjmp_buffer, 1);
}

static void jpegtransform_jpeg_emit_message(j_common_ptr cinfo, int msg_level)
{
    Q_UNUSED(msg_level)
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);

#ifdef ENABLE_DEBUG_MESSAGES
    kDebug() << buffer << " (" << msg_level << ")";
#endif
}

static void jpegtransform_jpeg_output_message(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);

#ifdef ENABLE_DEBUG_MESSAGES
    kDebug() << buffer;
#endif
}

static bool transformJPEG(const QString& src, const QString& destGiven,
                          JXFORM_CODE flip, JXFORM_CODE rotate, QString& err);

bool transformJPEG(const QString& src, const QString& dest,
                   Matrix &userAction, QString& err, bool updateFileTimeStamp)
{
    // Get Exif orientation action to do.
    KExiv2Iface::KExiv2 exiv2Iface;

    Matrix exifAction, action;
    JXFORM_CODE flip,rotate;

#if KEXIV2_VERSION >= 0x000600
    exiv2Iface.setUpdateFileTimeStamp(updateFileTimeStamp);
#endif

    exiv2Iface.load(src);
    getExifAction(exifAction, exiv2Iface.getImageOrientation());

    // Compose actions: first exif, then user
    action*=exifAction;
    action*=userAction;

    // Convert action into flip+rotate action
    convertTransform(action, flip, rotate);
    kDebug() << "Transforming with option " << flip << " " << rotate;
    if (!transformJPEG(src, dest, flip, rotate, err))
        return false;

    // And set finaly update the metadata to target file.

    QImage img(dest);
    QImage exifThumbnail = img.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    exiv2Iface.load(dest);
    exiv2Iface.setImageOrientation(KExiv2Iface::KExiv2::ORIENTATION_NORMAL);
    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    exiv2Iface.setImageDimensions(img.size());
    exiv2Iface.setExifThumbnail(exifThumbnail);
    exiv2Iface.save(dest);

    return true;
}

bool transformJPEG(const QString& src, const QString& destGiven,
                   JXFORM_CODE flip, JXFORM_CODE rotate, QString& err)
{
    if (flip == JXFORM_NONE && rotate == JXFORM_NONE)
    {
        // no-op, just copy
        if (src != destGiven)
        {
            QFile::remove(destGiven);
            QFile file(src);
            if (!file.copy(destGiven))
            {
                err = file.errorString();
                return false;
            }
        }
        return true;
    }

    //may be modified
    QString dest(destGiven);

    JCOPY_OPTION copyoption = JCOPYOPT_ALL;
    jpeg_transform_info transformoption;

    transformoption.force_grayscale = false;
    transformoption.trim            = false;

    struct jpeg_decompress_struct srcinfo;
    struct jpeg_compress_struct dstinfo;
    struct jpegtransform_jpeg_error_mgr jsrcerr, jdsterr;
    jvirt_barray_ptr * src_coef_arrays;
    jvirt_barray_ptr * dst_coef_arrays;

    // Initialize the JPEG decompression object with default error handling
    srcinfo.err                 = jpeg_std_error(&jsrcerr);
    srcinfo.err->error_exit     = jpegtransform_jpeg_error_exit;
    srcinfo.err->emit_message   = jpegtransform_jpeg_emit_message;
    srcinfo.err->output_message = jpegtransform_jpeg_output_message;

    // Initialize the JPEG compression object with default error handling
    dstinfo.err                 = jpeg_std_error(&jdsterr);
    dstinfo.err->error_exit     = jpegtransform_jpeg_error_exit;
    dstinfo.err->emit_message   = jpegtransform_jpeg_emit_message;
    dstinfo.err->output_message = jpegtransform_jpeg_output_message;

    FILE *input_file;
    FILE *output_file;

    input_file = fopen(QFile::encodeName(src), "rb");
    if (!input_file)
    {
        kError() << "ImageRotate/ImageFlip: Error in opening input file";
        err = i18n("Error in opening input file");
        return false;
    }

    output_file = fopen(QFile::encodeName(dest), "wb");
    if (!output_file)
    {
        fclose(input_file);
        kError() << "ImageRotate/ImageFlip: Error in opening output file";
        err = i18n("Error in opening output file");
        return false;
    }

    if (setjmp(jsrcerr.setjmp_buffer) || setjmp(jdsterr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&srcinfo);
        jpeg_destroy_compress(&dstinfo);
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    jpeg_create_decompress(&srcinfo);
    jpeg_create_compress(&dstinfo);

    jpeg_stdio_src(&srcinfo, input_file);
    jcopy_markers_setup(&srcinfo, copyoption);

    (void) jpeg_read_header(&srcinfo, true);

    bool twoPass = (flip != JXFORM_NONE);

    // If twoPass is true, we need another file (src -> tempFile -> destGiven)
    if (twoPass) 
    {
        KTemporaryFile tempFile;
        tempFile.setAutoRemove(false);
        tempFile.open();
        dest = tempFile.fileName();
    }

    output_file = fopen(QFile::encodeName(dest), "wb");
    if (!output_file)
    {
        fclose(input_file);
        kError() << "ImageRotate/ImageFlip: Error in opening output file";
        err = i18n("Error in opening output file");
        return false;
    }

    // First rotate - execute even if rotate is JXFORM_NONE to apply new EXIF settings
    transformoption.transform = rotate;

    jtransform_request_workspace(&srcinfo, &transformoption);

     // Read source file as DCT coefficients
    src_coef_arrays = jpeg_read_coefficients(&srcinfo);

    // Initialize destination compression parameters from source values
    jpeg_copy_critical_parameters(&srcinfo, &dstinfo);

    dst_coef_arrays = jtransform_adjust_parameters(&srcinfo, &dstinfo,
                                                   src_coef_arrays, &transformoption);

    // Specify data destination for compression
    jpeg_stdio_dest(&dstinfo, output_file);

    // Do not write a JFIF header if previously the image did not contain it
    dstinfo.write_JFIF_header = false;

    // Start compressor (note no image data is actually written here)
    jpeg_write_coefficients(&dstinfo, dst_coef_arrays);

    // Copy to the output file any extra markers that we want to preserve
    jcopy_markers_execute(&srcinfo, &dstinfo, copyoption);

    jtransform_execute_transformation(&srcinfo, &dstinfo,
                                      src_coef_arrays, &transformoption);

    // Finish compression and release memory
    jpeg_finish_compress(&dstinfo);
    jpeg_destroy_compress(&dstinfo);
    (void) jpeg_finish_decompress(&srcinfo);
    jpeg_destroy_decompress(&srcinfo);

    fclose(input_file);
    fclose(output_file);

    // Flip if needed
    if (twoPass) 
    {
        // Initialize the JPEG decompression object with default error handling
        srcinfo.err = jpeg_std_error(&jsrcerr);
        jpeg_create_decompress(&srcinfo);

        // Initialize the JPEG compression object with default error handling
        dstinfo.err = jpeg_std_error(&jdsterr);
        jpeg_create_compress(&dstinfo);

        input_file = fopen(QFile::encodeName(dest), "rb");
        if (!input_file)
        {
            kError() << "ImageRotate/ImageFlip: Error in opening input file";
            err = i18n("Error in opening input file");
            return false;
        }

        output_file = fopen(QFile::encodeName(destGiven), "wb");
        if (!output_file)
        {
            fclose(input_file);
            kError() << "ImageRotate/ImageFlip: Error in opening output file";
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

        jtransform_execute_transformation(&srcinfo, &dstinfo,
                                          src_coef_arrays, &transformoption);

        // Finish compression and release memory
        jpeg_finish_compress(&dstinfo);
        jpeg_destroy_compress(&dstinfo);
        (void) jpeg_finish_decompress(&srcinfo);
        jpeg_destroy_decompress(&srcinfo);

        fclose(input_file);
        fclose(output_file);

        // Unlink temp file
        unlink(QFile::encodeName(dest));
    }

    return true;
}

/** Converts the mathematically correct description
    into the primitive operations that can be carried out losslessly.
*/
void convertTransform(Matrix &action, JXFORM_CODE &flip, JXFORM_CODE &rotate) 
{
    flip   = JXFORM_NONE;
    rotate = JXFORM_NONE;

    if (action == Matrix::rotate90) 
    {
        rotate = JXFORM_ROT_90;
    }
    else if (action == Matrix::rotate180) 
    {
        rotate = JXFORM_ROT_180;
    }
    else if (action == Matrix::rotate270) 
    {
        rotate = JXFORM_ROT_270;
    }
    else if (action == Matrix::flipHorizontal) 
    {
        flip = JXFORM_FLIP_H;
    }
    else if (action == Matrix::flipVertical) 
    {
        flip = JXFORM_FLIP_V;
    }
    else if (action == Matrix::rotate90flipHorizontal) 
    {
        //first rotate, then flip!
        rotate = JXFORM_ROT_90;
        flip   = JXFORM_FLIP_H;
    }
    else if (action == Matrix::rotate90flipVertical) 
    {
        //first rotate, then flip!
        rotate = JXFORM_ROT_90;
        flip   = JXFORM_FLIP_V;
    }
}

void getExifAction(Matrix &action, KExiv2Iface::KExiv2::ImageOrientation exifOrientation) 
{
    switch (exifOrientation) 
    {
        case KExiv2Iface::KExiv2::ORIENTATION_NORMAL:
            break;

        case KExiv2Iface::KExiv2::ORIENTATION_HFLIP:
            action*=Matrix::flipHorizontal;
            break;

        case KExiv2Iface::KExiv2::ORIENTATION_ROT_180:
            action*=Matrix::rotate180;
            break;

        case KExiv2Iface::KExiv2::ORIENTATION_VFLIP:
            action*=Matrix::flipVertical;
            break;

        case KExiv2Iface::KExiv2::ORIENTATION_ROT_90_HFLIP:
            action*=Matrix::rotate90flipHorizontal;
            break;

        case KExiv2Iface::KExiv2::ORIENTATION_ROT_90:
            action*=Matrix::rotate90;
            break;

        case KExiv2Iface::KExiv2::ORIENTATION_ROT_90_VFLIP:
            action*=Matrix::rotate90flipVertical;
            break;

        case KExiv2Iface::KExiv2::ORIENTATION_ROT_270:
            action*=Matrix::rotate270;
            break;

        case KExiv2Iface::KExiv2::ORIENTATION_UNSPECIFIED:
            action*=Matrix::none;
            break;
    }
}

}  // namespace KIPIJPEGLossLessPlugin
