/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetects cpfind binary program and version
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

// KDE includes

#include <kdebug.h>

namespace KIPIPanoramaPlugin
{

bool CPFindBinary::parseHeader(const QString& output)
{
    QStringList lines    = output.split('\n');
    m_developmentVersion = false;

    foreach(QString line, lines)
    {
        kDebug() << path() << " help header line: \n" << line;

        if (line.contains(headerRegExp))
        {
            m_version = headerRegExp.cap(2);

            if (!headerRegExp.cap(1).isEmpty())
            {
                m_developmentVersion = true;
            }

            return true;
        }

        m_developmentVersion = true;
    }

    return false;
}

}  // namespace KIPIPanoramaPlugin

