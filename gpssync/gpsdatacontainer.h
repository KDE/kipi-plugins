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

// Libkmap includes

#include <libkmap/kmap_primitives.h>

namespace KIPIGPSSyncPlugin
{

class GPSDataContainer
{
public:

    GPSDataContainer()
    : m_hasFlags(0),
      m_coordinates(),
      m_nSatellites(-1),
      m_hDop(-1),
      m_pDop(-1),
      m_fixType(-1),
      m_speed(0)
    {
    }

    enum HasFlagsEnum {
        HasCoordinates = 1,
        HasAltitude = 2,
        HasIsInterpolated = 4,
        HasNSatellites = 8,
        HasHDop = 16,
        HasPDop = 32,
        HasFixType = 64,
        HasSpeed = 128
    };
    Q_DECLARE_FLAGS(HasFlags, HasFlagsEnum)

private:

    HasFlags m_hasFlags;
    KMap::WMWGeoCoordinate m_coordinates;
    int m_nSatellites;
    qreal m_hDop;
    qreal m_pDop;
    int m_fixType;
    qreal m_speed;

public:

    /* general */

    bool operator==(const GPSDataContainer& b) const
    {
        if (m_hasFlags != b.m_hasFlags)
            return false;

        if (m_hasFlags.testFlag(HasCoordinates))
        {
            if (!(m_coordinates==b.m_coordinates))
                return false;
        }

        if (hasNSatellites())
        {
            if (m_nSatellites!=b.m_nSatellites)
                return false;
        }

        if (hasHDop())
        {
            if (m_hDop!=b.m_hDop)
                return false;
        }

        if (hasHDop())
        {
            if (m_pDop!=b.m_pDop)
                return false;
        }

        if (hasFixType())
        {
            if (m_fixType!=b.m_fixType)
                return false;
        }

        if (hasSpeed())
        {
            if (m_speed!=b.m_speed)
                return false;
        }

        return true;
    }

    inline HasFlags flags() const
    {
        return m_hasFlags;
    }

    inline void clear()
    {
        m_hasFlags = 0;
        m_coordinates.clear();
    }

    inline void clearNonCoordinates()
    {
        m_hasFlags&= ~(HasNSatellites | HasHDop | HasPDop | HasFixType | HasSpeed);
    }

    /* coordinates */

    inline KMap::WMWGeoCoordinate getCoordinates() const
    {
        return m_coordinates;
    }

    inline void setCoordinates(const KMap::WMWGeoCoordinate& coordinates)
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

        clearNonCoordinates();
    }

    inline void setAltitude(const qreal alt)
    {
        m_coordinates.setAlt(alt);
        m_hasFlags|=HasAltitude;
    }

    inline bool hasAltitude() const
    {
        return m_hasFlags.testFlag(HasAltitude);
    }

    inline void setLatLon(const qreal lat, const qreal lon)
    {
        m_coordinates.setLatLon(lat, lon);
        m_hasFlags|=HasCoordinates;

        clearNonCoordinates();
    }

    inline void clearAltitude()
    {
        m_hasFlags&=~HasAltitude;
        m_coordinates.clearAlt();
    }

    inline bool hasCoordinates() const
    {
        return m_hasFlags.testFlag(HasCoordinates);
    }

    /* NSatellites */

    inline int getNSatellites() const
    {
        return m_nSatellites;
    }

    inline bool hasNSatellites() const
    {
        return m_hasFlags.testFlag(HasNSatellites);
    }

    inline void clearNSatellites()
    {
        m_hasFlags&= ~HasNSatellites;
    }

    inline void setNSatellites(const int nSatellites)
    {
        m_nSatellites = nSatellites;
        m_hasFlags|=HasNSatellites;
    }

    /* DOP */

    inline bool hasHDop() const
    {
        return m_hasFlags.testFlag(HasHDop);
    }

    inline bool hasPDop() const
    {
        return m_hasFlags.testFlag(HasPDop);
    }

    inline bool hasFixType() const
    {
        return m_hasFlags.testFlag(HasFixType);
    }

    inline void clearHDop()
    {
        m_hasFlags&= ~HasHDop;
    }

    inline void clearPDop()
    {
        m_hasFlags&= ~HasPDop;
    }

    inline void clearAnyDop()
    {
        m_hasFlags&= ~(HasHDop | HasPDop | HasFixType);
    }

    inline void setHDop(const qreal hDop)
    {
        m_hDop = hDop;
        m_hasFlags|=HasHDop;
    }

    inline void setPDop(const qreal pDop)
    {
        m_pDop = pDop;
        m_hasFlags|=HasPDop;
    }

    inline void setFixType(const int fixType)
    {
        m_fixType = fixType;
        m_hasFlags|=HasFixType;
    }

    inline qreal getHDop() const
    {
        return m_hDop;
    }

    inline qreal getPDop() const
    {
        return m_pDop;
    }

    inline qreal getFixType() const
    {
        return m_fixType;
    }

    /* speed */

    /**
     * @brief Return the speed in m/s
     */
    inline qreal getSpeed() const
    {
        return m_speed;
    }

    inline bool hasSpeed() const
    {
        return m_hasFlags.testFlag(HasSpeed);
    }

    /**
     * @brief Set the speed in m/s
     */
    inline void setSpeed(const qreal speed)
    {
        m_hasFlags|= HasSpeed;
        m_speed = speed;
    }

    inline void clearSpeed()
    {
        m_hasFlags&=~HasSpeed;
    }
};

} /* KIPIGPSSyncPlugin */

Q_DECLARE_OPERATORS_FOR_FLAGS(KIPIGPSSyncPlugin::GPSDataContainer::HasFlags)

#endif /* GPSDATACONTAINER_H */

