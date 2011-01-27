/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect binary program and version
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
}

BinaryIface::~BinaryIface()
{
}

bool BinaryIface::showResults() const
{
    if (!isAvailable() || !versionIsRight())
    {
        KMessageBox::information(
                kapp->activeWindow(),
                i18n("<qt><p>Unable to find <i>%1</i> executable.</p>"
                    "<p>This program is required to continue.<br/>"
                    "Please install it from <b>%2</b> package provided by your distributor<br/>"
                    "or download and install <a href=\"%3\">the source</a>.</p>"
                    "<p>Note: at least, <i>%4</i> version <b>%5</b> is required.</p></qt>",
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

QString BinaryIface::findHeader(const QStringList& output, const QString& header) const
{
    foreach(const QString& s, output)
    {
        if (s.startsWith(header))
            return s;
    }
    return QString();
}

}  // namespace KIPIPlugins
