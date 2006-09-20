/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-19
 * Description : GPS data file parser.
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

// C++ includes.

#include <cmath>

// Qt includes.

#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>

// KDE includes.

#include <kdebug.h>

// Local includes.

#include "gpsdataparser.h"

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

GPSDataParser::GPSDataParser(const KURL& url)
{
    m_GPSDataMap.clear();
    openFile(url);
}

void GPSDataParser::openFile(const KURL& url)
{

    // FIXME : use an external GPSBabel instance to parse GPS data file 
    //         and extract GPS positions.

    QFile       trackFile(url.path());
    QStringList trackLinesList;
    QString     trackLine;
    
    trackFile.open(IO_ReadOnly);
    
    // Read all lines and Enter them in a QStringList    

    while ( !trackFile.atEnd() )
    {
        trackFile.readLine(trackLine, 200);  
        if (trackLine.section('"', 1, 1) == "trackpoint")
        {
            trackLinesList <<  trackLine;
        }
    }

    trackFile.close();

    int row = 0;
    QString distance;
    GPSData gpsData;
    QDateTime prevGpsDate;

    for( QStringList::Iterator it = trackLinesList.begin(); it != trackLinesList.end(); ++it )
    {
        trackLine = *it;

        if (trackLine.section('"', 1, 1) == "trackpoint")
        {
            QDateTime gpsDate;
            double alt=0.0, lat=0.0, lng=0.0, spd=0.0;

            for (int col = 1 ; col != 5 ; col++)
            {   
                if (trackLine.section('"', (2*col), (2*col)).stripWhiteSpace() == "unixtime=")
                {
                    gpsDate.setTime_t( trackLine.section('"', (2*col+1), (2*col+1)).stripWhiteSpace().toInt() );
                }
                else
                {
                    switch (col)
                    {
                    case 2:
                        alt = trackLine.section('"', (2*col+1), (2*col+1)).stripWhiteSpace().toDouble();
                        break;
                    case 3:
                        lat = trackLine.section('"', (2*col+1), (2*col+1)).stripWhiteSpace().toDouble();
                        break;
                    case 4:
                        lng = trackLine.section('"', (2*col+1), (2*col+1)).stripWhiteSpace().toDouble();
                        break;
                    }
                }
            }

            if (row!=0)
            {
                spd = calculateDistance(lng, lat, gpsData.longitude(), gpsData.latitude()) / 
                      calculateTimeDiff(gpsDate, prevGpsDate)*3600.0;
            }
        
            gpsData.setAltitude(alt);
            gpsData.setLatitude(lat);
            gpsData.setLongitude(lng);
            gpsData.setSpeed(spd);
            prevGpsDate = gpsDate;

            m_GPSDataMap.insert( gpsDate, gpsData );

            row++;
        }
    }
}

double GPSDataParser::calculateDistance(double lon1, double lat1, double lon2, double lat2)
{
    const double pi = 3.1415968;
    const int radiusEarth = 6378;
            
    lat1 = lat1*pi/180;
    lat2 = lat2*pi/180;
    lon1 = lon1*pi/180;
    lon2 = lon2*pi/180;

    return acos(cos(lat1)*cos(lon1)*cos(lat2)*cos(lon2) + 
                cos(lat1)*sin(lon1)*cos(lat2)*sin(lon2) + 
                sin(lat1)*sin(lat2)) * radiusEarth;
}

int GPSDataParser::calculateTimeDiff(QDateTime date1, QDateTime date2)
{
    QDateTime UnixTime = date2;
    return UnixTime.secsTo( date1);
}

} // NameSpace KIPIGPSSyncPlugin

