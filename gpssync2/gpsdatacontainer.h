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
    : m_dirtyFlags(0),
      m_hasFlags(0),
      m_coordinates(),
      m_nSatellites(0)
    {
    }

    enum DirtyFlagsEnum {
        DirtyCoordinates = 1,
        DirtySatellites = 2
    };
    Q_DECLARE_FLAGS(DirtyFlags, DirtyFlagsEnum)

    enum HasFlagsEnum {
        HasCoordinates = 1,
        HasAltitude = 2,
        HasIsInterpolated = 4,
        HasNSatellites = 8
    };
    Q_DECLARE_FLAGS(HasFlags, HasFlagsEnum)

    DirtyFlags m_dirtyFlags;
    HasFlags m_hasFlags;
    WMW2::WMWGeoCoordinate m_coordinates;
    int m_nSatellites;

    void setCoordinates(const WMW2::WMWGeoCoordinate& coordinates)
    {
        m_coordinates = coordinates;
        m_dirtyFlags|=DirtyCoordinates;
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
        m_dirtyFlags|=DirtyCoordinates;
    }
        

    void setLatLon(const qreal lat, const qreal lon)
    {
        m_coordinates.setLatLon(lat, lon);
        m_dirtyFlags|=DirtyCoordinates;
        m_hasFlags|=HasCoordinates;
    }

    void clear()
    {
        m_hasFlags = 0;
        m_dirtyFlags = 0;
    }
        
};

} /* KIPIGPSSyncPlugin */

Q_DECLARE_OPERATORS_FOR_FLAGS(KIPIGPSSyncPlugin::GPSDataContainer::DirtyFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(KIPIGPSSyncPlugin::GPSDataContainer::HasFlags)

#endif /* GPSDATACONTAINER_H */

