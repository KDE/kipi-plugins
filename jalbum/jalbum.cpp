/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : a plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013-2017 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#include <QApplication>
#include <QString>
#include <QDir>
#include <QUrl>

// KDE includes

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "kipiplugins_debug.h"

#define JALBUM_JAR_PATH "/usr/share/jalbum/JAlbum.jar"

namespace KIPIJAlbumExportPlugin
{

class JAlbum::Private
{
public:

    Private()
    {
    }

    QUrl mAlbumPath;
    QUrl mJarPath;
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

QUrl JAlbum::albumPath() const
{
    return d->mAlbumPath;
}

QUrl JAlbum::jarPath() const
{
    return d->mJarPath;
}

void JAlbum::setPath(const QString& path)
{
    d->mAlbumPath = QUrl::fromUserInput(path, QString(), QUrl::AssumeLocalFile);
}

void JAlbum::setJar(const QString& jar)
{
    d->mJarPath = QUrl::fromUserInput(jar, QString(), QUrl::AssumeLocalFile);
}

void JAlbum::load()
{
    // FIXME: sure we need this?? (perhaps YES..)
    static bool bln_loaded = false;
    QString dfltJarPath(QLatin1String(JALBUM_JAR_PATH));
    QString dfltAlbumPath, tmpString;

    if (bln_loaded)
        return;

    bln_loaded = true;

    // read config
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group("jAlbum Settings");

    qCDebug(KIPIPLUGINS_LOG) << "Reading jAlbum data from kipirc file..";

#ifdef Q_OS_WIN
    dfltAlbumPath = QLatin1String(qgetenv("HOMEDRIVE").constData());
    dfltAlbumPath.append(QLatin1String(qgetenv("HOMEPATH").constData()));
    dfltAlbumPath.append(QLatin1String("\\Documents\\My Albums"));
#else
    dfltAlbumPath = QLatin1String(qgetenv("HOME").constData());
    dfltAlbumPath.append(QLatin1String("/Documents/My Albums"));
#endif

#ifdef Q_OS_WIN
    dfltJarPath = QLatin1String(qgetenv("ProgramFiles").constData());
    dfltJarPath.append(QLatin1String("\\jAlbum\\JAlbum.jar"));
#endif

    tmpString = group.readEntry("AlbumPath", dfltAlbumPath);
    d->mAlbumPath = QUrl::fromUserInput(QDir::toNativeSeparators(tmpString), QString(), QUrl::AssumeLocalFile);
    tmpString = group.readEntry("JarPath",   dfltJarPath);
    d->mJarPath   = QUrl::fromUserInput(QDir::toNativeSeparators(tmpString), QString(), QUrl::AssumeLocalFile);
}

void JAlbum::save()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group("jAlbum Settings");

    qCDebug(KIPIPLUGINS_LOG) << "Saving jAlbum data to kipirc file..";

    group.writeEntry("AlbumPath", albumPath().path());
    group.writeEntry("JarPath",   jarPath().path());

    qCDebug(KIPIPLUGINS_LOG) << "syncing..";
    config.sync();
}

} // namespace KIPIJAlbumExportPlugin
