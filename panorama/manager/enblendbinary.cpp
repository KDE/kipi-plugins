/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect enblend binary program and version
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

#include "enblendbinary.h"

// Qt includes

#include <QProcess>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

namespace KIPIPanoramaPlugin
{

EnblendBinary::EnblendBinary()
            : BinaryIface()
{
    m_configGroup       = "Panorama Settings";
    m_pathToBinary      = "enblend";
    setBaseName("enblend");
    m_versionArguments << "-V";
    readConfig();
}

EnblendBinary::~EnblendBinary()
{
}

bool EnblendBinary::parseHeader(const QString & output)
{
    QString headerStarts("enblend ");
    QString firstLine = output.section('\n', 0, 0);
    kDebug() << path() << " help header line: \n" << firstLine;
    if (firstLine.startsWith(headerStarts))
    {
        m_version = firstLine.remove(0, headerStarts.length()).section('-', 0, 0);
        kDebug() << "Found " << path() << " version: " << version() ;
        return true;
    }
    return false;
}

KUrl EnblendBinary::url() const
{
    return KUrl("http://enblend.sourceforge.net");
}

QString EnblendBinary::projectName() const
{
    return QString("Enblend");
}

QString EnblendBinary::minimalVersion() const
{
    return QString("4.0");
}

}  // namespace KIPIPanoramaPlugin
