/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect binary program and version
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "binaryiface.h"

// Qt includes

#include <QProcess>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

namespace KIPIPlugins
{

BinaryIface::BinaryIface()
{
    checkSystem();
}

BinaryIface::~BinaryIface()
{
}

void BinaryIface::checkSystem()
{
}

bool BinaryIface::isAvailable() const
{
    return m_available;
}

QString BinaryIface::version() const
{
    return m_version;
}

bool BinaryIface::versionIsRight() const
{
    if (m_version.isNull() || !isAvailable())
        return false;

    if (m_version.toFloat() >= minimalVersion().toFloat())
        return true;

    return false;
}

}  // namespace KIPIPlugins
