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

#include "kpquickimageinfo.h"

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
#include "kpquickglobal.h"

#define KP_QUICK_FQN( ns_or_class, member ) ns_or_class::member

// Macros creates wrapper that sets new value and emits changed signal. To avoid duplicating
// params checks, wrapper first sets new value using original setter, and only then checks
// if value has changed. Downside is that there are 2 calls to getter instead of one
#define KP_QUICK_SETTER_WRAPPER( type, setter, getter, signal ) \
void KP_QUICK_FQN( KPQuickImageInfo, setter) ( type newValue )\
{\
    type oldValue = KP_QUICK_FQN(KPImageInfo, getter)();\
    KP_QUICK_FQN(KPImageInfo, setter)(newValue);\
    if( oldValue != newValue ) {\
        emit signal(newValue);\
    }\
}

using namespace KIPI;

namespace KIPIPlugins
{

KPQuickImageInfo::KPQuickImageInfo(QObject* parent) :
    QObject(parent), KPImageInfo(QUrl())
{
}

KPQuickImageInfo::KPQuickImageInfo(const QUrl& url, QObject* parent):
    QObject(parent), KPImageInfo(url)
{
    connect( this, &KPQuickImageInfo::urlChanged, this, &KPQuickImageInfo::onUrlChanged );
	updateDependentData();
}

KPQuickImageInfo::~KPQuickImageInfo()
{
}

void KPQuickImageInfo::updateDependentData()
{
    m_thumbnailUrl = createThumbnailUrl(url());
    m_previewUrl = createPreviewUrl(url());
    qCDebug(KIPIPLUGINS_LOG) << "Thumbnail url: " << m_thumbnailUrl;
}

void KPQuickImageInfo::setUrl( const QUrl& newValue )
{
    QUrl oldValue = KPImageInfo::url();
	KPImageInfo::setUrl(newValue);
    if( oldValue != newValue ) {
		updateDependentData();
        emit urlChanged(newValue);
    }
}

KP_QUICK_SETTER_WRAPPER(const QString&, setDescription, description, descriptionChanged)
KP_QUICK_SETTER_WRAPPER(const QStringList&, setTagsPath, tagsPath, tagsPathChanged)
KP_QUICK_SETTER_WRAPPER(int, setRating, rating, ratingChanged)
KP_QUICK_SETTER_WRAPPER(int, setColorLabel, colorLabel, colorLabelChanged)
KP_QUICK_SETTER_WRAPPER(int, setPickLabel, pickLabel, pickLabelChanged)
KP_QUICK_SETTER_WRAPPER(const QDateTime&, setDate, date, dateChanged)
KP_QUICK_SETTER_WRAPPER(const QString&, setTitle, title, titleChanged)
KP_QUICK_SETTER_WRAPPER(const QString&, setName, name, nameChanged)
KP_QUICK_SETTER_WRAPPER(double, setLatitude, latitude, latitudeChanged)
KP_QUICK_SETTER_WRAPPER(double, setLongitude, longitude, longitudeChanged)
KP_QUICK_SETTER_WRAPPER(double, setAltitude, altitude, altitudeChanged)
KP_QUICK_SETTER_WRAPPER(int, setOrientation, orientation, orientationChanged)
KP_QUICK_SETTER_WRAPPER(const QStringList&, setCreators, creators, creatorsChanged )
KP_QUICK_SETTER_WRAPPER(const QString&, setCredit, credit, creditChanged)
KP_QUICK_SETTER_WRAPPER(const QString&, setRights, rights, rightsChanged)
KP_QUICK_SETTER_WRAPPER(const QString&, setSource, source, sourceChanged)

#define KP_QUICK_EMIT_UPDATE(attribute) emit attribute ## Changed (attribute());

/* Emit all xxxChanged signals with new values */
void KPQuickImageInfo::onUrlChanged(const QUrl&)
{
    KP_QUICK_EMIT_UPDATE(fileSize);
    KP_QUICK_EMIT_UPDATE(description);
    KP_QUICK_EMIT_UPDATE(tagsPath);
    KP_QUICK_EMIT_UPDATE(keywords);
    KP_QUICK_EMIT_UPDATE(rating);
    KP_QUICK_EMIT_UPDATE(colorLabel);
    KP_QUICK_EMIT_UPDATE(pickLabel);
    KP_QUICK_EMIT_UPDATE(date);
    KP_QUICK_EMIT_UPDATE(title);
    KP_QUICK_EMIT_UPDATE(name);
    KP_QUICK_EMIT_UPDATE(latitude);
    KP_QUICK_EMIT_UPDATE(longitude);
    KP_QUICK_EMIT_UPDATE(altitude);
    KP_QUICK_EMIT_UPDATE(orientation);
    KP_QUICK_EMIT_UPDATE(creators);
    KP_QUICK_EMIT_UPDATE(credit);
    KP_QUICK_EMIT_UPDATE(rights);
    KP_QUICK_EMIT_UPDATE(source);
}

}  // namespace KIPIPlugins
