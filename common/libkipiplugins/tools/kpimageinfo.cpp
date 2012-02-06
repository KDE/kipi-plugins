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

    QMap<QString, QVariant> attributes()
    {
        QMap<QString, QVariant> map; 
        if (hasValidData())
        {
            KIPI::ImageInfo info = iface->info(url);
            map                  = info.attributes();
        }
        return map;
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
    if (d->hasValidData())
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("comment", desc);
        info.addAttributes(map);
    }
}

QString KPImageInfo::description() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("comment", QString()).toString();
    }
    return QString();
}

void KPImageInfo::setTagsPath(const QStringList& tp)
{
    if (d->hasValidData())
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("tagspath", tp);
        info.addAttributes(map);
    }
}

QStringList KPImageInfo::tagsPath() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("tagspath", QStringList()).toStringList();
    }
    return QStringList();
}

QStringList KPImageInfo::keywords() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("tags", QStringList()).toStringList();
    }
    return QStringList();
}

void KPImageInfo::setRating(int r)
{
    if (d->hasValidData())
    {
        if (r < 0 || r > 5)
        {
            kDebug() << "Rating value is out of range (" << r << ")";
            return;
        }

        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("rating", r);
        info.addAttributes(map);
    }
}

int KPImageInfo::rating() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("rating", -1).toInt();
    }
    return (-1);
}

void KPImageInfo::setDate(const QDateTime& date)
{
    if (d->hasValidData())
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("date", date);
        info.addAttributes(map);
    }
}

QDateTime KPImageInfo::date() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("date", QDateTime()).toDateTime();
    }
    return QDateTime();
}

void KPImageInfo::setTitle(const QString& title)
{
    if (d->hasValidData())
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("title", title);
        info.addAttributes(map);
    }
}

QString KPImageInfo::title() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("title", QString()).toString();
    }
    return QString();
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
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty() && map.contains("latitude") && map.contains("longitude") && map.contains("altitude"))
            return true;
    }
    return false;
}

void KPImageInfo::setLatitude(double lat)
{
    if (d->hasValidData())
    {
        if (lat < -90.0 || lat > 90)
        {
            kDebug() << "Latitude value is out of range (" << lat << ")";
            return;
        }

        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("latitude", lat);
        info.addAttributes(map);
    }
}

double KPImageInfo::latitude() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("latitude", 0.0).toDouble();
    }
    return 0.0;
}

void KPImageInfo::setLongitude(double lng)
{
    if (d->hasValidData())
    {
        if (lng < -180.0  || lng > 180)
        {
            kDebug() << "Latitude value is out of range (" << lng << ")";
            return;
        }

        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("longitude", lng);
        info.addAttributes(map);
    }
}

double KPImageInfo::longitude() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("longitude", 0.0).toDouble();
    }
    return 0.0;
}

void KPImageInfo::setAltitude(double alt)
{
    if (d->hasValidData())
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("altitude", alt);
        info.addAttributes(map);
    }
}

double KPImageInfo::altitude() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return map.value("altitude", 0.0).toDouble();
    }
    return 0.0;
}

void KPImageInfo::setOrientation(KExiv2::ImageOrientation orientation)
{
    if (d->hasValidData())
    {
        KIPI::ImageInfo info = d->iface->info(d->url);
        QMap<QString, QVariant> map;
        map.insert("angle", (int)orientation);
        info.addAttributes(map);
    }
}

KExiv2::ImageOrientation KPImageInfo::orientation() const
{
    if (d->hasValidData())
    {
        QMap<QString, QVariant> map = d->attributes();
        if (!map.isEmpty()) return (KExiv2::ImageOrientation)(map.value("angle", KExiv2::ORIENTATION_UNSPECIFIED).toInt());
    }
    return KExiv2::ORIENTATION_UNSPECIFIED;
}

}  // namespace KIPIPlugins
