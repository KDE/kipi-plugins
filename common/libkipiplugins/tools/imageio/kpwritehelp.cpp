/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-13-11
 * Description : Kipi-Plugins shared library.
 *               Helper functions for writing images
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * Copyright (C) 2009-2010 by Patrick Spendrin <ps_ml@gmx.de>
 *
 * This file is based on jdatadst.c from libjpeg.
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

#include "kpwritehelp.h"

// Qt includes

#include <QIODevice>

// Local includes

#include "kipiplugins_debug.h"

/* choose an efficiently fwrite'able size */
#define BUFFER_SIZE  4096

namespace KIPIPlugins
{

/*
 * png stuff
 */

void kp_png_write_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    QIODevice* out = (QIODevice*)png_get_io_ptr(png_ptr);
    uint nr        = out->write((char*)data, length);

    if (nr != length)
    {
        png_error(png_ptr, "Write Error");
        return;
    }
}

void kp_png_flush_fn(png_structp png_ptr)
{
    Q_UNUSED(png_ptr);
}

}  // namespace KIPIPlugins
