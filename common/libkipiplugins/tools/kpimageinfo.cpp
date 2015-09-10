/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageInfo to manage easily
 *               item properties with KIPI host application.
 *
 * Copyright (C) 2012-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QDebug>

// Libkipi includes

#include <libkipi_version.h>
#include <KIPI/Interface>
#include <KIPI/ImageInfo>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

using namespace KIPI;

namespace KIPIPlugins
{

class KPImageInfo::Private
{
public:

    Private()
        : iface(0)
    {
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
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
            ImageInfo info = iface->info(url);
            map            = info.attributes();

            if (!map.isEmpty())
                return map.value(name, QVariant());
        }

        return QVariant();
    }

    void setAttribute(const QString& name, const QVariant& value)
    {
        if (hasValidData())
        {
            ImageInfo info = iface->info(url);
            QMap<QString, QVariant> map;
            map.insert(name, value);
            info.addAttributes(map);
        }
    }

    void removeAttribute(const QString& name)
    {
        ImageInfo info = iface->info(url);
        info.delAttributes(QStringList() << name);
    }

    bool hasAttribute(const QString& name) const
    {
        return (attribute(name).isValid());
    }

public:

    QUrl       url;
    Interface* iface;
};

KPImageInfo::KPImageInfo(const QUrl& url)
    : d(new Private)
{
    d->url = url;
}

KPImageInfo::~KPImageInfo()
{
    delete d;
}

QUrl KPImageInfo::url() const
{
    return d->url;
}

void KPImageInfo::cloneData(const QUrl& destination)
{
    if (d->hasValidData())
    {
        ImageInfo srcInfo  = d->iface->info(d->url);
        ImageInfo destInfo = d->iface->info(destination);
        destInfo.cloneData(srcInfo);
    }
}

qlonglong KPImageInfo::fileSize() const
{
    if (hasFileSize())
        return d->attribute(QLatin1String("filesize")).toLongLong();

#if KIPI_VERSION < 0x010500
    if (d->hasValidData())
    {
        ImageInfo info = d->iface->info(d->url);
        return (qlonglong)info.size();
    }
#endif

    return (-1);
}

bool KPImageInfo::hasFileSize() const
{
    return d->hasAttribute(QLatin1String("filesize"));
}

void KPImageInfo::setDescription(const QString& desc)
{
    if (d->iface)
    {
        d->setAttribute(QLatin1String("comment"), desc);

#if KIPI_VERSION < 0x010500
        if (d->hasValidData())
        {
            ImageInfo info = d->iface->info(d->url);
            info.setDescription(desc);
        }
#endif
    }
    else
    {
        KPMetadata meta(d->url.toLocalFile());

        // We set image comments, outside Exif, XMP, and IPTC.
        meta.setComments(desc.toUtf8());

        // We set Exif comments
        meta.setExifComment(desc);

        // We set IPTC comments
        QString trunc = desc;
        trunc.truncate(2000);
        meta.removeIptcTag("Iptc.Application2.Caption");
        meta.setIptcTagString("Iptc.Application2.Caption", trunc);
        meta.applyChanges();
    }
}

QString KPImageInfo::description() const
{
    if (d->iface)
    {
        if (hasDescription()) return d->attribute(QLatin1String("comment")).toString();

#if KIPI_VERSION < 0x010500
        if (d->hasValidData())
        {
            ImageInfo info = d->iface->info(d->url);
            return info.description();
        }
#endif
    }
    else
    {
        KPMetadata meta(d->url.toLocalFile());

        // We trying image comments, outside Exif, XMP, and IPTC.
        QString comment = meta.getCommentsDecoded();
        if (!comment.isEmpty()) return comment;

        // We trying to get Exif comments

        comment = meta.getExifComment();
        if (!comment.isEmpty()) return comment;

        // We trying to get IPTC comments

        comment = meta.getIptcTagString("Iptc.Application2.Caption", false);
        if (!comment.isEmpty()) return comment;
    }

    return QString();
}

bool KPImageInfo::hasDescription() const
{
    if (d->iface)
        return d->hasAttribute(QLatin1String("comment"));

    return (!description().isNull());
}

void KPImageInfo::setDate(const QDateTime& date)
{
    d->setAttribute(QLatin1String("date"), date);

#if KIPI_VERSION < 0x010500
    if (d->hasValidData())
    {
        ImageInfo info = d->iface->info(d->url);
        info.setTime(date);
    }
#endif
}

QDateTime KPImageInfo::date() const
{
    if (hasDate()) return d->attribute(QLatin1String("date")).toDateTime();

#if KIPI_VERSION < 0x010500
    if (d->hasValidData())
    {
        ImageInfo info = d->iface->info(d->url);
        return info.time();
    }
#endif

    return QDateTime();
}

