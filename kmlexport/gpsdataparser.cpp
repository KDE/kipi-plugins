/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-09-19
 * Description : GPS data file parser.
 *               (GPX format http://www.topografix.com/gpx.asp).
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "gpsdataparser.h"
#include "gpsdataparser_time.h"

// C++ includes

#include <cmath>
#include <cstdlib>

// Qt includes

#include <QDomDocument>
#include <QFile>
#include <QString>

// KDE includes

#include <kdebug.h>

namespace KIPIKMLExportPlugin
{

GPSDataParser::GPSDataParser()
{
    clear();
}

void GPSDataParser::clear()
{
    m_GPSDataMap.clear();
}

int GPSDataParser::numPoints() const
{
    return m_GPSDataMap.count();
}

bool GPSDataParser::matchDate(const QDateTime& photoDateTime, int maxGapTime, int secondsOffset,
                              bool offsetContainsTimeZone,
                              bool interpolate, int interpolationDstTime,
                              GPSDataContainer* const gpsData)
{
    // GPS device are sync in time by satelite using GMT time.
    QDateTime cameraGMTDateTime = photoDateTime.addSecs(secondsOffset*(-1));

    if (offsetContainsTimeZone)
    {
        cameraGMTDateTime.setTimeSpec(Qt::UTC);
    }

    kDebug() << "    photoDateTime: " << photoDateTime << photoDateTime.timeSpec();
    kDebug() << "cameraGMTDateTime: " << cameraGMTDateTime << cameraGMTDateTime.timeSpec();

    // We are trying to find the right date in the GPS points list.
    bool findItem = false;
    int nbSecItem = maxGapTime;
    int nbSecs;

    for (GPSDataMap::ConstIterator it = m_GPSDataMap.constBegin(); it != m_GPSDataMap.constEnd(); ++it )
    {
        // Here we check a possible accuracy in seconds between the
        // Camera GMT time and the GPS device GMT time.

        nbSecs = abs(cameraGMTDateTime.secsTo( it.key() ));
//         kDebug() << it.key() << cameraGMTDateTime << nbSecs;
//         kDebug() << it.key().timeSpec() << cameraGMTDateTime.timeSpec() << nbSecs;

        // We tring to find the minimal accuracy.
        if( nbSecs < maxGapTime && nbSecs < nbSecItem)
        {
            if (gpsData)
                *gpsData = m_GPSDataMap[it.key()];

            findItem  = true;
            nbSecItem = nbSecs;
        }
    }

    if (findItem) return true;

    // If we can't find it, we will trying to interpolate the GPS point.

    if (interpolate)
    {
        // The interpolate GPS point will be separate by at the maximum of 'interpolationDstTime'
        // seconds before and after the next and previous real GPS point found.

        QDateTime prevDateTime = findPrevDate(cameraGMTDateTime, interpolationDstTime);
        QDateTime nextDateTime = findNextDate(cameraGMTDateTime, interpolationDstTime);

        if (!nextDateTime.isNull() && !prevDateTime.isNull())
        {
            GPSDataContainer prevGPSPoint = m_GPSDataMap[prevDateTime];
            GPSDataContainer nextGPSPoint = m_GPSDataMap[nextDateTime];

            double alt1 = prevGPSPoint.altitude();
            double lon1 = prevGPSPoint.longitude();
            double lat1 = prevGPSPoint.latitude();
            uint   t1   = prevDateTime.toTime_t();
            double alt2 = nextGPSPoint.altitude();
            double lon2 = nextGPSPoint.longitude();
            double lat2 = nextGPSPoint.latitude();
            uint   t2   = nextDateTime.toTime_t();
            uint   tCor = cameraGMTDateTime.toTime_t();

            if (tCor-t1 != 0)
            {
                if (gpsData)
                {
                    gpsData->setAltitude(alt1  + (alt2-alt1) * (tCor-t1)/(t2-t1));
                    gpsData->setLatitude(lat1  + (lat2-lat1) * (tCor-t1)/(t2-t1));
                    gpsData->setLongitude(lon1 + (lon2-lon1) * (tCor-t1)/(t2-t1));
                    gpsData->setInterpolated(true);
                }
                return true;
            }
        }
    }

    return false;
}

QDateTime GPSDataParser::findNextDate(const QDateTime& dateTime, int secs)
{
    // We will find the item in GPS data list where the time is
    // at the maximum bigger than 'secs' mn of the value to match.
    QDateTime itemFound = dateTime.addSecs(secs);
    bool found          = false;

    for (GPSDataMap::ConstIterator it = m_GPSDataMap.constBegin(); it != m_GPSDataMap.constEnd(); ++it )
    {
        if (it.key() > dateTime)
        {
            if (it.key() < itemFound)
            {
                itemFound = it.key();
                found = true;
            }
        }
    }

    if (found)
        return itemFound;

    return QDateTime();
}

QDateTime GPSDataParser::findPrevDate(const QDateTime& dateTime, int secs)
{
    // We will find the item in GPS data list where the time is
    // at the maximum smaller than 'secs' mn of the value to match.
    QDateTime itemFound = dateTime.addSecs((-1)*secs);
    bool found          = false;

    for (GPSDataMap::ConstIterator it = m_GPSDataMap.constBegin(); it != m_GPSDataMap.constEnd(); ++it )
    {
        if (it.key() < dateTime)
        {
            if (it.key() > itemFound)
            {
                itemFound = it.key();
                found = true;
            }
        }
    }

    if (found)
        return itemFound;

    return QDateTime();
}

bool GPSDataParser::loadGPXFile(const KUrl& url)
{
    QFile gpxfile(url.path());

    if (!gpxfile.open(QIODevice::ReadOnly))
        return false;

    QDomDocument gpxDoc("gpx");

    if (!gpxDoc.setContent(&gpxfile))
        return false;

    QDomElement gpxDocElem = gpxDoc.documentElement();

    if (gpxDocElem.tagName()!="gpx")
        return false;

    for (QDomNode nTrk = gpxDocElem.firstChild(); !nTrk.isNull(); nTrk = nTrk.nextSibling())
    {
        QDomElement trkElem = nTrk.toElement();

        if (trkElem.isNull())           continue;

        if (trkElem.tagName() != "trk") continue;

        for (QDomNode nTrkseg = trkElem.firstChild(); !nTrkseg.isNull(); nTrkseg = nTrkseg.nextSibling())
        {
            QDomElement trksegElem = nTrkseg.toElement();

            if (trksegElem.isNull())              continue;

            if (trksegElem.tagName() != "trkseg") continue;

            for (QDomNode nTrkpt = trksegElem.firstChild(); !nTrkpt.isNull(); nTrkpt = nTrkpt.nextSibling())
            {
                QDomElement trkptElem = nTrkpt.toElement();

                if (trkptElem.isNull())             continue;

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
                for (QDomNode nTrkptMeta = trkptElem.firstChild(); !nTrkptMeta.isNull(); nTrkptMeta = nTrkptMeta.nextSibling())
                {
                    QDomElement trkptMetaElem = nTrkptMeta.toElement();

                    if (trkptMetaElem.isNull()) continue;

                    if (trkptMetaElem.tagName() == QString("time"))
                    {
                        // Get GPS point time stamp. If not available continue to next point.
                        const QString time = trkptMetaElem.text();

                        if (time.isEmpty()) continue;

                        ptDateTime = GPSDataParserParseTime(time);
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

                GPSDataContainer gpsData(ptAltitude, ptLatitude, ptLongitude, false);
                m_GPSDataMap.insert( ptDateTime, gpsData );
            }
        }
    }

    kDebug(AREA_CODE_LOADING) << "GPX File " << url.fileName()
                    << " parsed with " << numPoints()
                    << " points extracted" ;
    return true;
}

} // namespace KIPIKMLExportPlugin
