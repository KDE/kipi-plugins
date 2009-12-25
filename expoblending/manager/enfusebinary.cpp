/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect enfuse binary program and version
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

class EnfuseBinaryPriv
{
public:

    EnfuseBinaryPriv()
    {
        available = false;
        version.clear();
    }

    bool    available;

    QString version;
};

EnfuseBinary::EnfuseBinary()
           : d(new EnfuseBinaryPriv)
{
    checkSystem();
}

EnfuseBinary::~EnfuseBinary()
{
    delete d;
}

void EnfuseBinary::checkSystem()
{
    QProcess process;
    process.start(path(), QStringList() << "-h");
    d->available = process.waitForFinished();

    // The output look like this : ==== enfuse, version 3.2 ====
    QString headerStarts("==== enfuse, version ");

    QString stdOut(process.readAll());
    QString firstLine = stdOut.section('\n', 0, 0);

    if (firstLine.startsWith(headerStarts))
    {
        d->version = firstLine.remove(0, headerStarts.length()).section(" ", 0, 0);
        kDebug(AREA_CODE_LOADING) << "Found " << path() << " version: " << version() ;
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

const char *EnfuseBinary::path()
{
    return "enfuse";
}

bool EnfuseBinary::isAvailable() const
{
    return d->available;
}

QString EnfuseBinary::version() const
{
    return d->version;
}

bool EnfuseBinary::versionIsRight() const
{
    if (d->version.isNull() || !isAvailable())
        return false;

    if (d->version.toFloat() >= minimalVersion().toFloat())
        return true;

    return false;
}

QString EnfuseBinary::minimalVersion() const
{
    return QString("3.2");
}

}  // namespace KIPIExpoBlendingPlugin
