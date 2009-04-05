/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-19
 * Description : Autodetect gpsbabel binary program and version
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GPSBABELBINARY_H
#define GPSBABELBINARY_H

// Qt includes

#include <QString>

namespace KIPIGPSSyncPlugin
{

class GPSBabelBinaryPriv;

class GPSBabelBinary
{

public:

    GPSBabelBinary();
    ~GPSBabelBinary();

    static const char *path();
    bool isAvailable() const;
    QString version() const;
    bool versionIsRight() const;
    QString minimalVersion() const;

    void checkSystem();

private:

    GPSBabelBinaryPriv* const d;
};

} // namespace KIPIGPSSyncPlugin

#endif  // GPSBABELBINARY_H
