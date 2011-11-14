/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : Autodetects cpfind binary program and version
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

#include "cpfindbinary.h"

// Qt includes

#include <QProcess>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

namespace KIPIPanoramaPlugin
{

CPFindBinary::CPFindBinary()
    : BinaryIface()
{
    checkSystem();
}

CPFindBinary::~CPFindBinary()
{
}

void CPFindBinary::checkSystem()
{
    QProcess process;
    process.start(path(), QStringList() << "--version");
    m_available       = process.waitForFinished();

    // FIXME: Change that to a regexp
    QString headerStarts("Hugins cpfind ");
    QString headerStartsNew("Hugin's cpfind ");          // For Hugin 2011.4

    QString stdOut(process.readAllStandardOutput());
    QStringList lines = stdOut.split('\n');

    dev = false;
    foreach (QString line, lines)
    {
        kDebug() << path() << " help header line: \n" << line;
	m_version = "";
        if (line.startsWith(headerStarts))
        {
            m_version = line.remove(0, headerStarts.length()).section('.', 0, 1);
        }
	else if (line.startsWith(headerStartsNew))
	{
            m_version = line.remove(0, headerStartsNew.length()).section('.', 0, 1);
	}

        if (m_version != "")
        {
            m_version.remove("Pre-Release ");            // Special case with Hugin beta.

            kDebug() << "Found " << path() << " version: " << version() ;
            return;
        }
        dev = true;
    }
}

bool CPFindBinary::developmentVersion()
{
    return dev;
}

KUrl CPFindBinary::url() const
{
    return KUrl("http://hugin.sourceforge.net");
}

QString CPFindBinary::projectName() const
{
    return QString("Hugin");
}

QString CPFindBinary::path() const
{
    return QString("cpfind");
}

QString CPFindBinary::minimalVersion() const
{
    return QString("2010.4");
}

}  // namespace KIPIPanoramaPlugin
