/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : a plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#include "jalbum.h"

// Qt includes

#include <QDir>
#include <QString>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <klocale.h>

#define JALBUM_JAR_PATH "/usr/share/jalbum/JAlbum.jar"

namespace KIPIJAlbumExportPlugin
{

class JAlbum::Private
{
public:

    Private()
    {
    }

    KUrl             mAlbumPath;
    KUrl             mJarPath;
};

JAlbum::JAlbum()
    : d(new Private())
{
    load();
}

JAlbum::~JAlbum()
{
    delete d;
}

KUrl JAlbum::albumPath() const
{
    return d->mAlbumPath;
}

KUrl JAlbum::jarPath() const
{
    return d->mJarPath;
}

void JAlbum::setPath(const QString& path)
{
    d->mAlbumPath.setUrl(path);
#ifndef WIN32
    d->mAlbumPath.cleanPath();
#endif
}

void JAlbum::setJar(const QString& jar)
{
    d->mJarPath.setUrl(jar);
#ifndef WIN32
    d->mJarPath.cleanPath();
#endif
}

void JAlbum::load()
{
    // FIXME: sure we need this?? (perhaps YES..)
    static bool bln_loaded = false;
    QString dfltJarPath    = JALBUM_JAR_PATH;
    QString dfltAlbumPath, tmpString;

    if (bln_loaded)
        return;

    bln_loaded = true;

    // read config
    KConfig config("kipirc");
    KConfigGroup group = config.group("jAlbum Settings");

    kDebug() << "Reading data from kipirc file..";

#ifdef WIN32
    dfltAlbumPath = QString(qgetenv("HOMEDRIVE").constData());
    dfltAlbumPath.append(QString(qgetenv("HOMEPATH").constData()));
    dfltAlbumPath.append("\\Documents\\My Albums");
#else
    dfltAlbumPath = QString(qgetenv("HOME").constData());
    dfltAlbumPath.append("/Documents/My Albums");
#endif

#ifdef WIN32
    dfltJarPath = QString(qgetenv("ProgramFiles").constData());
    dfltJarPath.append("\\jAlbum\\JAlbum.jar");
#endif

    tmpString = group.readEntry("AlbumPath",   dfltAlbumPath);
    d->mAlbumPath.setUrl("file:///" + QDir::toNativeSeparators(tmpString));
#ifndef WIN32
    d->mAlbumPath.cleanPath();
#endif
    tmpString = group.readEntry("JarPath",     dfltJarPath);
    d->mJarPath.setUrl("file:///" + QDir::toNativeSeparators(tmpString));
#ifndef WIN32
    d->mJarPath.cleanPath();
#endif
}

void JAlbum::save()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("jAlbum Settings");

    kDebug() << "Saving data to kipirc file..";

    group.writeEntry(QString("AlbumPath"),   albumPath().path() );
    group.writeEntry(QString("JarPath"),     jarPath().path() );

    kDebug() << "syncing..";
    config.sync();
}

bool JAlbum::createDir(const QString& dirName)
{
    QStringList parts = dirName.split(QDir::separator(), QString::SkipEmptyParts);
    QDir dir          = QDir::root();

    Q_FOREACH(const QString& part, parts)
    {
        if (!dir.exists(part))
        {
            if (!dir.mkdir(part))
            {
                kDebug() << "Could not create directory";
                return false;
            }
        }

        dir.cd(part);
    }

    return true;
}

} // namespace KIPIJAlbumExportPlugin
