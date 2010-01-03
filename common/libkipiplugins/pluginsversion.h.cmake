/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins release ID header.
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KIPIPLUGINS_VERSION_H
#define KIPIPLUGINS_VERSION_H

// Qt includes.

#include <QString>

// KDE includes.

#include <klocale.h>

// Local includes.

#include <svnversion.h>

static const char kipiplugins_version[] = "${KIPIPLUGINS_VERSION_STRING}";

static inline const QString kipipluginsVersion()
{
    // We only take the mixed revision
    QString svnVer         = QString(SVNVERSION).section(":", 0, 0);

    QString kipipluginsVer = QString(kipiplugins_version);
    if (!svnVer.isEmpty() && !svnVer.startsWith("unknow") && !svnVer.startsWith("export"))
	kipipluginsVer = i18nc("%1 is kipi-plugins version, %2 is the svn revision", 
	                       "%1 (rev.: %2)", kipiplugins_version, svnVer);

    return kipipluginsVer;
}

#endif // KIPIPLUGINS_VERSION_H
