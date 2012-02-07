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

#ifndef KPIMAGEINFO_H
#define KPIMAGEINFO_H

// Qt includes

#include <QList>
#include <QString>
#include <QStringList>
#include <QDateTime>

// KDE includes

#include <kurl.h>

// LibKexiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPI
{
    class Interface;
}

using namespace KExiv2Iface;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPImageInfo
{

public:

    /** Contructor with KIPI interface instance get from plugin and item url that you want to manage.
     */
    KPImageInfo(KIPI::Interface* iface, const KUrl& url);
    ~KPImageInfo();

    /** Return true if KIPI host application provide attribute name for item
     *  List of supported attributes is given in KIPI::ImageInfo class.
     */
    bool hasAttribute(const QString& name) const;

    /** Clone all attributes from current KPImageInfo instance to item pointed by destination url.
     *  In other words, url of KPImageInfo instance is the source of attributes to clone on destination.
     */
    void cloneData(const KUrl& destination);

    /** Manage description (lead comment) of item.
     */
    void    setDescription(const QString& desc);
    QString description() const;

    /** Manage complete tags path of item.
     */
    void        setTagsPath(const QStringList& tp);
    QStringList tagsPath() const;

    /** Get keywords list (tag names) of item.
     */
    QStringList keywords() const;

    /** Manage rating (0-5 stars) of item.
     */
    void setRating(int r);
    int  rating() const;

    /** Manage date of item.
     */
    void      setDate(const QDateTime& date);
    QDateTime date() const;

    /** Manage title of item.
     */
    void    setTitle(const QString& title);
    QString title() const;

    /** Manage item name.
     */
    void setName(const QString& name);
    QString name() const;

    /** Return true if all geolocation info are available (latitude, longitude, and altitude)
     */
    bool hasFullGeolocationInfo() const;

    /** Manage item latitude geolocation information : double value in degrees (-90.0 >= lat <=90.0).
     */
    void   setLatitude(double lat);
    double latitude() const;

    /** Manage item longitude geolocation information : double value in degrees (-180.0 >= long <=180.0).
     */
    void   setLongitude(double lng);
    double longitude() const;

    /** Manage item altitude geolocation information : double value in meters.
     */
    void   setAltitude(double alt);
    double altitude() const;

    /** Manage orientation of item. See KExiv2::ImageOrientation for possible values.
     *  Use KExiv2Iface::RotationMatrix::toMatrix() to get a QMatrix corresponding to orientation flage and to apply on QImage.
     */
    void setOrientation(KExiv2::ImageOrientation orientation);
    KExiv2::ImageOrientation orientation() const;

private:

    class KPImageInfoPrivate;
    KPImageInfoPrivate* const d;
};

} // namespace KIPIPlugins

#endif  // KPIMAGEINFO_H
