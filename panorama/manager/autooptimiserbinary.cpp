/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect autooptimiser binary program and version
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

#include "autooptimiserbinary.h"

// Qt includes

#include <QProcess>
#include <QMessageBox>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>

namespace KIPIPanoramaPlugin
{

AutoOptimiserBinary::AutoOptimiserBinary()
    : BinaryIface()
{
    m_configGroup       = "Panorama Settings";
    m_pathToBinary      = "autooptimiser";
    setBaseName("autooptimiser");
    m_versionArguments.clear();
    readConfig();
}

AutoOptimiserBinary::~AutoOptimiserBinary()
{
}

bool AutoOptimiserBinary::parseHeader(const QString & output)
{
    QString headerStarts("autooptimiser version ");
    QString firstLine = output.section('\n', 1, 1);
    kDebug() << path() << " help header line: \n" << firstLine;
    if (firstLine.startsWith(headerStarts))
    {
        m_version = firstLine.remove(0, headerStarts.length()).section(".", 0, 1);
        m_version.remove("Pre-Release ");            // Special case with Hugin beta.
        kDebug() << "Found " << path() << " version: " << version() ;
        return true;
    }
    return false;
}

KUrl AutoOptimiserBinary::url() const
{
    return KUrl("http://hugin.sourceforge.net");
}

QString AutoOptimiserBinary::projectName() const
{
    return QString("Hugin");
}

QString AutoOptimiserBinary::minimalVersion() const
{
    return QString("2010.4");
}

}  // namespace KIPIPanoramaPlugin
