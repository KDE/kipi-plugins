/***************************************************************************
 *   Copyright (C) 2006-2007 by Stéphane Pontier                           *
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

#ifndef KIPIGPSSYNCPLUGINKMLGPSDATAPARSER_H
#define KIPIGPSSYNCPLUGINKMLGPSDATAPARSER_H

// Qt includes.

#include <qdom.h>

// KDE includes.

#include <klocale.h>

// Local includes.

#include "gpsdataparser.h"


namespace KIPIGPSSyncPlugin {

/*! a classe derivated from GPSDataParser mainly to transform GPS data to KML
 *  @author Stéphane Pontier shadow.walker@free.fr
 */
class KMLGPSDataParser : public GPSDataParser
{
public:
    KMLGPSDataParser();

    ~KMLGPSDataParser();

    /*! KIPIGPSSyncPlugin::KMLGPSDataParser::lineString()
     *  @return the string containing the time ordered point (lon,lat,alt)
     */
    QString lineString();
    /*! Create a KML Element that will contain the points and the linetrace of the GPS
     *  @param parent the QDomElement to which the track will be added
     *  @param root the QDomDocument used to create all elements
     *  @param timeZone the Timezone of the pictures
     *  @param altitudeMode altitude mode of the line and points
     */
    void CreateTrack(QDomElement &parent, QDomDocument &root, int timeZone, int altitudeMode);

private:
    /*! @todo maybe initialize it in the constructor */
    /*! the root document, used to create all QDomElements */
    QDomDocument *kmlDocument;

    /*!
    \fn QDomElement KIPIGPSSyncPlugin::KMLGPSDataParser::addKmlElement(QDomElement target, QString tag)
     *  Add a new element
     *  @param target the parent element to which add the element
     *  @param tag the new element name
     *  @return the New element
     */
    QDomElement addKmlElement(QDomElement &target, QString tag)
    {
        QDomElement kmlElement = kmlDocument->createElement( tag );
        target.appendChild( kmlElement );
        return kmlElement;
    }

    /*!
    \fn QDomElement KIPIGPSSyncPlugin::KMLGPSDataParser::addKmlTextElement(QDomElement target, QString tag, QString text)
     *  Add a new element with a text
     *  @param target the parent element to which add the element
     *  @param tag the new element name
     *  @param text the text content of the new element
     *  @return the New element
     */
    QDomElement addKmlTextElement(QDomElement &target, QString tag, QString text)
    {
        QDomElement kmlElement = kmlDocument->createElement( tag );
        target.appendChild( kmlElement );
        QDomText kmlTextElement = kmlDocument->createTextNode( text );
        kmlElement.appendChild( kmlTextElement );
        return kmlElement;
    }
};

}

#endif
