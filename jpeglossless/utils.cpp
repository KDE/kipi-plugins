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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
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
    int fdin, fdout;
    struct stat statbuf;
    char *s, *d;

    // open the input file
    if ((fdin = ::open(QFile::encodeName(src).data(), O_RDONLY)) < 0) {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:CopyFile: Failed to open input file"
                    << endl;
	return false;
    }

    // find size of input file
    if (::fstat (fdin,&statbuf) < 0) {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:CopyFile: Failed to stat input file"
                    << endl;
	return false;
    }

    // open/create the output file
    if ((fdout = ::open(QFile::encodeName(dst).data(), O_RDWR | O_CREAT | O_TRUNC,
                       statbuf.st_mode)) < 0) {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:CopyFile: Failed to open output file"
                    << endl;
        ::close(fdin);
        return false;
    }


    // go to the location corresponding to the last byte
    if (::lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1) {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:CopyFile: lseek error"
                    << endl;
	return false;
    }

    // write a dummy byte at the last location
    if (::write(fdout, "", 1) != 1) {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:CopyFile: write error"
                    << endl;
	return false;
    }

    // mmap the input file
    if ((s = (char*)::mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0))
	== (caddr_t) -1) {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:CopyFile: mmap error for input"
                    << endl;
	return false;
    }

    // mmap the output file
    if ((d = (char*)::mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE,
                           MAP_SHARED, fdout, 0)) == (caddr_t) -1) {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:CopyFile: mmap error for output"
                    << endl;
	return false;
    }

    // this copies the input file to the output file
    ::memcpy (d, s, statbuf.st_size);

    // munmap the input/ouput
    ::munmap(s, statbuf.st_size);
    ::munmap(d, statbuf.st_size);

    // close the file descriptors
    ::close(fdin);
    ::close(fdout);

    return true;
}

bool MoveFile(const QString& src, const QString& dst)
{
    if (!CopyFile(src,dst)) return false;

    if (::unlink(QFile::encodeName(src).data()) != 0) {
        kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin:MoveFile: failed to unlink src"
                    << endl;
    }
    return true;
}

}  // NameSpace KIPIJPEGLossLessPlugin
