/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-05-12
 * @brief  Parses retrieved data into tag string.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 * @author Copyright (C) 2010 by Gabriel Voicu
 *         <a href="mailto:ping dot gabi at gmail dot com">ping dot gabi at gmail dot com</a>
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

#ifndef PARSETAGSTRING_H
#define PARSETAGSTRING_H

// KDE includes

#include <kdebug.h>

// local includes

#include "backend-rg.h"

namespace KIPIGPSSyncPlugin
{

QStringList makeTagString(const RGInfo& info,const QString& inputFormat,const QString& backendName)
{
    QString auxReturnedFormat = inputFormat;
    QString returnedAddress   = inputFormat;
    QString returnedFormat;

    QStringList returnedAddressElements;

    int indexFBracket = -1;

    while ( ( indexFBracket = returnedAddress.indexOf("{") ) >= 0 )
    {
        int indexLBracket       = returnedAddress.indexOf("}"); 
        QString humanTag        = returnedAddress.mid(indexFBracket + 1, indexLBracket-indexFBracket-1);
        int indexFormatFBracket = auxReturnedFormat.indexOf("{");
        auxReturnedFormat.replace(indexFormatFBracket-1, humanTag.length()+3, "");
        bool dataAdded          = false;
        QString result;

        if (backendName == QString("OSM"))
        {
            if (humanTag == QString("Country"))
            {
                if (!info.rgData[QString("country")].isEmpty())
                {
                    result    = info.rgData[QString("country")];
                    returnedFormat.append("/{Country}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("State district"))
            {
                if (!info.rgData[QString("state_district")].isEmpty())
                {
                    result    = info.rgData[QString("state_district")];
                    returnedFormat.append("/{State district}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("County"))
            {
                if (!info.rgData[QString("county")].isEmpty())
                {
                    result    = info.rgData[QString("county")];
                    returnedFormat.append("/{County}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("City"))
            {
                if (!info.rgData[QString("city")].isEmpty())
                {
                    result    = info.rgData[QString("city")];
                    returnedFormat.append("/{City}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("City district"))
            {
                if (!info.rgData[QString("city_district")].isEmpty())
                {
                    result    = info.rgData[QString("city_district")];
                    returnedFormat.append("/{City district}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("Suburb"))
            {
                if (!info.rgData[QString("suburb")].isEmpty())
                {
                    result    = info.rgData[QString("suburb")];
                    returnedFormat.append("/{Suburb}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("Street"))
            {
                if (!info.rgData[QString("road")].isEmpty())
                {
                    result    = info.rgData[QString("road")];
                    returnedFormat.append("/{Street}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("State"))
            {
                if (!info.rgData[QString("state")].isEmpty())
                {
                    result    = info.rgData[QString("state")];
                    returnedFormat.append("/{State}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("Town"))
            {
                if (!info.rgData[QString("town")].isEmpty())
                {
                    result    = info.rgData[QString("town")];
                    returnedFormat.append("/{Town}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("Village"))
            {
                if (!info.rgData[QString("village")].isEmpty())
                {
                    result    = info.rgData[QString("village")];
                    returnedFormat.append("/{Village}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("Hamlet"))
            {
                if (!info.rgData[QString("hamlet")].isEmpty())
                {
                    result    = info.rgData[QString("hamlet")];
                    returnedFormat.append("/{Hamlet}");
                    dataAdded = true;
                }
            }
            else if (humanTag == QString("House number"))
            {
                if (!info.rgData[QString("house_number")].isEmpty())
                {
                    result    = info.rgData[QString("house_number")];
                    returnedFormat.append("/{House number}");
                    dataAdded = true;
                }
            }
            else
            {
                returnedAddress.replace(indexFBracket-1, indexLBracket-indexFBracket+2, "");

                int indexFormatFBracket = auxReturnedFormat.indexOf("{");
                int indexFormatLBracket = auxReturnedFormat.indexOf("}");
                auxReturnedFormat.replace(indexFormatFBracket-1,indexFormatLBracket-indexFormatFBracket+2,"");
                dataAdded               = true;
            }
        }

        else if (backendName == QString("GeonamesUS"))
        {

            if (humanTag.compare(QString("LAU2")) == 0)
            {
                if (!info.rgData[QString("adminName2")].isEmpty())
                {
                    result    = info.rgData[QString("adminName2")];
                    returnedFormat.append("/{LAU2}");
                    dataAdded = true;
                }
            }

           else if (humanTag == QString("LAU1"))
            {
                if (!info.rgData[QString("adminName1")].isEmpty())
                {
                    result    = info.rgData[QString("adminName1")];
                    returnedFormat.append("/{LAU1}");
                    dataAdded = true;
                }
            }

            else if (humanTag == QString("City"))
            {
                if (!info.rgData[QString("placeName")].isEmpty())
                {
                    result    = info.rgData[QString("placeName")];
                    returnedFormat.append("/{City}");
                    dataAdded = true;
                }
            }
            else
            {
                returnedAddress.replace(indexFBracket-1, indexLBracket-indexFBracket+2, "");

                int indexFormatFBracket = auxReturnedFormat.indexOf("{");
                int indexFormatLBracket = auxReturnedFormat.indexOf("}");
                auxReturnedFormat.replace(indexFormatFBracket-1,indexFormatLBracket-indexFormatFBracket+2,"");
                dataAdded               = true;
            }
        }

        else if (backendName == QString("Geonames"))
        {
            if (humanTag.compare(QString("Country")) == 0)
            {
                if (!info.rgData[QString("countryName")].isEmpty())
                {
                    result    = info.rgData[QString("countryName")];
                    returnedFormat.append("/{Country}");
                    dataAdded = true;
                }
            }

            else if (humanTag == QString("Place"))
            {
                if (!info.rgData[QString("name")].isEmpty())
                {
                    result    = info.rgData[QString("name")];
                    returnedFormat.append("/{Place}");
                    dataAdded = true;
                }
            }
            else
            {
                returnedAddress.replace(indexFBracket-1, indexLBracket-indexFBracket+2, ""); 

                int indexFormatFBracket = auxReturnedFormat.indexOf("{");
                int indexFormatLBracket = auxReturnedFormat.indexOf("}");
                auxReturnedFormat.replace(indexFormatFBracket-1,indexFormatLBracket-indexFormatFBracket+2,"");
                dataAdded               = true;
            }
        }

        if (!dataAdded)
        {
            returnedAddress.replace(indexFBracket-1, humanTag.length()+3, "");
        }
        else
        {
            returnedAddress.replace(indexFBracket, humanTag.length()+2, result);
        }
    }

    returnedAddressElements.append(returnedFormat);
    returnedAddressElements.append(returnedAddress);

    return returnedAddressElements;
}

} // KIPIGPSSyncPlugin

#endif /* PARSETAGSTRING_H */
