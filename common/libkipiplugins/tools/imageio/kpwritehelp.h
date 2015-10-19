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
#include <png.h>
}

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

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
