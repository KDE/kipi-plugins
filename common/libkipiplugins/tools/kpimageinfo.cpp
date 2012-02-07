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

    void removeAttribute(const QString& name)
    {
        KIPI::ImageInfo info = iface->info(url);
        info.delAttributes(QStringList() << name);
    }

    bool hasAttribute(const QString& name) const
    {
        if (hasValidData())
        {
            KIPI::ImageInfo info       = iface->info(url);
            QMap<QString, QVariant>map = info.attributes();
            return (map.contains(name));
        }
        return false;
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

KUrl KPImageInfo::url() const
{
    return d->url;
}

void KPImageInfo::cloneData(const KUrl& destination)
{
    KIPI::ImageInfo srcInfo  = d->iface->info(d->url);
    KIPI::ImageInfo destInfo = d->iface->info(destination);
    destInfo.cloneData(srcInfo);
}

qlonglong KPImageInfo::fileSize() const
{
    return d->attribute("filesize").toLongLong();
}

bool KPImageInfo::hasFileSize() const
{
    return d->hasAttribute("filesize");
}

void KPImageInfo::setDescription(const QString& desc)
{
    d->setAttribute("comment", desc);
}

QString KPImageInfo::description() const
{
    return d->attribute("comment").toString();
}

bool KPImageInfo::hasDescription() const
{
    return d->hasAttribute("comment");
}

void KPImageInfo::setTagsPath(const QStringList& tp)
{
    d->setAttribute("tagspath", tp);
}

QStringList KPImageInfo::tagsPath() const
{
    return d->attribute("tagspath").toStringList();
}

bool KPImageInfo::hasTagsPath() const
{
    return d->hasAttribute("tagspath");
}

QStringList KPImageInfo::keywords() const
{
    QStringList keywords = d->attribute("keywords").toStringList();
    if (keywords.isEmpty())
        keywords = d->attribute("tags").toStringList();     // For compatibility.

    return keywords;
}

bool KPImageInfo::hasKeywords() const
{
    return (d->hasAttribute("keywords") || 
            d->hasAttribute("tags"));                      // For compatibility.
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

bool KPImageInfo::hasRating() const
{
    return d->hasAttribute("rating");
}

void KPImageInfo::setColorLabel(int cl)
{
    if (cl < 0 || cl > 10)
    {
        kDebug() << "Color label value is out of range (" << cl << ")";
        return;
    }

    d->setAttribute("colorlabel", cl);
}

int KPImageInfo::colorLabel() const
{
    return d->attribute("colorlabel").toInt();
}

bool KPImageInfo::hasColorLabel() const
{
    return d->hasAttribute("colorlabel");
}

void KPImageInfo::setPickLabel(int pl)
{
    if (pl < 0 || pl > 10)
    {
        kDebug() << "Pick label value is out of range (" << pl << ")";
        return;
    }

    d->setAttribute("picklabel", pl);
}

int KPImageInfo::pickLabel() const
{
    return d->attribute("picklabel").toInt();
}

bool KPImageInfo::hasPickLabel() const
{
    return d->hasAttribute("picklabel");
}

void KPImageInfo::setDate(const QDateTime& date)
{
    d->setAttribute("date", date);
}

QDateTime KPImageInfo::date() const
{
    return d->attribute("date").toDateTime();
}

bool KPImageInfo::hasDate() const
{
    return d->hasAttribute("date");
}

bool KPImageInfo::isExactDate() const
{
    if (d->hasAttribute("isexactdate"))
        return d->attribute("isexactdate").toBool();

    return true;
}

void KPImageInfo::setTitle(const QString& title)
{
    d->setAttribute("title", title);
}

QString KPImageInfo::title() const
{
    return d->attribute("title").toString();
}

bool KPImageInfo::hasTitle() const
{
    return d->hasAttribute("title");
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

void KPImageInfo::setLatitude(double lat)
{
    if (lat < -90.0 || lat > 90)
    {
        kDebug() << "Latitude value is out of range (" << lat << ")";
        return;
    }

    d->setAttribute("latitude", lat);
}

bool KPImageInfo::hasLatitude() const
{
    return d->hasAttribute("latitude");
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

bool KPImageInfo::hasLongitude() const
{
    return d->hasAttribute("longitude");
}

void KPImageInfo::setAltitude(double alt)
{
    d->setAttribute("altitude", alt);
}

double KPImageInfo::altitude() const
{
    return d->attribute("altitude").toDouble();
}

bool KPImageInfo::hasAltitude() const
{
    return d->hasAttribute("altitude");
}

bool KPImageInfo::hasGeolocationInfo() const
{
    return (d->hasAttribute("latitude") && d->hasAttribute("longitude") && d->hasAttribute("altitude"));
}

void KPImageInfo::removeGeolocationInfo()
{
    d->removeAttribute("gpslocation");
}

void KPImageInfo::setOrientation(KExiv2::ImageOrientation orientation)
{
    d->setAttribute("orientation", (int)orientation);
    d->setAttribute("angle",       (int)orientation);     // For compatibility.
}

KExiv2::ImageOrientation KPImageInfo::orientation() const
{
    KExiv2::ImageOrientation orientation = KExiv2::ORIENTATION_UNSPECIFIED;
    if (d->hasAttribute("orientation"))
        orientation = (KExiv2::ImageOrientation)(d->attribute("orientation").toInt());
    else
        orientation = (KExiv2::ImageOrientation)(d->attribute("angle").toInt());       // For compatibility.
    
    return orientation;
}

bool KPImageInfo::hasOrientation() const
{
    return (d->hasAttribute("orientation") || 
            d->hasAttribute("angle"));                      // For compatibility.
}

}  // namespace KIPIPlugins
