/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect vig_optimize binary program and version
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

#include "vigoptimizebinary.h"

// Qt includes

#include <QProcess>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

namespace KIPIPanoramaPlugin
{

VigOptimiserBinary::VigOptimiserBinary()
    : BinaryIface()
{
    checkSystem();
}

VigOptimiserBinary::~VigOptimiserBinary()
{
}

void VigOptimiserBinary::checkSystem()
{
    QProcess process;
    process.start(path(), QStringList() << "-h");
    m_available       = process.waitForFinished();

    QString headerStarts("vig_optimize version ");

    QString stdOut(process.readAllStandardError());
    QString firstLine = stdOut.section('\n', 1, 1);

    kDebug() << path() << " help header line: \n" << firstLine;

    if (firstLine.startsWith(headerStarts))
    {
        m_version = firstLine.remove(0, headerStarts.length()).section(".", 0, 1);
        m_version.remove("Pre-Release ");            // Special case with Hugin beta.

        kDebug() << "Found " << path() << " version: " << version() ;
    }
}

KUrl VigOptimiserBinary::url() const
{
    return KUrl("http://hugin.sourceforge.net");
}

QString VigOptimiserBinary::projectName() const
{
    return QString("Hugin");
}

QString VigOptimiserBinary::path() const
{
    return QString("vig_optimize");
}

QString VigOptimiserBinary::minimalVersion() const
{
    return QString("2011.0");
}

}  // namespace KIPIPanoramaPlugin
