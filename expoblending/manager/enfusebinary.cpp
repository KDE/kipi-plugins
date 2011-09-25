/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect enfuse binary program and version
 *
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "enfusebinary.h"

// Qt includes

#include <QProcess>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

namespace KIPIExpoBlendingPlugin
{

EnfuseBinary::EnfuseBinary()
            : BinaryIface()
{
    checkSystem();
}

EnfuseBinary::~EnfuseBinary()
{
}

void EnfuseBinary::checkSystem()
{
    QProcess process;
    process.start(path(), QStringList() << "-V");
    m_available = process.waitForFinished();

    QString stdOut = process.readAll();

    // Work around Enfuse <= 3.2
    // The output look like this : ==== enfuse, version 3.2 ====

    QString headerStarts("==== enfuse, version ");
    QString firstLine = findHeader(stdOut.split('\n', QString::SkipEmptyParts), headerStarts);
    if (firstLine.isEmpty())
    {
        // Work around Enfuse >= 4.0
        // The output look like this : enfuse 4.0-753b534c819d

        headerStarts = QString("enfuse ");
        firstLine = findHeader(stdOut.split('\n', QString::SkipEmptyParts), headerStarts);
        kDebug() << path() << " help header line: \n" << firstLine;
        m_version = firstLine.remove(0, headerStarts.length()).section('-', 0, 0);
        kDebug() << "Found " << path() << " version: " << version();
    }
    else
    {
        kDebug() << path() << " help header line: \n" << firstLine;
        m_version = firstLine.remove(0, headerStarts.length()).section(' ', 0, 0);
        kDebug() << "Found " << path() << " version: " << version();
    }
}

KUrl EnfuseBinary::url() const
{
    return KUrl("http://enblend.sourceforge.net");
}

QString EnfuseBinary::projectName() const
{
    return QString("Enblend");
}

QString EnfuseBinary::path() const
{
    return QString("enfuse");
}

QString EnfuseBinary::minimalVersion() const
{
    return QString("3.2");
}

}  // namespace KIPIExpoBlendingPlugin