bool KPImageInfo::hasDate() const
{
    return d->hasAttribute(QLatin1String("date"));
}

bool KPImageInfo::isExactDate() const
{
    if (d->hasAttribute(QLatin1String("isexactdate")))
        return d->attribute(QLatin1String("isexactdate")).toBool();

#if KIPI_VERSION < 0x010500
    if (d->hasValidData())
    {
        ImageInfo info = d->iface->info(d->url);
        return info.isTimeExact();
    }
#endif

    return true;
}

void KPImageInfo::setName(const QString& name)
{
    d->setAttribute(QLatin1String("name"), name);

    if (d->hasValidData())
    {
#if (KIPI_VERSION >= 0x010300) && (KIPI_VERSION < 0x010500)
        ImageInfo info = d->iface->info(d->url);
        info.setName(name);
#elif (KIPI_VERSION < 0x010300)
        ImageInfo info = d->iface->info(d->url);
        info.setTitle(name);
#endif
    }
}

QString KPImageInfo::name() const
{
    if (hasName()) return d->attribute(QLatin1String("name")).toString();

    if (d->hasValidData())
    {
#if (KIPI_VERSION >= 0x010300) && (KIPI_VERSION < 0x010500)
        ImageInfo info = d->iface->info(d->url);
        return info.name();
#elif (KIPI_VERSION < 0x010300)
        ImageInfo info = d->iface->info(d->url);
        return info.title();
#endif
    }
    return QString();
}

bool KPImageInfo::hasName() const
{
    return d->hasAttribute(QLatin1String("name"));
}

void KPImageInfo::setOrientation(KPMetadata::ImageOrientation orientation)
{
    d->setAttribute(QLatin1String("orientation"), (int)orientation);
    d->setAttribute(QLatin1String("angle"),       (int)orientation);     // NOTE: For compatibility.

#if KIPI_VERSION < 0x010500
    if (d->hasValidData())
    {
        ImageInfo info = d->iface->info(d->url);
        info.setAngle((int)orientation);
    }
#endif
}

KPMetadata::ImageOrientation KPImageInfo::orientation() const
{
    KPMetadata::ImageOrientation orientation = KPMetadata::ORIENTATION_UNSPECIFIED;

    if (d->hasAttribute(QLatin1String("orientation")))
        orientation = (KPMetadata::ImageOrientation)(d->attribute(QLatin1String("orientation")).toInt());
    else if (d->hasAttribute(QLatin1String("angle")))
        orientation = (KPMetadata::ImageOrientation)(d->attribute(QLatin1String("angle")).toInt());    // NOTE: For compatibility.

#if KIPI_VERSION < 0x010500
    if (d->hasValidData())
    {
        ImageInfo info = d->iface->info(d->url);
        orientation    = (KPMetadata::ImageOrientation)info.angle();
    }
#endif

    return orientation;
}

bool KPImageInfo::hasOrientation() const
{
    return (d->hasAttribute(QLatin1String("orientation")) || 
            d->hasAttribute(QLatin1String("angle")));          // NOTE: For compatibility.
}

void KPImageInfo::setTitle(const QString& title)
{
    d->setAttribute(QLatin1String("title"), title);
}

QString KPImageInfo::title() const
{
    return d->attribute(QLatin1String("title")).toString();
}

bool KPImageInfo::hasTitle() const
{
    return d->hasAttribute(QLatin1String("title"));
}

void KPImageInfo::setLatitude(double lat)
{
    if (lat < -90.0 || lat > 90)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Latitude value is out of range (" << lat << ")";
        return;
    }

    d->setAttribute(QLatin1String("latitude"), lat);
}

bool KPImageInfo::hasLatitude() const
{
    return d->hasAttribute(QLatin1String("latitude"));
}

double KPImageInfo::latitude() const
{
    return d->attribute(QLatin1String("latitude")).toDouble();
}

void KPImageInfo::setLongitude(double lng)
{
    if (lng < -180.0  || lng > 180)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Latitude value is out of range (" << lng << ")";
        return;
    }

    d->setAttribute(QLatin1String("longitude"), lng);
}

double KPImageInfo::longitude() const
{
    return d->attribute(QLatin1String("longitude")).toDouble();
}

bool KPImageInfo::hasLongitude() const
{
    return d->hasAttribute(QLatin1String("longitude"));
}

void KPImageInfo::setAltitude(double alt)
{
    d->setAttribute(QLatin1String("altitude"), alt);
}

double KPImageInfo::altitude() const
{
    return d->attribute(QLatin1String("altitude")).toDouble();
}

bool KPImageInfo::hasAltitude() const
{
    return d->hasAttribute(QLatin1String("altitude"));
}

