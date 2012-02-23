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

// Local includes

#include "kipiplugins_export.h"
#include "kpmetadata.h"

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPImageInfo
{

public:

    /** Contructor with KIPI interface instance get from plugin and item url that you want to manage.
     */
    KPImageInfo(Interface* const iface, const KUrl& url);
    ~KPImageInfo();

    /** return item url.
     */
    KUrl url() const;

    /** Clone all attributes from current KPImageInfo instance to item pointed by destination url.
     *  In other words, url of KPImageInfo instance is the source of attributes to clone on destination.
     */
    void cloneData(const KUrl& destination);

    /** Manage item file size in bytes.
     */
    qlonglong fileSize() const;
    bool      hasFileSize() const;

    /** Manage description (lead comment) of item with KIPI host application.
     *  NOTE: if interface is null, description is managed through image metadata directly.
     */
    void    setDescription(const QString& desc);
    QString description() const;
    bool    hasDescription() const;

    /** Manage complete tags path of item.
     */
    void        setTagsPath(const QStringList& tp);
    QStringList tagsPath() const;
    bool        hasTagsPath() const;

    /** Get keywords list (tag names) of item with KIPI host application.
     *  NOTE: if interface is null, keywords are managed through image metadata directly.
     */
    QStringList keywords() const;
    bool        hasKeywords() const;

    /** Manage rating (0-5 stars) of item.
     */
    void setRating(int r);
    int  rating() const;
    bool hasRating() const;
    
    /** Manage color label of item (0-10 : none, red, orange, yellow, green, blue, magenta, gray, black, white)
     */
    void setColorLabel(int cl);
    int  colorLabel() const;
    bool hasColorLabel() const;

    /** Manage pick label of item (0-4 : none, rejected, pending, accepted)
     */
    void setPickLabel(int pl);
    int  pickLabel() const;
    bool hasPickLabel() const;

    /** Manage date of item.
     */
    void      setDate(const QDateTime& date);
    QDateTime date() const;
    bool      hasDate() const;

    /** In the case the application supports time ranges (like this image is from 1998-2000),
        this attribute will be true if the date is an exact specification, and thus not a range.
        If KIPI host do not support date range, thi smethod return always true.
    */
    bool      isExactDate() const;

    /** Manage title of item.
     */
    void    setTitle(const QString& title);
    QString title() const;
    bool    hasTitle() const;

    /** Manage item name.
     */
    void    setName(const QString& name);
    QString name() const;
    bool    hasName() const;

    /** Manage item latitude geolocation information : double value in degrees (-90.0 >= lat <=90.0).
     */
    void   setLatitude(double lat);
    double latitude() const;
    bool   hasLatitude() const;

    /** Manage item longitude geolocation information : double value in degrees (-180.0 >= long <=180.0).
     */
    void   setLongitude(double lng);
    double longitude() const;
    bool   hasLongitude() const;

    /** Manage item altitude geolocation information : double value in meters.
     */
    void   setAltitude(double alt);
    double altitude() const;
    bool   hasAltitude() const;

    /** Return true if all geolocation attributes are available (latitude, longitude, and altitude).
     */
    bool hasGeolocationInfo() const;

    /** Remove all geolocation attributes of item.
     */
    void removeGeolocationInfo();

    /** Manage orientation of item. See libkexiv2 library for details.
     *  Use RotationMatrix::toMatrix() from libkexiv2 to get a QMatrix corresponding to orientation flage and to apply it on QImage.
     */
    void                         setOrientation(KPMetadata::ImageOrientation orientation);
    KPMetadata::ImageOrientation orientation() const;
    bool                         hasOrientation() const;

private:

    class KPImageInfoPrivate;
    KPImageInfoPrivate* const d;
};

} // namespace KIPIPlugins

#endif  // KPIMAGEINFO_H
