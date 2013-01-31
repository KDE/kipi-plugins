/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins release ID header.
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KP_VERSION_H
#define KP_VERSION_H

// Qt includes.

#include <QString>

// KDE includes.

#include <klocale.h>

namespace pluginsVersion
{
    const QString gitVersion();
}

static const char kipiplugins_version[] = "${KIPIPLUGINS_VERSION_STRING}";

static inline const QString kipipluginsVersion()
{
    return QString(kipiplugins_version);
}

static inline KLocalizedString additionalInformation()
{
    QString gitVer       = pluginsVersion::gitVersion();
    KLocalizedString ret = ki18n("IRC:\n"
                     "irc.freenode.net - #kde-imaging\n\n"
                     "Feedback:\n"
                     "kde-imaging@kde.org\n\n"
                     "Build date: %1 (target: %2)")
                     .subs(__DATE__)
                     .subs("@CMAKE_BUILD_TYPE@");

    if (!gitVer.isEmpty() && !gitVer.startsWith("unknow") && !gitVer.startsWith("export"))
    {
        ret = ki18n("IRC:\n"
                    "irc.freenode.net - #kde-imaging\n\n"
                    "Feedback:\n"
                    "kde-imaging@kde.org\n\n"
                    "Build date: %1 (target: %2)\n"
                    "Rev.: %3")
                    .subs(__DATE__)
                    .subs("@CMAKE_BUILD_TYPE@")
                    .subs(QString("<a href='http://commits.kde.org/kipi-plugins/%1'>%2</a>").arg(gitVer).arg(gitVer));
    }

    return ret;
}

#endif // KP_VERSION_H
