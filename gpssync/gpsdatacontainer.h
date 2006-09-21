/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-19
 * Description : GPS data container.
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef GPSDATACONTAINER_H
#define GPSDATACONTAINER_H

// Qt includes.

namespace KIPIGPSSyncPlugin
{

class GPSDataContainer
{
public:
    
    GPSDataContainer(): m_altitude(0.0), m_latitude(0.0), m_longitude(0.0), m_speed(0.0){}

    GPSDataContainer(double altitude, double latitude, double longitude, double speed)
                   : m_altitude(altitude), m_latitude(latitude),
                     m_longitude(longitude), m_speed(speed)
    {}

    void setAltitude(double alt)  { m_altitude  = alt; }
    void setLatitude(double lat)  { m_latitude  = lat; }
    void setLongitude(double lng) { m_longitude = lng; }
    void setSpeed(double spd)     { m_speed     = spd; }

    double altitude()  const { return m_altitude;  }
    double latitude()  const { return m_latitude;  }
    double longitude() const { return m_longitude; }
    double speed()     const { return m_speed;     }
    
private:
    
    double m_altitude;
    double m_latitude;
    double m_longitude;
    double m_speed;
};

} // NameSpace KIPIGPSSyncPlugin

#endif  // GPSDATACONTAINER_H