bool KPImageInfo::hasGeolocationInfo() const
{
    return (d->hasAttribute(QLatin1String("latitude"))  &&
            d->hasAttribute(QLatin1String("longitude")) &&
            d->hasAttribute(QLatin1String("altitude")));
}

void KPImageInfo::removeGeolocationInfo()
{
    d->removeAttribute(QLatin1String("gpslocation"));
}

void KPImageInfo::setRating(int r)
{
    if (r < 0 || r > 5)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Rating value is out of range (" << r << ")";
        return;
    }

    d->setAttribute(QLatin1String("rating"), r);
}

int KPImageInfo::rating() const
{
    return d->attribute(QLatin1String("rating")).toInt();
}

bool KPImageInfo::hasRating() const
{
    return d->hasAttribute(QLatin1String("rating"));
}

void KPImageInfo::setColorLabel(int cl)
{
    if (cl < 0 || cl > 10)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Color label value is out of range (" << cl << ")";
        return;
    }

    d->setAttribute(QLatin1String("colorlabel"), cl);
}

int KPImageInfo::colorLabel() const
{
    return d->attribute(QLatin1String("colorlabel")).toInt();
}

bool KPImageInfo::hasColorLabel() const
{
    return d->hasAttribute(QLatin1String("colorlabel"));
}

void KPImageInfo::setPickLabel(int pl)
{
    if (pl < 0 || pl > 10)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Pick label value is out of range (" << pl << ")";
        return;
    }

    d->setAttribute(QLatin1String("picklabel"), pl);
}

int KPImageInfo::pickLabel() const
{
    return d->attribute(QLatin1String("picklabel")).toInt();
}

bool KPImageInfo::hasPickLabel() const
{
    return d->hasAttribute(QLatin1String("picklabel"));
}

void KPImageInfo::setTagsPath(const QStringList& tp)
{
    d->setAttribute(QLatin1String("tagspath"), tp);
}

QStringList KPImageInfo::tagsPath() const
{
    return d->attribute(QLatin1String("tagspath")).toStringList();
}

bool KPImageInfo::hasTagsPath() const
{
    return d->hasAttribute(QLatin1String("tagspath"));
}

QStringList KPImageInfo::keywords() const
{
    QStringList keywords;

    if(d->iface)
    {
        keywords = d->attribute(QLatin1String("keywords")).toStringList();

        if (keywords.isEmpty())
            keywords = d->attribute(QLatin1String("tags")).toStringList();     // NOTE: For compatibility.
    }
    else
    {
        KPMetadata meta(d->url.toLocalFile());
        // Trying to find IPTC keywords
        keywords = meta.getIptcKeywords();

        if(!keywords.isEmpty())
            return keywords;

        // Trying to find Xmp keywords
        keywords = meta.getXmpKeywords();

        if(!keywords.isEmpty())
            return keywords;
    }

    return keywords;
}

bool KPImageInfo::hasKeywords() const
{
    if(d->iface)
    {
        return (d->hasAttribute(QLatin1String("keywords")) ||
                d->hasAttribute(QLatin1String("tags")));       // NOTE: For compatibility.
    }
    else
    {
        KPMetadata meta(d->url.toLocalFile());
        // Trying to find IPTC keywords
        QStringList keywords = meta.getIptcKeywords();

        if(!keywords.isEmpty())
            return true;

        // Trying to find Xmp keywords
        keywords = meta.getXmpKeywords();

        if(!keywords.isEmpty())
            return true;
    }

    return false;
}

void KPImageInfo::setCreators(const QStringList& list)
{
    d->setAttribute(QLatin1String("creators"), list);
}

QStringList KPImageInfo::creators() const
{
    return d->attribute(QLatin1String("creators")).toStringList();
}

bool KPImageInfo::hasCreators() const
{
    return d->hasAttribute(QLatin1String("creators"));
}

void KPImageInfo::setCredit(const QString& val)
{
    d->setAttribute(QLatin1String("credit"), val);
}

QString KPImageInfo::credit() const
{
    return d->attribute(QLatin1String("credit")).toString();
}

bool KPImageInfo::hasCredit() const
{
    return d->hasAttribute(QLatin1String("credit"));
}

void KPImageInfo::setRights(const QString& val)
{
    d->setAttribute(QLatin1String("rights"), val);
}

QString KPImageInfo::rights() const
{
    return d->attribute(QLatin1String("rights")).toString();
}

bool KPImageInfo::hasRights() const
{
    return d->hasAttribute(QLatin1String("rights"));
}

void KPImageInfo::setSource(const QString& val)
{
    d->setAttribute(QLatin1String("source"), val);
}

QString KPImageInfo::source() const
{
    return d->attribute(QLatin1String("source")).toString();
}

bool KPImageInfo::hasSource() const
{
    return d->hasAttribute(QLatin1String("source"));
}

}  // namespace KIPIPlugins
