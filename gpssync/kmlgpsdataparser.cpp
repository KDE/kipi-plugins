/***************************************************************************
 *   Copyright (C) 2006 by Stéphane Pontier                                *
 *   shadow.walker@free.fr                                                 *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// KDE includes.

#include <kdebug.h>

// Local includes.

#include "kmlgpsdataparser.h"

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
    for (GPSDataMap::Iterator it = m_GPSDataMap.begin();
         it != m_GPSDataMap.end(); ++it )
    {
        line += QString("%1,%2,%3 ").arg(it.data().longitude()).arg(it.data().latitude()).arg(it.data().altitude());
    }
    return line;
}

}


/*!
\fn void KIPIGPSSyncPlugin::KMLGPSDataParser::CreateTrack(QDomElement &parent, QDomDocument &root, int timeZone, int altitudeMode)
 */
void KIPIGPSSyncPlugin::KMLGPSDataParser::CreateTrack(QDomElement &parent, QDomDocument &root, int timeZone, int altitudeMode)
{
    kmlDocument = &root;
    kdDebug( 51001 ) << "creation d'un trackpoint" << endl;

    // create a folder that will contain tracks and points

    QDomElement kmlFolder = addKmlElement(parent, "Folder");
    addKmlTextElement(kmlFolder, "name", i18n("Tracks"));

    // add the linetrack
    QDomElement kmlPlacemark = addKmlElement(kmlFolder, "Placemark");
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

    // create the points
    QDomElement kmlPointsFolder = addKmlElement(kmlFolder, "Folder");
    addKmlTextElement(kmlPointsFolder, "name", i18n("Points"));
    addKmlTextElement(kmlPointsFolder, "visibility", "0");
    addKmlTextElement(kmlPointsFolder, "open", "0");
    int i = 0;
    for (GPSDataMap::Iterator it = m_GPSDataMap.begin();
         it != m_GPSDataMap.end(); ++it )
    {
        QDomElement kmlPointPlacemark = addKmlElement(kmlPointsFolder, "Placemark");
        addKmlTextElement(kmlPointPlacemark, "name", QString("%1 %2 ").arg(i18n("Point")).arg(i));
        addKmlTextElement(kmlPointPlacemark, "styleUrl", "#track");
        QDomElement kmlTimeStamp = addKmlElement(kmlPointPlacemark, "TimeStamp");
        // GPS device are sync in time by satelite using GMT time.
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
        i++;
    }

}
