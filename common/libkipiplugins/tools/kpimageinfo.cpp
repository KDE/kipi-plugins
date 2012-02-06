/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageInfo to manage easily
 *               item properties with KIPI host application.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpimageinfo.h"

// Qt includes

#include <QMap>
#include <QVariant>

// KDE includes

#include <kdebug.h>

// Libkipi includes

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>

namespace KIPIPlugins
{

class KPImageInfo::KPImageInfoPrivate
{
public:

    KPImageInfoPrivate()
    {
        iface = 0;
    }

    KUrl             url;
    KIPI::Interface* iface;
};

KPImageInfo::KPImageInfo(KIPI::Interface* iface, const KUrl& url)
    : d(new KPImageInfoPrivate)
{
    d->iface = iface;
    d->url   = url;
}

KPImageInfo::~KPImageInfo()
{
    delete d;
}

void KPImageInfo::setDescription(const QString& desc)
{
    if (d->iface)
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("comment", desc);
        info.addAttributes(map);
    }
}

QString KPImageInfo::description() const
{
    if (d->iface)
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map = info.attributes();
        if (!map.isEmpty()) return map.value("comment", QString()).toString();
    }
    return QString();
}

QStringList KPImageInfo::keywords() const
{
    if (d->iface)
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map = info.attributes();
        if (!map.isEmpty()) return map.value("tags", QStringList()).toStringList();
    }
    return QStringList();
}

void KPImageInfo::setRating(int r)
{
    if (d->iface)
    {
        if (r < 0 || r > 5)
            kDebug() << "rating is out of rage (" << r << ")";

        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("rating", r);
        info.addAttributes(map);
    }
}

int KPImageInfo::rating() const
{
    if (d->iface)
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map = info.attributes();
        if (!map.isEmpty()) return map.value("rating", -1).toInt();
    }
    return (-1);
}

}  // namespace KIPIPlugins
