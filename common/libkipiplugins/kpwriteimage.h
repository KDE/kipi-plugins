/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-19-09
 * Description : Kipi-Plugins shared library.
 *               Interface to write image data to common picture format.
 * 
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KIPI_WRITE_IMAGE_H
#define KIPI_WRITE_IMAGE_H

// Libkipi includes.

#include <libkipi/libkipi_export.h>

// LibKDcraw includes.

#include <libkdcraw/rawdecodingsettings.h>

namespace KIPIPlugins
{

class LIBKIPI_EXPORT KPWriteImage
{
public:
 
    KPWriteImage(){};
    ~KPWriteImage(){};

private: 

    QByteArray getICCProfilFromFile(KDcrawIface::RawDecodingSettings::OutputColorSpace colorSpace);

};

}  // NameSpace KIPIPlugins

#endif /* KIPI_WRITE_IMAGE_H */
