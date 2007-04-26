/***************************************************************************
 *   Copyright (C) 2006-2007 by Stéphane Pontier <shadow.walker@free.fr>   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

// Local includes.

#include "kmlgpsdataparser.h"

// KDE includes.

#include <kdebug.h>
#include <klocale.h>

namespace KIPIGPSSyncPlugin {

KMLGPSDataParser::KMLGPSDataParser()
 : GPSDataParser()
{
}


KMLGPSDataParser::~KMLGPSDataParser()
{
}

QString KMLGPSDataParser::lineString() {
    QString line = "";
    // cache the end to not recalculate it with large number of points
    GPSDataMap::ConstIterator end (m_GPSDataMap.constEnd());
    for (GPSDataMap::ConstIterator it = m_GPSDataMap.constBegin();
         it != end; ++it )
    {
        line += QString("%1,%2,%3 ").arg(it.data().longitude()).arg(it.data().latitude()).arg(it.data().altitude());
    }
    return line;
}

}

/*!
\fn void KIPIGPSSyncPlugin::KMLGPSDataParser::CreateTrackLine(QDomElement &parent, QDomDocument &root, int altitudeMode)
 */
void KIPIGPSSyncPlugin::KMLGPSDataParser::CreateTrackLine(QDomElement &parent, QDomDocument &root, int altitudeMode)
{
    kmlDocument = &root;

    // add the linetrack
    QDomElement kmlPlacemark = addKmlElement(parent, "Placemark");
    addKmlTextElement(kmlPlacemark, "name", i18n("Track"));
    QDomElement kmlLineString = addKmlElement(kmlPlacemark, "LineString");
    addKmlTextElement(kmlLineString, "coordinates", lineString());
    addKmlTextElement(kmlPlacemark, "styleUrl", "#linetrack");
    if (altitudeMode == 2 ) {
        addKmlTextElement(kmlLineString, "altitudeMode", "absolute");
    } else if (altitudeMode == 1 ) {
        addKmlTextElement(kmlLineString, "altitudeMode", "relativeToGround");
    } else {
        addKmlTextElement(kmlLineString, "altitudeMode", "clampToGround");
    }
}
/*!
\fn void KIPIGPSSyncPlugin::KMLGPSDataParser::CreateTrackPoints(QDomElement &parent, QDomDocument &root, int timeZone, int altitudeMode)
 */
void KIPIGPSSyncPlugin::KMLGPSDataParser::CreateTrackPoints(QDomElement &parent, QDomDocument &root, int timeZone, int altitudeMode)
{
    kmlDocument = &root;
    kdDebug( 51001 ) << "creation d'un trackpoint" << endl;

    // create the points
    QDomElement kmlPointsFolder = addKmlElement(parent, "Folder");
    addKmlTextElement(kmlPointsFolder, "name", i18n("Points"));
    addKmlTextElement(kmlPointsFolder, "visibility", "0");
    addKmlTextElement(kmlPointsFolder, "open", "0");
    int i = 0;
    // cache the end to not recalculate it with large number of points
    GPSDataMap::ConstIterator end (m_GPSDataMap.constEnd());
    for (GPSDataMap::ConstIterator it = m_GPSDataMap.constBegin();
         it != end; ++it, i++)
    {
        QDomElement kmlPointPlacemark = addKmlElement(kmlPointsFolder, "Placemark");
        addKmlTextElement(kmlPointPlacemark, "name", QString("%1 %2 ").arg(i18n("Point")).arg(i));
        addKmlTextElement(kmlPointPlacemark, "styleUrl", "#track");
        QDomElement kmlTimeStamp = addKmlElement(kmlPointPlacemark, "TimeStamp");
        // GPS device are sync in time by satellite using GMT time.
        // If the camera time is different than GMT time, we want to
        // convert the GPS time to localtime of the picture to be display
        // in the same timeframe
        QDateTime GPSLocalizedTime = it.key().addSecs(timeZone*3600);

        addKmlTextElement(kmlTimeStamp, "when", GPSLocalizedTime.toString("yyyy-MM-ddThh:mm:ssZ"));
        QDomElement kmlGeometry = addKmlElement(kmlPointPlacemark, "Point");
        addKmlTextElement(kmlPointPlacemark, "visibility", "0");
        if (it.data().latitude()) {
            addKmlTextElement(kmlGeometry, "coordinates",
                              QString("%1,%2,%3").arg(it.data().longitude()).arg(it.data().latitude()).arg(it.data().altitude()));
        } else {
            addKmlTextElement(kmlGeometry, "coordinates", QString("%1,%2").arg(it.data().longitude()).arg(it.data().latitude()));
        }
        if (altitudeMode == 2 ) {
            addKmlTextElement(kmlGeometry, "altitudeMode", "absolute");
        } else if (altitudeMode == 1 ) {
            addKmlTextElement(kmlGeometry, "altitudeMode", "relativeToGround");
        } else {
            addKmlTextElement(kmlGeometry, "altitudeMode", "clampToGround");
        }
    }

}
