/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2014-09-30
 * Description : a plugin to export to a remote Piwigo server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010-2014 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#include "piwigos.h"

// Qt includes

#include <QString>
#include <QApplication>

// KDE includes

#include <kconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIPiwigoExportPlugin
{

Piwigo::Piwigo()
{
    load();
}

Piwigo::~Piwigo()
{
}

QString Piwigo::url() const
{
    return mUrl;
}

QString Piwigo::username() const
{
    return mUsername;
}

QString Piwigo::password() const
{
    return mPassword;
}

void Piwigo::setUrl(const QString& url)
{
    mUrl = url;
}

void Piwigo::setUsername(const QString& username)
{
    mUsername = username;
}

void Piwigo::setPassword(const QString& password)
{
    mPassword = password;
}

void Piwigo::load()
{
    // FIXME: sure we need this?? (perhaps YES..)
    static bool bln_loaded = false;

    if (bln_loaded)
        return;

    bln_loaded = true;

    // read config
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("Piwigo Settings");

    qCDebug(KIPIPLUGINS_LOG) << "Reading data from kipirc file..";

    mUrl      = group.readEntry("URL",      QString());
    mUsername = group.readEntry("Username", QString());
    mPassword = group.readEntry("Password", QString());
}

void Piwigo::save()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("Piwigo Settings");

    qCDebug(KIPIPLUGINS_LOG) << "Saving data to kipirc file..";

    group.writeEntry(QString::fromUtf8("URL"),      url());
    group.writeEntry(QString::fromUtf8("Username"), username());
    group.writeEntry(QString::fromUtf8("Password"), password());

    qCDebug(KIPIPLUGINS_LOG) << "syncing..";
    config.sync();
}

} // namespace KIPIPiwigoExportPlugin
