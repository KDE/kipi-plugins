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

#include <libkipi/version.h>
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

    bool hasValidData() const
    {
        return (iface && !url.isEmpty());
    }

    QVariant attribute(const QString& name) const
    {
        QMap<QString, QVariant> map; 
        if (hasValidData())
        {
            KIPI::ImageInfo info = iface->info(url);
            map                  = info.attributes();
            if (!map.isEmpty()) return map.value(name, QVariant());
        }
        return QVariant();
    }

    void setAttribute(const QString& name, const QVariant& value)
    {
        if (hasValidData())
        {
            KIPI::ImageInfo info = iface->info(url);
            QMap<QString, QVariant> map;
            map.insert(name, value);
            info.addAttributes(map);
        }
    }

public:

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
    d->setAttribute("comment", desc);
}

QString KPImageInfo::description() const
{
    return d->attribute("comment").toString();
}

void KPImageInfo::setTagsPath(const QStringList& tp)
{
    d->setAttribute("tagspath", tp);
}

QStringList KPImageInfo::tagsPath() const
{
    return d->attribute("tagspath").toStringList();
}

QStringList KPImageInfo::keywords() const
{
    return d->attribute("tags").toStringList();
}

void KPImageInfo::setRating(int r)
{
    if (r < 0 || r > 5)
    {
        kDebug() << "Rating value is out of range (" << r << ")";
        return;
    }

    d->setAttribute("rating", r);
}

int KPImageInfo::rating() const
{
    return d->attribute("rating").toInt();
}

void KPImageInfo::setDate(const QDateTime& date)
{
    d->setAttribute("date", date);
}

QDateTime KPImageInfo::date() const
{
    return d->attribute("date").toDateTime();
}

void KPImageInfo::setTitle(const QString& title)
{
    d->setAttribute("title", title);
}

QString KPImageInfo::title() const
{
    return d->attribute("title").toString();
}

void KPImageInfo::setName(const QString& name)
{
    if (d->hasValidData())
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
#if KIPI_VERSION >= 0x010300
        info.setName(name);
#else
        info.setTitle(name);
#endif
    }
}

QString KPImageInfo::name() const
{
    if (d->hasValidData())
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
#if KIPI_VERSION >= 0x010300
        return info.name();
#else
        return info.title();
#endif
    }
    return QString();
}

bool KPImageInfo::hasFullGeolocationInfo() const
{
    if (d->hasValidData())
    {
        KIPI::ImageInfo info        = d->iface->info(d->url);
        QMap<QString, QVariant> map = info.attributes();
        if (!map.isEmpty() && map.contains("latitude") && map.contains("longitude") && map.contains("altitude"))
            return true;
    }
    return false;
}

void KPImageInfo::setLatitude(double lat)
{
    if (lat < -90.0 || lat > 90)
    {
        kDebug() << "Latitude value is out of range (" << lat << ")";
        return;
    }

    d->setAttribute("latitude", lat);
}

double KPImageInfo::latitude() const
{
    return d->attribute("latitude").toDouble();
}

void KPImageInfo::setLongitude(double lng)
{
    if (lng < -180.0  || lng > 180)
    {
        kDebug() << "Latitude value is out of range (" << lng << ")";
        return;
    }

    d->setAttribute("longitude", lng);
}

double KPImageInfo::longitude() const
{
    return d->attribute("longitude").toDouble();
}

void KPImageInfo::setAltitude(double alt)
{
    d->setAttribute("altitude", alt);
}

double KPImageInfo::altitude() const
{
    return d->attribute("altitude").toDouble();
}

void KPImageInfo::setOrientation(KExiv2::ImageOrientation orientation)
{
    d->setAttribute("angle", (int)orientation);
}

KExiv2::ImageOrientation KPImageInfo::orientation() const
{
    return (KExiv2::ImageOrientation)(d->attribute("angle").toInt());
}

}  // namespace KIPIPlugins
