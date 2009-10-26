/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "galleries.h"

// Qt includes

#include <QString>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <klocale.h>

namespace KIPIGalleryExportPlugin
{

Gallery::Gallery()
{
        load();
}

Gallery::~Gallery()
{
}

QString Gallery::name() const
{
    return mName;
}

QString Gallery::url() const
{
    return mUrl;
}

QString Gallery::username() const
{
    return mUsername;
}

QString Gallery::password() const
{
    return mPassword;
}

unsigned int Gallery::version() const
{
    return mVersion;
}

unsigned int Gallery::galleryId() const
{
    return mGalleryId;
}

// -------------------------------------

void Gallery::setName(const QString& name)
{
    mName = name;
}

void Gallery::setUrl(const QString& url)
{
    mUrl = url;
}

void Gallery::setUsername(const QString& username)
{
    mUsername = username;
}

void Gallery::setPassword(const QString& password)
{
    mPassword = password;
}

void Gallery::setVersion(unsigned int version)
{
    mVersion = version;
}

void Gallery::setGalleryId(unsigned int galleryId)
{
    mGalleryId = galleryId;
}

void Gallery::load()
{
    // FIXME: sure we need this?? (perhaps YES..)
    static bool bln_loaded = false;
    if (bln_loaded) return;
    bln_loaded = true;

    // read config
    KConfig config("kipirc");
    KConfigGroup group = config.group("Gallery Settings");

    kDebug() << "Reading data from kipirc file..";

    mName     = group.readEntry("Name",     QString() );
    mUrl      = group.readEntry("URL",      QString() );
    mUsername = group.readEntry("Username", QString() );
    mVersion  = group.readEntry("Version",  QString().toInt() );
    mPassword = group.readEntry("Password", QString() );
}

void Gallery::save()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Gallery Settings");

    kDebug() << "Saving data to kipirc file..";

    group.writeEntry(QString("Name"),     name() );
    group.writeEntry(QString("URL"),      url() );
    group.writeEntry(QString("Username"), username() );
    group.writeEntry(QString("Version"),  version() );
    group.writeEntry(QString("Password"), password() );

    kDebug() << "syncing..";
    config.sync();
}

} // namespace KIPIGalleryExportPlugin
