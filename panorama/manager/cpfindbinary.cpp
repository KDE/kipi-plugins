/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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
#include <QRegExp>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>

namespace KIPIPanoramaPlugin
{

bool CPFindBinary::parseHeader(const QString& output)
{
    bool ret = false;

    // FIXME: Change that to a regexp
    QString headerStartsNew("Hugin's cpfind ");          // For Hugin 2011.4
    QStringList lines    = output.split('\n');
    m_developmentVersion = false;
    
    foreach(QString line, lines)
    {
        kDebug() << path() << " help header line: \n" << line;

        if (line.startsWith(m_headerStarts))
        {
            m_version = line.remove(0, m_headerStarts.length()).section('.', 0, 1);
        }
        else if (line.startsWith(headerStartsNew))
        {
            m_version = line.remove(0, headerStartsNew.length()).section('.', 0, 1);
        }
        else
        {
            kDebug() << "no match";
        }

        if (!m_version.isEmpty())
        {
            if (m_version.startsWith("Pre-Release "))
            {
                m_developmentVersion = true;
                m_version.remove("Pre-Release ");            // Special case with Hugin beta.
            }
            ret = true;
            break;
        }
        else
        {
            ret = false;
        }
        m_developmentVersion = true;
    }

    return ret;
}

}  // namespace KIPIPanoramaPlugin

