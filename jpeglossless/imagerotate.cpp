/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-10-14
 * Description : batch image rotation
 * 
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#define XMD_H

// C++ includes.

#include <cstdio>
#include <cstdlib>

// Qt includes.

#include <qimage.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <qfile.h>
#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kdebug.h>
#include <kurl.h>

// Lib KExif includes. 

#include <libkexif/kexifdata.h>

// Local includes.

#include "imagerotate.h"
#include "utils.h"

// C Ansi includes.

extern "C" 
{
#include <sys/types.h>
#include <unistd.h>
#include <jpeglib.h>
#include <wand/magick-wand.h>
#include "transupp.h"
#include "jpegtransform.h"
}

namespace KIPIJPEGLossLessPlugin
{

bool rotate(const QString& src, RotateAction angle, const QString& TmpFolder, QString& err)
{
    QFileInfo fi(src);

    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) 
    {
        err = i18n("Error in opening input file");
        return false;
    }

    // Generate temporary filename 
    QString tmp = TmpFolder + "imagerotation-" + fi.fileName();
    
    if (isJPEG(src))
    {
        if (!rotateJPEG(src, tmp, angle, err))
            return false;
    }
    else
    {
        // B.K.O #123499 : we using Image Magick API here instead QT API 
        // else RAW/TIFF/PNG 16 bits image are broken!
        if (!rotateImageMagick(src, tmp, angle, err))
            return false;
    }

    // Move back to original file
    if (!MoveFile(tmp, src)) 
    {
        err = i18n("Cannot update source image");
        return false;
    }

    return true;
}

bool rotateJPEG(const QString& src, const QString& dest, RotateAction angle, QString& err)
{
    Matrix &transform=Matrix::none;

    switch(angle)
    {
        case (Rot90):
        {
            transform = Matrix::rotate90;
            break;
        }
        case (Rot180):
        {
            transform = Matrix::rotate180;
            break;
        }
        case (Rot270):
        {
            transform = Matrix::rotate270;
            break;
        }
        case (Rot0):
        {
            transform = Matrix::none;
            break;
        }
        default:
        {
            kdError() << "ImageRotate: Nonstandard rotation angle" << endl;
            err = i18n("Nonstandard rotation angle");
            return false;
        }
    }

    return transformJPEG(src, dest, transform, err);
}

bool rotateImageMagick(const QString& src, const QString& dest, RotateAction angle, QString& err)
{
    bool valRet = true;
    MagickWandGenesis();
    PixelWand *background    = NewPixelWand();
    MagickWand *magickWand   = NewMagickWand();  
    MagickBooleanType status = MagickReadImage(magickWand, QFile::encodeName(src));
    
    if (status == MagickFalse)
    {
        err = i18n("Failed to load original image");
        valRet = false;
        goto Exit_Method;
    }

    PixelSetColor(background, "#000000");

    switch(angle)
    {
        case (Rot90):
        {   
            status = MagickRotateImage(magickWand, background, 90.0);
            break;
        }
        case (Rot180):
        {
            status = MagickRotateImage(magickWand, background, 180.0);
            break;
        }
        case (Rot270):
        {
            status = MagickRotateImage(magickWand, background, 270.0);
            break;
        }
        case (Rot0):
        {
            break;
        }
        default:
        {
            kdError() << "ImageRotate: Nonstandard rotation angle" << endl;
            err = i18n("Nonstandard rotation angle");
            valRet = false;
            goto Exit_Method;
        }
    }

    if (status == MagickFalse)
    {
        err = i18n("Failed to process image rotation image");
        valRet = false;
        goto Exit_Method;
    }

    status = MagickWriteImages(magickWand, QFile::encodeName(dest), MagickTrue);
    
    if (status == MagickFalse)
    {
        err = i18n("Failed to save target image");
        valRet = false;
        goto Exit_Method;
    }

Exit_Method:

    if (background) DestroyPixelWand(background);
    MagickClearException(magickWand);
    if (magickWand) DestroyMagickWand(magickWand);
    MagickWandTerminus();
    return valRet;
}

}  // NameSpace KIPIJPEGLossLessPlugin
