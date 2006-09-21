/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-19
 * Description : GPS data file parser. 
 *               (GPX format http://www.topografix.com/gpx.asp).
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

#ifndef GPSDATAPARSER_H
#define GPSDATAPARSER_H

// Qt includes.

#include <qdatetime.h>
#include <qmap.h>

// KDE includes.

#include <kurl.h>

namespace KIPIGPSSyncPlugin
{

class GPSData
{
public:
    
    GPSData(): m_altitude(0.0), m_latitude(0.0), m_longitude(0.0), m_speed(0.0){}

    GPSData(double altitude, double latitude, double longitude, double speed)
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
    
    double    m_altitude;
    double    m_latitude;
    double    m_longitude;
    double    m_speed;
};

class GPSDataParser
{
	
public:

	GPSDataParser();
	~GPSDataParser(){};	

    bool loadGPXFile(const KURL& url);    

    void clear();
    int  numPoints();
    bool parseDates(QDateTime dateTime, int averageSecs, double& alt, double& lat, double& lon);

private:

    double calculateDistance(double lon1, double lat1, double lon2, double lat2);
    int    calculateTimeDiff(QDateTime date1, QDateTime date2);

private: 

    typedef QMap<QDateTime, GPSData> GPSDataMap; 

    GPSDataMap m_GPSDataMap;
};

} // NameSpace KIPIGPSSyncPlugin

#endif  // GPSDATAPARSER_H
