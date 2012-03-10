/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-13-11
 * Description : Kipi-Plugins shared library.
 *               Helper functions for writing images
 *
 * Copyright (C) 2009-2010 by Patrick Spendrin <ps_ml@gmx.de>
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

#ifndef KPWRITEHELP_H
#define KPWRITEHELP_H

// C++ includes

#include <cstdio>
 
// Qt includes 
 
#include <QtCore/QIODevice>

// C ANSI includes

extern "C"
{
#include <jpeglib.h>
#include <png.h>
}

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

/**
  * a replacement function for jpeg_stdio_dest
  * for convenience reasons, it uses a QIODevice instead of a QFile, but the main advantage is to not give over
  * a FILE* pointer on Windows which can break due to different MS C Runtime libraries.
  *
  * Prepare for output to a QIODevice.
  * The caller must have already opened the device, and is responsible
  * for closing it after finishing compression.
  */
KIPIPLUGINS_EXPORT void kp_jpeg_qiodevice_dest(j_compress_ptr cinfo, QIODevice* const outfile);

/**
  * a replacement function for jpeg_stdio_src
  * for convenience reasons, it uses a QIODevice instead of a QFile, but the main advantage is to not give over
  * a FILE* pointer on Windows which can break due to different MS C Runtime libraries.
  *
  * Prepare for input from a QIODevice.
  * The caller must have already opened the device, and is responsible
  * for closing it after finishing reading.
  */
KIPIPLUGINS_EXPORT void kp_jpeg_qiodevice_src(j_decompress_ptr cinfo, QIODevice* const infile);

/**
  * a callback function for writing a png image
  */
KIPIPLUGINS_EXPORT void kp_png_write_fn(png_structp png_ptr, png_bytep data, png_size_t length);

/**
  * a callback function for flushing the buffers, currently unused, since no buffering happens
  */
KIPIPLUGINS_EXPORT void kp_png_flush_fn(png_structp png_ptr);

}  // namespace KIPIPlugins

#endif // KPWRITEHELP_H
