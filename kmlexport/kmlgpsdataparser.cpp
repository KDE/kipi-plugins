/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-05-16
 * Description : a tool to export GPS data to KML file.
 *
 * Copyright (C) 2006-2007 by Stephane Pontier <shadow dot walker at free dot fr>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kmlgpsdataparser.h"

// KDE includes

#include <kdebug.h>
#include <klocale.h>

namespace KIPIKMLExportPlugin
{

KMLGPSDataParser::KMLGPSDataParser()
    : GPSDataParser()
{
    kmlDocument = 0;
}

KMLGPSDataParser::~KMLGPSDataParser()
{
}

QString KMLGPSDataParser::lineString()
{
    QString line = "";
    // cache the end to not recalculate it with large number of points
    GPSDataMap::ConstIterator end (m_GPSDataMap.constEnd());

    for (GPSDataMap::ConstIterator it = m_GPSDataMap.constBegin(); it != end; ++it )
    {
        line += QString("%1,%2,%3 ").arg(it.value().longitude()).arg(it.value().latitude()).arg(it.value().altitude());
    }

    return line;
}

void KMLGPSDataParser::CreateTrackLine(QDomElement& parent, QDomDocument& root, int altitudeMode)
{
    kmlDocument = &root;

    // add the linetrack
    QDomElement kmlPlacemark  = addKmlElement(parent, "Placemark");
    addKmlTextElement(kmlPlacemark, "name", i18n("Track"));
    QDomElement kmlLineString = addKmlElement(kmlPlacemark, "LineString");
    addKmlTextElement(kmlLineString, "coordinates", lineString());
    addKmlTextElement(kmlPlacemark, "styleUrl", "#linetrack");

    if (altitudeMode == 2 )
    {
        addKmlTextElement(kmlLineString, "altitudeMode", "absolute");
    }
    else if (altitudeMode == 1 )
    {
        addKmlTextElement(kmlLineString, "altitudeMode", "relativeToGround");
    }
    else
    {
        addKmlTextElement(kmlLineString, "altitudeMode", "clampToGround");
    }
}

void KMLGPSDataParser::CreateTrackPoints(QDomElement& parent, QDomDocument& root, int timeZone, int altitudeMode)
{
    kmlDocument = &root;
    kDebug(AREA_CODE_LOADING) << "creation d'un trackpoint" ;

    // create the points
    QDomElement kmlPointsFolder = addKmlElement(parent, "Folder");
    addKmlTextElement(kmlPointsFolder, "name", i18n("Points"));
    addKmlTextElement(kmlPointsFolder, "visibility", "0");
    addKmlTextElement(kmlPointsFolder, "open", "0");
    int i = 0;

    // cache the end to not recalculate it with large number of points
    GPSDataMap::ConstIterator end (m_GPSDataMap.constEnd());

    for (GPSDataMap::ConstIterator it = m_GPSDataMap.constBegin(); it != end; ++it, ++i)
    {
        QDomElement kmlPointPlacemark = addKmlElement(kmlPointsFolder, "Placemark");
        addKmlTextElement(kmlPointPlacemark, "name", QString("%1 %2 ").arg(i18n("Point")).arg(i));
        addKmlTextElement(kmlPointPlacemark, "styleUrl", "#track");
        QDomElement kmlTimeStamp      = addKmlElement(kmlPointPlacemark, "TimeStamp");
        // GPS device are sync in time by satellite using GMT time.
        // If the camera time is different than GMT time, we want to
        // convert the GPS time to localtime of the picture to be display
        // in the same timeframe
        QDateTime GPSLocalizedTime = it.key().addSecs(timeZone*3600);

        addKmlTextElement(kmlTimeStamp, "when", GPSLocalizedTime.toString("yyyy-MM-ddThh:mm:ssZ"));
        QDomElement kmlGeometry = addKmlElement(kmlPointPlacemark, "Point");
        addKmlTextElement(kmlPointPlacemark, "visibility", "0");

        if (it.value().latitude())
        {
            addKmlTextElement(kmlGeometry, "coordinates",
                              QString("%1,%2,%3 ")
                              .arg(it.value().longitude()).arg(it.value().latitude()).arg(it.value().altitude()));
        }
        else
        {
            addKmlTextElement(kmlGeometry, "coordinates", QString("%1,%2 ").arg(it.value().longitude()).arg(it.value().latitude()));
        }
        if (altitudeMode == 2 )
        {
            addKmlTextElement(kmlGeometry, "altitudeMode", "absolute");
        }
        else if (altitudeMode == 1 )
        {
            addKmlTextElement(kmlGeometry, "altitudeMode", "relativeToGround");
        }
        else
        {
            addKmlTextElement(kmlGeometry, "altitudeMode", "clampToGround");
        }
    }
}

} // namespace KIPIKMLExportPlugin
