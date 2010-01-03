/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect align_image_stack binary program and version
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

AlignBinary::AlignBinary()
           : BinaryIface()
{
    checkSystem();
}

AlignBinary::~AlignBinary()
{
}

void AlignBinary::checkSystem()
{
    QProcess process;
    process.start(path(), QStringList() << "-h");
    m_available = process.waitForFinished();

    // The output look like this : align_image_stack version 2009.2.0.4461
    QString headerStarts("align_image_stack version ");

    QString stdOut(process.readAllStandardError());
    QString firstLine = stdOut.section('\n', 1, 2);

    kDebug() << path() << " help header line: \n" << firstLine;

    if (firstLine.startsWith(headerStarts))
    {
        m_version = firstLine.remove(0, headerStarts.length()).section(".", 0, 1);
        m_version.remove("Pre-Release ");            // Special case with Hugin beta.

        kDebug() << "Found " << path() << " version: " << version() ;
    }
}

KUrl AlignBinary::url() const
{
    return KUrl("http://hugin.sourceforge.net");
}

QString AlignBinary::projectName() const
{
    return QString("Hugin");
}

QString AlignBinary::path() const
{
    return QString("align_image_stack");
}

QString AlignBinary::minimalVersion() const
{
    return QString("0.8");
}

}  // namespace KIPIExpoBlendingPlugin
