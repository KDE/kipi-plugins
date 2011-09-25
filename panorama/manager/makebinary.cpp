/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect make binary program and version
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

#include "makebinary.h"

// Qt includes

#include <QProcess>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

namespace KIPIPanoramaPlugin
{

MakeBinary::MakeBinary() : BinaryIface()
{
    checkSystem();
}

MakeBinary::~MakeBinary()
{
}

void MakeBinary::checkSystem()
{
    QProcess process;
    process.start(path(), QStringList() << "-v");
    m_available       = process.waitForFinished();

    QString headerStarts("GNU Make ");

    QString stdOut(process.readAllStandardOutput());
    QString firstLine = stdOut.section('\n', 0, 0);

    kDebug() << path() << " help header line: \n" << firstLine;

    if (firstLine.startsWith(headerStarts))
    {
        m_version = firstLine.remove(0, headerStarts.length()).section('.', 0, 1);

        kDebug() << "Found " << path() << " version: " << version() ;
    }
}

KUrl MakeBinary::url() const
{
    return KUrl("http://www.gnu.org/software/make/make.html");
}

QString MakeBinary::projectName() const
{
    return QString("GNU");
}

QString MakeBinary::path() const
{
    return QString("make");
}

QString MakeBinary::minimalVersion() const
{
    return QString("3.80");
}

}  // namespace KIPIPanoramaPlugin
