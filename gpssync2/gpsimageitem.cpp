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
: KipiImageItem(interface, url, false), m_gpsData(), m_savedState(), m_dirty(false)
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

    m_gpsData.clear();

    if (m_interface)
    {
        // try to load the GPS data from the KIPI interface:
        QMap<QString, QVariant> attributes;
        KIPI::ImageInfo info = m_interface->info(m_url);
        attributes = info.attributes();

        if (attributes.contains("latitude") &&
            attributes.contains("longitude"))
        {
            m_gpsData.setLatLon(attributes["latitude"].toDouble(), attributes["longitude"].toDouble());

            if (attributes.contains("altitude"))
            {
                m_gpsData.setAltitude(attributes["altitude"].toDouble());
            }
        }
    }

    if (!m_gpsData.m_hasFlags.testFlag(GPSDataContainer::HasCoordinates))
    {
        // could not load the coordinates from the interface,
        // read them directly from the file
        openExiv2IFaceIfNotOpen(false);

        double alt, lat, lng;
        // TODO: handle missing altitude properly!
        bool infoIsValid = m_exiv2Iface->getGPSInfo(alt, lat, lng);
        if (infoIsValid)
        {
            m_gpsData.setCoordinates(WMW2::WMWGeoCoordinate(lat, lng, alt));
        }
    }

    // mark us as not-dirty, because the data was just loaded:
    m_dirty = false;
    m_savedState = m_gpsData;
}

QVariant GPSImageItem::data(const int column, const int role) const
{
    if (role==RoleCoordinates)
    {
        return QVariant::fromValue(m_gpsData.m_coordinates);
    }
    else if ((column==ColumnLatitude)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.m_coordinates.hasLatitude())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.m_coordinates.lat(), 7);
    }
    else if ((column==ColumnLongitude)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.m_coordinates.hasLongitude())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.m_coordinates.lon(), 7);
    }
    else if ((column==ColumnAltitude)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.m_coordinates.hasAltitude())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.m_coordinates.alt());
    }
    else if ((column==ColumnHDOP)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.hasHDop())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.m_hDop);
    }
    else if ((column==ColumnNSatellites)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.hasNSatellites())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.m_nSatellites, 0);
    }
    else if ((column==ColumnStatus)&&(role==Qt::DisplayRole))
    {
        if (m_dirty)
        {
            return i18n("Modified");
        }

        return QString();
    }

    return KipiImageItem::data(column, role);
}

