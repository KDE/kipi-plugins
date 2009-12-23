/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect align_image_stack binary program and version
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "alignbinary.h"

// Qt includes

#include <QProcess>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>

namespace KIPIExpoBlendingPlugin
{

class AlignBinaryPriv
{
public:

    AlignBinaryPriv()
    {
        available = false;
        version.clear();
    }

    bool    available;

    QString version;
};

AlignBinary::AlignBinary()
           : d(new AlignBinaryPriv)
{
    checkSystem();
}

AlignBinary::~AlignBinary()
{
    delete d;
}

void AlignBinary::checkSystem()
{
    QProcess process;
    process.start(path(), QStringList() << "-h");
    d->available = process.waitForFinished();

    // The output look like this : align_image_stack version 2009.2.0.4461
    QString headerStarts("align_image_stack version ");

    QString stdOut(process.readAll());
    QString firstLine = stdOut.section('\n', 1, 1);

    if (firstLine.startsWith(headerStarts))
    {
        d->version = firstLine.remove(0, headerStarts.length()).section(".", 0, 1);
        kDebug(AREA_CODE_LOADING) << "Found align_image_stack version: " << version() ;
    }
}

const char *AlignBinary::path()
{
    return "align_image_stack";
}

bool AlignBinary::isAvailable() const
{
    return d->available;
}

QString AlignBinary::version() const
{
    return d->version;
}

bool AlignBinary::versionIsRight() const
{
    if (d->version.isNull() || !isAvailable())
        return false;

    if (d->version.toFloat() >= minimalVersion().toFloat())
        return true;

    return false;
}

QString AlignBinary::minimalVersion() const
{
    return QString("2009.2");
}

}  // namespace KIPIExpoBlendingPlugin
