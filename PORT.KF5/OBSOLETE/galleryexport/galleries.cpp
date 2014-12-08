/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
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

class Gallery::Private
{
public:

    Private()
    {
        mVersion   = 0;
        mGalleryId = 0;
    };

    unsigned int     mVersion;
    unsigned int     mGalleryId;
    QString          mName;
    QString          mUrl;
    QString          mUsername;
    QString          mPassword;
};

Gallery::Gallery()
    : d(new Private())
{
    load();
}

Gallery::~Gallery()
{
    delete d;
}

QString Gallery::name() const
{
    return d->mName;
}

QString Gallery::url() const
{
    return d->mUrl;
}

QString Gallery::username() const
{
    return d->mUsername;
}

QString Gallery::password() const
{
    return d->mPassword;
}

unsigned int Gallery::version() const
{
    return d->mVersion;
}

unsigned int Gallery::galleryId() const
{
    return d->mGalleryId;
}

void Gallery::setName(const QString& name)
{
    d->mName = name;
}

void Gallery::setUrl(const QString& url)
{
    d->mUrl = url;
}

void Gallery::setUsername(const QString& username)
{
    d->mUsername = username;
}

void Gallery::setPassword(const QString& password)
{
    d->mPassword = password;
}

void Gallery::setVersion(unsigned int version)
{
    d->mVersion = version;
}

void Gallery::setGalleryId(unsigned int galleryId)
{
    d->mGalleryId = galleryId;
}

void Gallery::load()
{
    // FIXME: sure we need this?? (perhaps YES..)
    static bool bln_loaded = false;

    if (bln_loaded)
        return;

    bln_loaded = true;

    // read config
    KConfig config("kipirc");
    KConfigGroup group = config.group("Gallery Settings");

    kDebug() << "Reading data from kipirc file..";

    d->mName     = group.readEntry("Name",     QString() );
    d->mUrl      = group.readEntry("URL",      QString() );
    d->mUsername = group.readEntry("Username", QString() );
    d->mVersion  = group.readEntry("Version",  QString().toInt() );
    d->mPassword = group.readEntry("Password", QString() );
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
