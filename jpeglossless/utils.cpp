/* ============================================================
 * File  : utils.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-12-03
 * Description :
 *
 * Copyright 2003 by Renchi Raju

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

#include <kdebug.h>

#include <qimage.h>
#include <qstring.h>
#include <qfile.h>

extern "C" {
#include <tiffio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
}

#include "utils.h"

namespace KIPIJPEGLossLessPlugin
{

bool QImageToTiff(const QImage& image, const QString& dst)
{
    TIFF               *tif;
    unsigned char      *data;
    int                 x, y;
    QRgb                rgb;

    tif = TIFFOpen(QFile::encodeName(dst).data(), "w");
    if (tif)
    {
        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, image.width());
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, image.height());
        TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_ADOBE_DEFLATE);
        {
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, 0));

            data = new unsigned char[image.width()*3];
            unsigned char *dptr = 0;

            for (y = 0; y < image.height(); y++)
            {
                dptr = data;
                for (x = 0; x < image.width(); x++) {
                    rgb = *((uint *)image.scanLine(y) + x);
                    *(dptr++) = qRed(rgb);
                    *(dptr++) = qGreen(rgb);
                    *(dptr++) = qBlue(rgb);
                }
                TIFFWriteScanline(tif, data, y, 0);
            }

            delete [] data;

        }
        TIFFClose(tif);

        return true;
    }

    return false;
}

bool isJPEG(const QString& file)
{
    QString format=QString(QImageIO::imageFormat(file)).upper();
    return format=="JPEG";
}


bool CopyFile(const QString& src, const QString& dst)
{
    QFile sFile(src);
    QFile dFile(dst);

    if ( !sFile.open(IO_ReadOnly) )
        return false;

    if ( !dFile.open(IO_WriteOnly) )
    {
        sFile.close();
        return false;
    }

    const int MAX_IPC_SIZE = (1024*32);
    char buffer[MAX_IPC_SIZE];

    Q_LONG len;
    while ((len = sFile.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if (len == -1 || dFile.writeBlock(buffer, (Q_ULONG)len) == -1)
        {
            sFile.close();
            dFile.close();
            return false;
        }
    }

    sFile.close();
    dFile.close();

    return true;
}

bool MoveFile(const QString& src, const QString& dst)
{
    struct stat stbuf;
    if (::stat(QFile::encodeName(dst), &stbuf) != 0)
    {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:MoveFile: failed to stat src"
                           << endl;
        return false;
    }
    
    if (!CopyFile(src,dst))
        return false;

    struct utimbuf timbuf;
    timbuf.actime = stbuf.st_atime;
    timbuf.modtime = stbuf.st_mtime;
    if (::utime(QFile::encodeName(dst), &timbuf) != 0)
    {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:MoveFile: failed to update dst time"
                           << endl;
    }
    
    if (::unlink(QFile::encodeName(src).data()) != 0)
    {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:MoveFile: failed to unlink src"
                           << endl;
    }
    return true;
}

}  // NameSpace KIPIJPEGLossLessPlugin
