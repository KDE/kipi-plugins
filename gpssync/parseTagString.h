/* ============================================================
 *
 * Date        : 2010-05-12
 * Description : Parses retrieved data into tag string
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "backend-rg.h"
#include "kdebug.h"

namespace KIPIGPSSyncPlugin
{

QStringList makeTagString(const RGInfo& info, QString inputFormat, QString backendName)
{

    QString auxReturnedFormat = inputFormat;
    QString returnedAddress = inputFormat;
    QString returnedFormat;        
    

    QStringList returnedAddressElements;   

    int indexFBracket = -1;

    while( ( indexFBracket = returnedAddress.indexOf("{") ) >= 0 )
    {

        int indexLBracket = returnedAddress.indexOf("}"); 

        QString humanTag = returnedAddress.mid(indexFBracket + 1, indexLBracket-indexFBracket-1);

        int indexFormatFBracket = auxReturnedFormat.indexOf("{");
        auxReturnedFormat.replace(indexFormatFBracket-1, humanTag.length()+3, "");
        bool dataAdded = false;      

        kDebug()<<returnedAddress; 
 
        if(backendName.compare(QString("OSM")) == 0)
        {

            if(humanTag.compare(QString("Country")) == 0)
            {
                if(!info.rgData[QString("country")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 9, info.rgData[QString("country")]);
                    returnedFormat.append("/{Country}");
                    dataAdded = true;
                }
            }
            else if(humanTag.compare(QString("County")) == 0)
            {
                if(!info.rgData[QString("county")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 8, info.rgData[QString("county")]);
                    returnedFormat.append("/{County}");
                    dataAdded = true;
                }
            }    
            else if(humanTag.compare(QString("City")) == 0)
            {
                if(!info.rgData[QString("city")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 6, info.rgData[QString("city")]);
                    returnedFormat.append("/{City}");
                    dataAdded = true;
                }
            }
            else if(humanTag.compare(QString("Street")) == 0)
            {
                if(!info.rgData[QString("road")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 8, info.rgData[QString("road")]);
                    returnedFormat.append("/{Street}");
                    dataAdded = true;
                }
            }
            else if(humanTag.compare(QString("State")) == 0)
            {
                if(!info.rgData[QString("state")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 7, info.rgData[QString("state")]);
                    returnedFormat.append("/{State}");
                    dataAdded = true;
                }

            }
            else if(humanTag.compare(QString("Town")) == 0)
            {
                if(!info.rgData[QString("town")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 6, info.rgData[QString("town")]);
                    returnedFormat.append("/{Town}");
                    dataAdded = true;
                }

            }
            else if(humanTag.compare(QString("Village")) == 0)
            {
                if(!info.rgData[QString("village")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 9, info.rgData[QString("village")]);
                    returnedFormat.append("/{Village}");
                    dataAdded = true;
                }

            }
            else if(humanTag.compare(QString("Hamlet")) == 0)
            {
                if(!info.rgData[QString("hamlet")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 8, info.rgData[QString("hamlet")]);
                    returnedFormat.append("/{Hamlet}");
                    dataAdded = true;
                }

            }
            else
            {
                returnedAddress.replace(indexFBracket-1, indexLBracket-indexFBracket+2, "");

                int indexFormatFBracket = auxReturnedFormat.indexOf("{");
                int indexFormatLBracket = auxReturnedFormat.indexOf("}");
                auxReturnedFormat.replace(indexFormatFBracket-1,indexFormatLBracket-indexFormatFBracket+2,"");
                dataAdded = true;
            }

        }
        
        else if(backendName.compare(QString("GeonamesUS")) == 0)
        {

            if(humanTag.compare(QString("LAU2")) == 0)
            {
                if(!info.rgData[QString("adminName2")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 6, info.rgData[QString("adminName2")]);
                    returnedFormat.append("/{LAU2}");
                    dataAdded = true;
                }
            }

           else if(humanTag.compare(QString("LAU1")) == 0)
            {
                if(!info.rgData[QString("adminName1")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 6, info.rgData[QString("adminName1")]);
                    returnedFormat.append("/{LAU1}");
                    dataAdded = true;
                }
            }

            else if(humanTag.compare(QString("City")) == 0)
            {
                if(!info.rgData[QString("placeName")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 6, info.rgData[QString("placeName")]);
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
                dataAdded = true;
            }
        }

        else if(backendName.compare(QString("Geonames")) == 0)
        {
            if(humanTag.compare(QString("Country")) == 0)
            {
                if(!info.rgData[QString("countryName")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 9, info.rgData[QString("countryName")]);
                    returnedFormat.append("/{Country}");
                    dataAdded = true;
                }
            }

            else if(humanTag.compare(QString("Place")) == 0)
            {
                if(!info.rgData[QString("name")].isEmpty())
                {
                    returnedAddress.replace(indexFBracket, 6, info.rgData[QString("name")]);
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
                dataAdded = true;
            }
        }
    
        if(!dataAdded)
        {
            returnedAddress.replace(indexFBracket-1, humanTag.length()+3, "");
        }

    } 

    returnedAddressElements.append(returnedFormat);
    returnedAddressElements.append(returnedAddress);

    return returnedAddressElements;

}

}
