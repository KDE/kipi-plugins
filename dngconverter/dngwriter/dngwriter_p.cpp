/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QFile>

// KDE includes

#include <kdebug.h>

// Local includes

#include "dngwriter.h"
#include "dngwriter_p.h"

namespace DNGIface
{

DNGWriterPrivate::DNGWriterPrivate()
{
    reset();
}

DNGWriterPrivate::~DNGWriterPrivate()
{
}

void DNGWriterPrivate::reset()
{
    cancel                  = false;
    jpegLossLessCompression = true;
    updateFileDate          = false;
    backupOriginalRawFile   = false;
    previewMode             = DNGWriter::MEDIUM;
}

void DNGWriterPrivate::cleanup()
{
    ::remove(QFile::encodeName(outputFile));
}

}  // namespace DNGIface