bool GPSImageItem::setData(const int column, const int role, const QVariant& value)
{
    if (role==RoleCoordinates)
    {
        if (value.canConvert<WMW2::WMWGeoCoordinate>())
        {
            m_gpsData.setCoordinates(value.value<WMW2::WMWGeoCoordinate>());
            m_dirty = true;
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
    m_gpsData.setCoordinates(newCoordinates);
    m_dirty = true;
    emitDataChanged();
}

void GPSImageItem::setHeaderData(KipiImageModel* const model)
{
    KipiImageItem::setHeaderData(model);

    model->setColumnCount(ColumnGPSImageItemCount);
    model->setHeaderData(ColumnLatitude, Qt::Horizontal, i18n("Latitude"), Qt::DisplayRole);
    model->setHeaderData(ColumnLongitude, Qt::Horizontal, i18n("Longitude"), Qt::DisplayRole);
    model->setHeaderData(ColumnAltitude, Qt::Horizontal, i18n("Altitude"), Qt::DisplayRole);
    model->setHeaderData(ColumnHDOP, Qt::Horizontal, i18n("HDOP"), Qt::DisplayRole);
    model->setHeaderData(ColumnNSatellites, Qt::Horizontal, i18n("# satellites"), Qt::DisplayRole);
    model->setHeaderData(ColumnStatus, Qt::Horizontal, i18n("Status"), Qt::DisplayRole);
}

QString GPSImageItem::saveChanges()
{
    // determine what is to be done first
    bool shouldRemoveCoordinates = false;
    bool shouldRemoveAltitude = false;
    bool shouldWriteCoordinates = false;
    bool shouldWriteAltitude = false;
    qreal altitude = 0;
    qreal latitude = 0;
    qreal longitude = 0;

    // do we have gps information?
    if (m_gpsData.m_hasFlags.testFlag(GPSDataContainer::HasCoordinates))
    {
        shouldWriteCoordinates = true;
        latitude = m_gpsData.m_coordinates.lat();
        longitude = m_gpsData.m_coordinates.lon();
        
        if (m_gpsData.m_hasFlags.testFlag(GPSDataContainer::HasAltitude))
        {
            shouldWriteAltitude = true;
            altitude = m_gpsData.m_coordinates.alt();
        }
        else
        {
            shouldRemoveAltitude = true;
        }
    }
    else
    {
        shouldRemoveCoordinates = true;
        shouldRemoveAltitude = true;
    }

    QString returnString;
    
    // first try to write the information to the image file
    KExiv2Iface::KExiv2* const exiv2Iface = new KExiv2Iface::KExiv2;
    bool success = exiv2Iface->load(m_url.path());
    if (!success)
    {
        // TODO: more verbosity!
        returnString = i18n("Failed to open file.");
    }
    if (success)
    {
        exiv2Iface->setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
        exiv2Iface->setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

        if (shouldWriteCoordinates)
        {
            // TODO: write the altitude only if we have it
            // TODO: write HDOP and #satellites
            success = exiv2Iface->setGPSInfo(altitude, latitude, longitude);
            if (!success)
            {
                returnString = i18n("Failed to add GPS info to image");
            }
        }
        if (shouldRemoveCoordinates)
        {
            // TODO: remove only the altitude if requested
            success = exiv2Iface->removeGPSInfo();
            if (!success)
            {
                returnString = i18n("Failed to remove GPS info from image");
            }
        }
    }
    if (success)
    {
        success = exiv2Iface->save(m_url.path());
        if (!success)
        {
            returnString = i18n("Unable to save changes to file");
        }
        else
        {
            m_dirty = false;
            m_savedState = m_gpsData;
        }
    }

    delete exiv2Iface;

    // now tell the interface about the changes
    // TODO: remove the altitude if it is not available
    if (m_interface)
    {
        if (shouldWriteCoordinates)
        {
            QMap<QString, QVariant> attributes;
            attributes.insert("latitude", latitude);
            attributes.insert("longitude", longitude);
            if (shouldWriteAltitude)
            {
                attributes.insert("altitude", altitude);
            }

            KIPI::ImageInfo info = m_interface->info(m_url);
            info.addAttributes(attributes);
        }

        if (shouldRemoveCoordinates)
        {
            QStringList listToRemove;
            listToRemove << "gpslocation";
            KIPI::ImageInfo info = m_interface->info(m_url);
            info.delAttributes(listToRemove);
        }
    }

    if (returnString.isEmpty())
    {
        // mark all changes as not dirty and tell the model:
        emitDataChanged();
    }

    return returnString;
}

/**
 * @brief Restore the gps data to @p container. Sets m_dirty to false if container equals savedState.
 */
void GPSImageItem::restoreGPSData(const GPSDataContainer& container)
{
    m_dirty = !(container == m_savedState);
    m_gpsData = container;
    emitDataChanged();
}

bool GPSImageItem::lessThan(const KipiImageItem* const otherItem, const int column) const
{
    const GPSImageItem* const otherGPSItem = dynamic_cast<const GPSImageItem*>(otherItem);
    if (!otherGPSItem)
        return false;

    switch (column)
    {
    case ColumnAltitude:
    {
        if (!m_gpsData.hasAltitude())
            return false;

        if (!otherGPSItem->m_gpsData.hasAltitude())
            return true;

        return m_gpsData.m_coordinates.alt() < otherGPSItem->m_gpsData.m_coordinates.alt();
    }

    case ColumnNSatellites:
    {
        if (!m_gpsData.hasNSatellites())
            return false;

        if (!otherGPSItem->m_gpsData.hasNSatellites())
            return true;

        return m_gpsData.m_nSatellites < otherGPSItem->m_gpsData.m_nSatellites;
    }

    case ColumnHDOP:
    {
        if (!m_gpsData.hasHDop())
            return false;

        if (!otherGPSItem->m_gpsData.hasHDop())
            return true;

        return m_gpsData.m_hDop < otherGPSItem->m_gpsData.m_hDop;
    }

    case ColumnLatitude:
    {
        if (!m_gpsData.hasCoordinates())
            return false;

        if (!otherGPSItem->m_gpsData.hasCoordinates())
            return true;

        return m_gpsData.m_coordinates.lat() < otherGPSItem->m_gpsData.m_coordinates.lat();
    }

    case ColumnLongitude:
    {
        if (!m_gpsData.hasCoordinates())
            return false;

        if (!otherGPSItem->m_gpsData.hasCoordinates())
            return true;

        return m_gpsData.m_coordinates.lon() < otherGPSItem->m_gpsData.m_coordinates.lon();
    }

    case ColumnStatus:
    {
        return m_dirty && !otherGPSItem->m_dirty;
    }

    default:
        return KipiImageItem::lessThan(otherItem, column);
    }
}

} /* KIPIGPSSyncPlugin */

