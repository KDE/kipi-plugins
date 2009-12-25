/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect binary program and version
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

#include "binaryiface.h"

// KDE includes

#include <kapplication.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

namespace KIPIPlugins
{

BinaryIface::BinaryIface()
{
    checkSystem();
}

BinaryIface::~BinaryIface()
{
}

void BinaryIface::checkSystem()
{
}

bool BinaryIface::showResults() const
{
    if (!isAvailable() || !versionIsRight())
    {
        KMessageBox::information(
                kapp->activeWindow(),
                i18n("<p>Unable to find %1 executable:<br/> "
                    "This program is required by this plugin to align bracketed images. "
                    "Please install this program from %2 package from your distributor "
                    "or <a href=\"%3\">download the source</a>.</p>"
                    "<p>Note: at least, %4 version %5 is required.</p>",
                    path(),
                    projectName(),
                    url().url(),
                    path(),
                    minimalVersion()),
                QString(),
                QString(),
                KMessageBox::Notify | KMessageBox::AllowLink);

        return false;
    }

    return true;
}

bool BinaryIface::isAvailable() const
{
    return m_available;
}

QString BinaryIface::version() const
{
    return m_version;
}

bool BinaryIface::versionIsRight() const
{
    if (version().isNull() || !isAvailable())
        return false;

    if (version().toFloat() >= minimalVersion().toFloat())
        return true;

    return false;
}

}  // namespace KIPIPlugins
