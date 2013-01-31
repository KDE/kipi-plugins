/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageInfo to manage easily
 *               item properties with KIPI host application.
 *
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <libkipi/pluginloader.h>

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
            if (!map.isEmpty()) return map.value(name, QVariant());
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

    KUrl       url;
    Interface* iface;
};

KPImageInfo::KPImageInfo(const KUrl& url)
    : d(new Private)
{
    d->url = url;
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
    if (d->hasValidData())
    {
        ImageInfo srcInfo  = d->iface->info(d->url);
        ImageInfo destInfo = d->iface->info(destination);
        destInfo.cloneData(srcInfo);
    }
}

qlonglong KPImageInfo::fileSize() const
{
    if (hasFileSize()) return d->attribute("filesize").toLongLong();

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
    return d->hasAttribute("filesize");
}

void KPImageInfo::setDescription(const QString& desc)
{
    if (d->iface)
    {
        d->setAttribute("comment", desc);

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
        if (hasDescription()) return d->attribute("comment").toString();

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
        return d->hasAttribute("comment");

    return (!description().isNull());
}

void KPImageInfo::setDate(const QDateTime& date)
{
    d->setAttribute("date", date);

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
    if (hasDate()) return d->attribute("date").toDateTime();

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
    return d->hasAttribute("date");
}

bool KPImageInfo::isExactDate() const
{
    if (d->hasAttribute("isexactdate"))
        return d->attribute("isexactdate").toBool();

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
    d->setAttribute("name", name);

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
    if (hasName()) return d->attribute("name").toString();

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
    return d->hasAttribute("name");
}

void KPImageInfo::setOrientation(KPMetadata::ImageOrientation orientation)
{
    d->setAttribute("orientation", (int)orientation);
    d->setAttribute("angle",       (int)orientation);     // NOTE: For compatibility.

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

    if (d->hasAttribute("orientation"))
        orientation = (KPMetadata::ImageOrientation)(d->attribute("orientation").toInt());
    else if (d->hasAttribute("angle"))
        orientation = (KPMetadata::ImageOrientation)(d->attribute("angle").toInt());    // NOTE: For compatibility.

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
    return (d->hasAttribute("orientation") || 
            d->hasAttribute("angle"));          // NOTE: For compatibility.
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
    QStringList keywords;

    if(d->iface)
    {
        keywords = d->attribute("keywords").toStringList();
        if (keywords.isEmpty())
            keywords = d->attribute("tags").toStringList();     // NOTE: For compatibility.
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
        return (d->hasAttribute("keywords") ||
                d->hasAttribute("tags"));       // NOTE: For compatibility.
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
    d->setAttribute("creators", list);
}

QStringList KPImageInfo::creators() const
{
    return d->attribute("creators").toStringList();
}

bool KPImageInfo::hasCreators() const
{
    return d->hasAttribute("creators");
}

void KPImageInfo::setCredit(const QString& val)
{
    d->setAttribute("credit", val);
}

QString KPImageInfo::credit() const
{
    return d->attribute("credit").toString();
}

bool KPImageInfo::hasCredit() const
{
    return d->hasAttribute("credit");
}

void KPImageInfo::setRights(const QString& val)
{
    d->setAttribute("rights", val);
}

QString KPImageInfo::rights() const
{
    return d->attribute("rights").toString();
}

bool KPImageInfo::hasRights() const
{
    return d->hasAttribute("rights");
}

void KPImageInfo::setSource(const QString& val)
{
    d->setAttribute("source", val);
}

QString KPImageInfo::source() const
{
    return d->attribute("source").toString();
}

bool KPImageInfo::hasSource() const
{
    return d->hasAttribute("source");
}

}  // namespace KIPIPlugins
