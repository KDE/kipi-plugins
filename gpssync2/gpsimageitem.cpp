/* ============================================================
 *
 * Date        : 2010-03-21
 * Description : An item to hold information about an image
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "gpsimageitem.h"

// KDE includes

#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// local includes

#include "kipiimagemodel.h"

namespace KIPIGPSSyncPlugin
{

GPSImageItem::GPSImageItem(KIPI::Interface* const interface, const KUrl& url, const bool autoLoad)
: KipiImageItem(interface, url, false)
{
    if (autoLoad)
    {
        loadImageData();
    }
}

GPSImageItem::~GPSImageItem()
{
}

void GPSImageItem::loadImageDataInternal()
{
    KipiImageItem::loadImageDataInternal();

    m_coordinates.clear();

    if (m_interface)
    {
        // try to load the GPS data from the KIPI interface:
        QMap<QString, QVariant> attributes;
        KIPI::ImageInfo info = m_interface->info(m_url);
        attributes = info.attributes();

        if (attributes.contains("latitude") &&
            attributes.contains("longitude"))
        {
            m_coordinates.setLatLon(attributes["latitude"].toDouble(), attributes["longitude"].toDouble());

            if (attributes.contains("altitude"))
            {
                m_coordinates.setAlt(attributes["altitude"].toDouble());
            }
        }
    }

    if (!m_coordinates.hasCoordinates())
    {
        // could not load the coordinates from the interface,
        // read them directly from the file
        openExiv2IFaceIfNotOpen(false);

        double alt, lat, lng;
        // TODO: handle missing altitude properly!
        bool infoIsValid = m_exiv2Iface->getGPSInfo(alt, lat, lng);
        if (infoIsValid)
        {
            m_coordinates = WMW2::WMWGeoCoordinate(lat, lng, alt);
        }
    }
}

QVariant GPSImageItem::data(const int column, const int role) const
{
    if (role==RoleCoordinates)
    {
        return QVariant::fromValue(m_coordinates);
    }
    else if ((column==ColumnLatitude)&&(role==Qt::DisplayRole))
    {
        if (!m_coordinates.hasLatitude())
            return QString();

        return KGlobal::locale()->formatNumber(m_coordinates.lat());
    }
    else if ((column==ColumnLongitude)&&(role==Qt::DisplayRole))
    {
        if (!m_coordinates.hasLongitude())
            return QString();

        return KGlobal::locale()->formatNumber(m_coordinates.lon());
    }
    else if ((column==ColumnAltitude)&&(role==Qt::DisplayRole))
    {
        if (!m_coordinates.hasAltitude())
            return QString();
    
        return KGlobal::locale()->formatNumber(m_coordinates.alt());
    }

    return KipiImageItem::data(column, role);
}

bool GPSImageItem::setData(const int column, const int role, const QVariant& value)
{
    if (role==RoleCoordinates)
    {
        if (value.canConvert<WMW2::WMWGeoCoordinate>())
        {
            m_coordinates = value.value<WMW2::WMWGeoCoordinate>();
        }
    }
    else
    {
        return KipiImageItem::setData(column, role, value);
    }

    return true;
}

void GPSImageItem::setCoordinates(const WMW2::WMWGeoCoordinate& newCoordinates)
{
    m_coordinates = newCoordinates;
    emitDataChanged();
}

void GPSImageItem::setHeaderData(KipiImageModel* const model)
{
    KipiImageItem::setHeaderData(model);

    model->setColumnCount(ColumnGPSImageItemCount);
    model->setHeaderData(ColumnLatitude, Qt::Horizontal, i18n("Latitude"), Qt::DisplayRole);
    model->setHeaderData(ColumnLongitude, Qt::Horizontal, i18n("Longitude"), Qt::DisplayRole);
    model->setHeaderData(ColumnAltitude, Qt::Horizontal, i18n("Altitude"), Qt::DisplayRole);
}

} /* KIPIGPSSyncPlugin */

