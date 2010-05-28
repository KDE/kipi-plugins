/* ============================================================
 *
 * Date        : 2010-04-19
 * Description : A class to hold the GPS related data
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

#ifndef GPSDATACONTAINER_H
#define GPSDATACONTAINER_H

// WMW2 includes

#include "worldmapwidget2/worldmapwidget2_primitives.h"

namespace KIPIGPSSyncPlugin
{

class GPSDataContainer
{
public:
    GPSDataContainer()
    : m_hasFlags(0),
      m_coordinates(),
      m_nSatellites(0)
    {
    }

    bool operator==(const GPSDataContainer& b) const
    {
        if (m_hasFlags != b.m_hasFlags)
            return false;

        if (m_hasFlags.testFlag(HasCoordinates))
        {
            if (!(m_coordinates==b.m_coordinates))
                return false;
        }

        if (m_nSatellites!=b.m_nSatellites)
            return false;

        return true;
    }

    enum HasFlagsEnum {
        HasCoordinates = 1,
        HasAltitude = 2,
        HasIsInterpolated = 4,
        HasNSatellites = 8
    };
    Q_DECLARE_FLAGS(HasFlags, HasFlagsEnum)

    HasFlags m_hasFlags;
    WMW2::WMWGeoCoordinate m_coordinates;
    int m_nSatellites;

    void setCoordinates(const WMW2::WMWGeoCoordinate& coordinates)
    {
        m_coordinates = coordinates;
        if (coordinates.hasCoordinates())
        {
            m_hasFlags|=HasCoordinates;
        }
        else
        {
            m_hasFlags&=~HasCoordinates;
        }
        if (coordinates.hasAltitude())
        {
            m_hasFlags|=HasAltitude;
        }
        else
        {
            m_hasFlags&=~HasAltitude;
        }
    }

    void setAltitude(const qreal alt)
    {
        m_coordinates.setAlt(alt);
        m_hasFlags|=HasAltitude;
    }


    void setLatLon(const qreal lat, const qreal lon)
    {
        m_coordinates.setLatLon(lat, lon);
        m_hasFlags|=HasCoordinates;
    }

    void clear()
    {
        m_hasFlags = 0;
    }

    bool hasCoordinates() const
    {
        return m_hasFlags.testFlag(HasCoordinates);
    }
};

} /* KIPIGPSSyncPlugin */

Q_DECLARE_OPERATORS_FOR_FLAGS(KIPIGPSSyncPlugin::GPSDataContainer::HasFlags)

#endif /* GPSDATACONTAINER_H */

