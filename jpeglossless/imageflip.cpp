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
#include "jpegtransform.h"
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
