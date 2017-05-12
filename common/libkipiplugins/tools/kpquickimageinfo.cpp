/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageInfo to manage easily
 *               item properties with KIPI host application.
 *
 * Copyright (C) 2012-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Libkipi includes

#include <libkipi_version.h>
#include <KIPI/Interface>
#include <KIPI/ImageInfo>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

#define KP_QUICK_SETTER_WRAPPER( type, setter, getter, signal )
using namespace KIPI;

namespace KIPIPlugins
{

KPQuickImageInfo::KPQuickImageInfo(const QUrl& url)
    : d(new Private)
{
    d->url = url;
}

KPQuickImageInfo::~KPQuickImageInfo()
{
    delete d;
}

QUrl KPQuickImageInfo::url() const
{
    return d->url;
}

void KPQuickImageInfo::setUrl(const QUrl& url)
{
    if( d->url != url ) {
        d->url = url;
        emit urlChanged(url);
    }
}

void KPQuickImageInfo::cloneData(const QUrl& destination)
{
    if (d->hasValidData())
    {
        ImageInfo srcInfo  = d->iface->info(d->url);
        ImageInfo destInfo = d->iface->info(destination);
        destInfo.cloneData(srcInfo);
    }
}

qlonglong KPQuickImageInfo::fileSize() const
{
    if (hasFileSize())
        return d->attribute(QLatin1String("filesize")).toLongLong();

    return (-1);
}

bool KPQuickImageInfo::hasFileSize() const
{
    return d->hasAttribute(QLatin1String("filesize"));
}

void KPQuickImageInfo::setDescription(const QString& desc)
{
    if (interface())
    {
        QString oldDescription = description();
	if (desc != oldDescription) {
	    KPImageInfo::setDescription(desc);
            emit descriptionChanged(desc);
        }
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "KIPI interface is null";
    }
}

void KPQuickImageInfo::setDate(const QDateTime& date)
{
    QDateTime oldDate = KPImageInfo::date();
    if( date != oldDate ) {
        KPImageInfo::setDate(date);
        emti dateChanged(date);
    }
}

void KPQuickImageInfo::setName(const QString& name)
{
    QString oldName = KPImageInfo::name();
    if( oldName != name ) {
        KPImageInfo::setName(name);
        emit nameChanged(name);
    }
}

void KPQuickImageInfo::setOrientation(int orientation)
{
    int oldOrientation = KPImageInfo::orientation();
    if( oldOrientation != orientation ) {
        KPImageInfo::setOrientation(orientation);
        emit orientationChanged(orientation);
    }
}

void KPQuickImageInfo::setTitle(const QString& title)
{
    QString oldTitle = KPImageInfo::title();
    if( oldTitle != title ) {
        KPImageInfo::setTitle(title);
        emti titleChanged(title);
    }
}

void KPQuickImageInfo::setLatitude(double lat)
{
    if (lat < -90.0 || lat > 90)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Latitude value is out of range (" << lat << ")";
        return;
    }
    double oldLat = KPImageInfo::latitude();
    if( oldLat != lat ) {
        KPImageInfo::setLatitude(lat);
        emit latitudeChanged(lat);
    }
}

void KPQuickImageInfo::setLongitude(double lng)
{
    if (lng < -180.0  || lng > 180)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Latitude value is out of range (" << lng << ")";
        return;
    }
    double oldLng = KPImageInfo::longitude();
    if( oldLng != lng ) {
        KPImageInfo::setLongitude(lat);
        emit longitudeChanged(lat);
    }

}

void KPQuickImageInfo::setAltitude(double alt)
{
    double oldAlt = KPImageInfo::altitude();
    if( oldAlt != lat ) {
        KPImageInfo::setAltitude(lat);
        emit altitudeChanged(lat);
    }
    d->setAttribute(QLatin1String("altitude"), alt);
}


void KPQuickImageInfo::setRating(int r)
{
    if (r < 0 || r > 5)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Rating value is out of range (" << r << ")";
        return;
    }
    int oldRating = KPImageInfo::rating();
    if( oldRating != rating) {
        KPImageInfo::setRating(r);
        emit ratingChanged(r);
    }
}

void KPQuickImageInfo::setColorLabel(int cl)
{
    if (cl < 0 || cl > 10)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Color label value is out of range (" << cl << ")";
        return;
    }
    int oldCl = KPImageInfo::colorLabel();
    if( oldCl != cl ) {
        KPImageInfo::setColorLabel(cl);
        emit colorLabelChanged();
    }
}

void KPQuickImageInfo::setPickLabel(int pl)
{
    if (pl < 0 || pl > 10)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Pick label value is out of range (" << pl << ")";
        return;
    }


    d->setAttribute(QLatin1String("picklabel"), pl);
}

int KPQuickImageInfo::pickLabel() const
{
    return d->attribute(QLatin1String("picklabel")).toInt();
}

bool KPQuickImageInfo::hasPickLabel() const
{
    return d->hasAttribute(QLatin1String("picklabel"));
}

void KPQuickImageInfo::setTagsPath(const QStringList& tp)
{
    d->setAttribute(QLatin1String("tagspath"), tp);
}

QStringList KPQuickImageInfo::tagsPath() const
{
    return d->attribute(QLatin1String("tagspath")).toStringList();
}

bool KPQuickImageInfo::hasTagsPath() const
{
    return d->hasAttribute(QLatin1String("tagspath"));
}

QStringList KPQuickImageInfo::keywords() const
{
    QStringList keywords;

    if (d->iface)
    {
        keywords = d->attribute(QLatin1String("keywords")).toStringList();
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "KIPI interface is null";
    }

    return keywords;
}

bool KPQuickImageInfo::hasKeywords() const
{
    if (d->iface)
    {
        return d->hasAttribute(QLatin1String("keywords"));
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "KIPI interface is null";
    }

    return false;
}

void KPQuickImageInfo::setCreators(const QStringList& list)
{
    d->setAttribute(QLatin1String("creators"), list);
}

QStringList KPQuickImageInfo::creators() const
{
    return d->attribute(QLatin1String("creators")).toStringList();
}

bool KPQuickImageInfo::hasCreators() const
{
    return d->hasAttribute(QLatin1String("creators"));
}

void KPQuickImageInfo::setCredit(const QString& val)
{
    d->setAttribute(QLatin1String("credit"), val);
}

QString KPQuickImageInfo::credit() const
{
    return d->attribute(QLatin1String("credit")).toString();
}

bool KPQuickImageInfo::hasCredit() const
{
    return d->hasAttribute(QLatin1String("credit"));
}

void KPQuickImageInfo::setRights(const QString& val)
{
    d->setAttribute(QLatin1String("rights"), val);
}

QString KPQuickImageInfo::rights() const
{
    return d->attribute(QLatin1String("rights")).toString();
}

bool KPQuickImageInfo::hasRights() const
{
    return d->hasAttribute(QLatin1String("rights"));
}

void KPQuickImageInfo::setSource(const QString& val)
{
    d->setAttribute(QLatin1String("source"), val);
}

QString KPQuickImageInfo::source() const
{
    return d->attribute(QLatin1String("source")).toString();
}

bool KPQuickImageInfo::hasSource() const
{
    return d->hasAttribute(QLatin1String("source"));
}

}  // namespace KIPIPlugins
