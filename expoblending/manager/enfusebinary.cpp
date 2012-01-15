/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

bool EnfuseBinary::parseHeader(const QString& output)
{
    // Work around Enfuse <= 3.2
    // The output look like this : ==== enfuse, version 3.2 ====
    QString headerStartsOld("==== enfuse, version ");
    QString firstLine = output.section('\n', m_headerLine, m_headerLine);
    if (firstLine.startsWith(m_headerStarts))
    {
        kDebug() << path() << " help header line: \n" << firstLine;
        setVersion(firstLine.remove(0, m_headerStarts.length()));
        kDebug() << "Found " << path() << " version: " << version();
        return true;
    }
    else if (firstLine.startsWith(headerStartsOld))
    {
        kDebug() << path() << " help header line: \n" << firstLine;
        setVersion(firstLine.remove(0, headerStartsOld.length()));
        kDebug() << "Found " << path() << " version: " << version();
        return true;
    }
    return false;
}

}  // namespace KIPIExpoBlendingPlugin
