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

QString makeTagString(const RGInfo& info, QString inputFormat, QString backendName)
{

    kDebug()<<"Backend name:"<<backendName;
    QString returnedFormat = inputFormat;

    int indexFBracket = -1;

    while( ( indexFBracket = returnedFormat.indexOf("{") ) != -1)
    {

        int indexLBracket = returnedFormat.indexOf("}"); 

        QString humanTag = returnedFormat.mid(indexFBracket + 1, indexLBracket-indexFBracket-1);


        if(backendName.compare(QString("OSM")) == 0)
        {

            if(humanTag.compare(QString("Country")) == 0)
            {
                if(info.rgData[QString("country")].isEmpty())
                {
                    //"My Tags/{Country}" => "My Tags"
                    returnedFormat.replace(indexFBracket-1, 10, "" );
                }
                else
                {
                    returnedFormat.replace(indexFBracket, 9, info.rgData[QString("country")]);
                }
            }

            else if(humanTag.compare(QString("City")) == 0)
            {
                if(info.rgData[QString("city")].isEmpty())
                {
                    //"My Tags/{City}" => "My Tags"
                    returnedFormat.replace(indexFBracket-1, 7, "" );
                }
                else
                {
                    returnedFormat.replace(indexFBracket, 6, info.rgData[QString("city")]);
                }
            }
            else
            {
                returnedFormat.replace(indexFBracket-1, indexLBracket-indexFBracket+2, "");
            }

        }
        
        else if(backendName.compare(QString("GeonamesUS")) == 0)
        {

            if(humanTag.compare(QString("Country")) == 0)
            {
                if(info.rgData[QString("adminName1")].isEmpty())
                {
                    //"My Tags/{Country}" => "My Tags"
                    returnedFormat.replace(indexFBracket-1, 10, "" );
                }
                else
                {
                    returnedFormat.replace(indexFBracket, 9, info.rgData[QString("adminName1")]);
                }
            }

            else if(humanTag.compare(QString("City")) == 0)
            {
                if(info.rgData[QString("placeName")].isEmpty())
                {
                    //"My Tags/{City}" => "My Tags"
                    returnedFormat.replace(indexFBracket-1, 7, "" );
                }
                else
                {
                    returnedFormat.replace(indexFBracket, 6, info.rgData[QString("placeName")]);
                }
            }
            else
            {
                returnedFormat.replace(indexFBracket-1, indexLBracket-indexFBracket+2, "");
            }
        }

        else if(backendName.compare(QString("Geonames")) == 0)
        {

            if(humanTag.compare(QString("Country")) == 0)
            {
                if(info.rgData[QString("countryName")].isEmpty())
                {
                    //"My Tags/{Country}" => "My Tags"
                    returnedFormat.replace(indexFBracket-1, 10, "" );
                }
                else
                {
                    returnedFormat.replace(indexFBracket, 9, info.rgData[QString("countryName")]);
                }
            }

            else if(humanTag.compare(QString("City")) == 0)
            {
                if(info.rgData[QString("name")].isEmpty())
                {
                    //"My Tags/{City}" => "My Tags"
                    returnedFormat.replace(indexFBracket-1, 7, "" );
                }
                else
                {
                    returnedFormat.replace(indexFBracket, 6, info.rgData[QString("name")]);
                }
            }
            else
            {  
                returnedFormat.replace(indexFBracket-1, indexLBracket-indexFBracket+2, ""); 
            }
        }

    } 

    return returnedFormat;

}

}
