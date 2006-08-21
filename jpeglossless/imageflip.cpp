/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2003-10-14
 * Description : batch image flip
 *
 * Copyright 2003-2006 by Gilles Caulier
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
#include <cassert>
#include <string>

// C Ansi includes.

extern "C" 
{
#include <sys/types.h>
#include <unistd.h>
#include <jpeglib.h>
}

// Qt includes.

#include <qimage.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kdebug.h>
#include <kurl.h>

// Lib KExif inlcudes.

#include <libkexif/kexifdata.h>

// ImageMgick includes.

#include <Magick++.h>

// Local includes

#include "transupp.h"
#include "jpegtransform.h"
#include "utils.h"
#include "imageflip.h"

namespace KIPIJPEGLossLessPlugin
{

bool flip(const QString& src, FlipAction action, const QString& TmpFolder, QString& err)
{
    QFileInfo fi(src);
    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) 
    {
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
        // B.K.O #123499 : we using Image Magick API here instead QT API 
        // else RAW/TIFF/PNG 16 bits image are broken!
        if (!flipImageMagick(src, tmp, action, err))
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

bool flipJPEG(const QString& src, const QString& dest, FlipAction action, QString& err)
{
    Matrix &transform=Matrix::none;

    switch(action)
    {
        case (FlipHorizontal):
        {
            transform=Matrix::flipHorizontal;
            break;
        }
        case (FlipVertical):
        {
            transform=Matrix::flipVertical;
            break;
        }
        default:
        {
            kdError() << "ImageFlip: Nonstandard flip action" << endl;
            err = i18n("Nonstandard flip action");
            return false;
        }
    }

    return transformJPEG(src, dest, transform, err);
}

bool flipImageMagick(const QString& src, const QString& dest, FlipAction action, QString& err)
{
    try 
    {
        Magick::Image image;
        std::string srcFileName(QFile::encodeName(src));
        image.read(srcFileName);

        switch(action)
        {
            case (FlipHorizontal):
            {
                image.flop();
                break;
            }
            case (FlipVertical):
            {
                image.flip();
                break;
            }
            default:
            {
                kdError() << "ImageFlip: Nonstandard flip action" << endl;
                err = i18n("Nonstandard flip action");
                return false;
            }
        }
    
        std::string destFileName(QFile::encodeName(dest));
        image.write(destFileName);
        return true;
    }
    catch( std::exception &error_ )
    {
        err = i18n("Cannot flip: %1").arg(error_.what());
        kdError() << "ImageFlip: ImageMagick exception: " << error_.what() << endl;
        return false;
    }
}

}  // NameSpace KIPIJPEGLossLessPlugin

