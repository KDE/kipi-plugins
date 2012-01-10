/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetects cpclean binary program and version
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "cpcleanbinary.h"

// Qt includes

#include <QProcess>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

namespace KIPIPanoramaPlugin
{

CPCleanBinary::CPCleanBinary()
    : BinaryIface()
{
    m_configGroup       = "Panorama Settings";
    m_pathToBinary      = "cpclean";
    setBaseName("cpclean");
    m_versionArguments << "-h";
    readConfig();
}

CPCleanBinary::~CPCleanBinary()
{
}

bool CPCleanBinary::parseHeader(const QString& output)
{
    QString headerStarts("cpclean version ");
    QString firstLine = output.section('\n', 1, 1);
    kDebug() << path() << " help header line: \n" << firstLine;
    if (firstLine.startsWith(headerStarts))
    {
        m_version = firstLine.remove(0, headerStarts.length()).section('.', 0, 1);
        m_version.remove("Pre-Release ");            // Special case with Hugin beta.
        kDebug() << "Found " << path() << " version: " << version() ;
        return true;
    }
    return false;
}

KUrl CPCleanBinary::url() const
{
    return KUrl("http://hugin.sourceforge.net");
}

QString CPCleanBinary::projectName() const
{
    return QString("Hugin");
}

QString CPCleanBinary::minimalVersion() const
{
    return QString("2010.4");
}

}  // namespace KIPIPanoramaPlugin
