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

// C++ includes.

#include <cmath>
#include <cstdlib>

// Qt includes.

#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qdom.h>
#include <qtextstream.h>

// KDE includes.

#include <kdebug.h>

// Local includes.

#include "gpsdataparser.h"

namespace KIPIGPSSyncPlugin
{

GPSDataParser::GPSDataParser()
{
    clear();
}

void GPSDataParser::clear()
{
    m_GPSDataMap.clear();
}

int GPSDataParser::numPoints()
{
    return m_GPSDataMap.count();
}

bool GPSDataParser::parseDates(QDateTime dateTime, int averageSecs, double& alt, double& lat, double& lon)
{
    for (GPSDataMap::Iterator it = m_GPSDataMap.begin();
         it != m_GPSDataMap.end(); ++it )
    {
        int nbSecs = abs(dateTime.secsTo( it.key() ));
        if( nbSecs < averageSecs )
        {
            GPSData data = m_GPSDataMap[it.key()];
            alt = data.altitude();
            lat = data.latitude();
            lon = data.longitude();
            return true;
        }
    }

    return false;
}

bool GPSDataParser::loadGPXFile(const KURL& url)
{
    QFile gpxfile(url.path());

    if (!gpxfile.open(IO_ReadOnly))
        return false;

    QDomDocument gpxDoc("gpx");
    if (!gpxDoc.setContent(&gpxfile))
        return false;

    QDomElement gpxDocElem = gpxDoc.documentElement();
    if (gpxDocElem.tagName()!="gpx")
        return false;
    
    for (QDomNode nTrk = gpxDocElem.firstChild();
         !nTrk.isNull(); nTrk = nTrk.nextSibling()) 
    {
        QDomElement trkElem = nTrk.toElement();
        if (trkElem.isNull()) continue;
        if (trkElem.tagName() != "trk") continue;

        for (QDomNode nTrkseg = trkElem.firstChild();
            !nTrkseg.isNull(); nTrkseg = nTrkseg.nextSibling()) 
        {
            QDomElement trksegElem = nTrkseg.toElement();
            if (trksegElem.isNull()) continue;
            if (trksegElem.tagName() != "trkseg") continue;

            for (QDomNode nTrkpt = trksegElem.firstChild();
                !nTrkpt.isNull(); nTrkpt = nTrkpt.nextSibling()) 
            {
                QDomElement trkptElem = nTrkpt.toElement();
                if (trkptElem.isNull()) continue;
                if (trkptElem.tagName() != "trkpt") continue;

                QDateTime ptDateTime;
                double    ptAltitude  = 0.0;
                double    ptLatitude  = 0.0;
                double    ptLongitude = 0.0;

                // Get GPS position. If not available continue to next point.
                QString lat = trkptElem.attribute("lat");
                QString lon = trkptElem.attribute("lon");
                if (lat.isEmpty() || lon.isEmpty()) continue;

                ptLatitude  = lat.toDouble();
                ptLongitude = lon.toDouble();

                // Get metadata of track point (altitude and time stamp)
                for (QDomNode nTrkptMeta = trkptElem.firstChild();
                    !nTrkptMeta.isNull(); nTrkptMeta = nTrkptMeta.nextSibling()) 
                {
                    QDomElement trkptMetaElem = nTrkptMeta.toElement();
                    if (trkptMetaElem.isNull()) continue;
                    if (trkptMetaElem.tagName() == QString("time")) 
                    {
                        // Get GPS point time stamp. If not available continue to next point.
                        QString time = trkptMetaElem.text();
                        if (time.isEmpty()) continue;
                        ptDateTime = QDateTime::fromString(time, Qt::ISODate);
                    }
                    if (trkptMetaElem.tagName() == QString("ele")) 
                    {
                        // Get GPS point altitude. If not available continue to next point.
                        QString ele = trkptMetaElem.text();
                        if (!ele.isEmpty())
                            ptAltitude  = ele.toDouble();
                    }
                }

                if (ptDateTime.isNull())
                    continue;

                GPSData gpsData(ptAltitude, ptLatitude, ptLongitude, 0.0);
                m_GPSDataMap.insert( ptDateTime, gpsData );

/*
                kdDebug( 51001 ) << "Date:" << ptDateTime 
                                 << "Alt:"  << ptAltitude 
                                 << "Lat:"  << ptLatitude 
                                 << "Lon:"  << ptLongitude 
                                 << endl;*/
            }
        }
    }

    kdDebug( 51001 ) << "GPX File " << url.fileName() 
                     << " parsed with " << numPoints() 
                     << " points extracted" << endl;
    return true;
}

/* NOTE: Since this class can parse GPX file, this code is obsolete !

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
    }*/

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

