/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-19
 * Description : GPS data container.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef GPSDATACONTAINER_H
#define GPSDATACONTAINER_H

namespace KIPIGPSSyncPlugin
{

class GPSDataContainer
{
public:

    GPSDataContainer(): m_interpolated(false), m_altitude(0.0), 
                        m_latitude(0.0), m_longitude(0.0) 
    {};

    GPSDataContainer(double altitude, double latitude, 
                     double longitude, bool interpolated)
                   : m_interpolated(interpolated), m_altitude(altitude),
                     m_latitude(latitude), m_longitude(longitude)
    {};

    ~GPSDataContainer()
    {};

    GPSDataContainer& operator=(const GPSDataContainer& data)
    {
        m_interpolated = data.isInterpolated();
        m_altitude     = data.altitude();
        m_latitude     = data.latitude();
        m_longitude    = data.longitude();
        return *this;
    };

    void setInterpolated(bool ite) { m_interpolated = ite; };
    void setAltitude(double alt)   { m_altitude     = alt; };
    void setLatitude(double lat)   { m_latitude     = lat; };
    void setLongitude(double lng)  { m_longitude    = lng; };

    bool   isInterpolated() const { return m_interpolated; };
    double altitude()       const { return m_altitude;     };
    double latitude()       const { return m_latitude;     };
    double longitude()      const { return m_longitude;    };

private:

    bool   m_interpolated;

    double m_altitude;
    double m_latitude;
    double m_longitude;
};

} // namespace KIPIGPSSyncPlugin

#endif  // GPSDATACONTAINER_H
