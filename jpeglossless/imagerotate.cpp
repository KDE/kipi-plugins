/* ============================================================
 * File  : imagerotate.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-10-14
 * Description : batch image rotation
 *
 * Copyright 2003 by Renchi Raju

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
#include <qwmatrix.h>
#include <qfile.h>
#include <qfileinfo.h>

#include "imagerotate.h"
#include "utils.h"

#define XMD_H

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

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool rotate(const QString& src, RotateAction angle, const QString& TmpFolder,
            QString& err)
{
    QFileInfo fi(src);
    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) {
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
        if (!rotateQImage(src, tmp, angle, err))
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

bool rotateJPEG(const QString& src, const QString& dest,
                RotateAction angle, QString& err)
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


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool rotateQImage(const QString& src, const QString& dest,
                 RotateAction angle, QString& err)
{
    QWMatrix m;

    switch(angle)
    {
    case (Rot90):
    {
        m.rotate(90);
        break;
    }
    case (Rot180):
    {
        m.rotate(180);        
        break;
    }
    case (Rot270):
    {
        m.rotate(270);
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
        return false;
    }
    }

    QImage image(src);
    if (image.isNull()) {
        err = i18n("Error in opening input file");
        return false;
    }

    image = image.xForm(m);

    if (QString(QImageIO::imageFormat(src)).upper() == QString("TIFF")) {
        QImageToTiff(image, dest);
    }
    else
        image.save(dest, QImageIO::imageFormat(src));

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////


}
