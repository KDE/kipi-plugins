/* ============================================================
 *
 * Date        : 2010-01-17
 * Description : test parsing gpx data
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

// Qt includes

#include <QDateTime>
// KDE includes

#include <qtest_kde.h>
#include <kdebug.h>
#include <kurl.h>

// local includes

#include "test_rgparsing.moc"
//#include "../gpsdataparser.h"
//#include "../gpsdataparser_p.h"
#include "../gpsreversegeocodingwidget.h"


using namespace KIPIGPSSyncPlugin;

QTEST_KDEMAIN_CORE(TestRGParsing)


/**
 * @brief Dummy test that does nothing
 */
void TestRGParsing::testNoOp()
{
}

QString MakeTagString(const QString& howToFormat, const RGInfo& info)
{

    QString returnedFormat = howToFormat;

    int countryIndex = returnedFormat.indexOf("{Country}");
    if(countryIndex != -1)
    {

        returnedFormat.replace(countryIndex, strlen("{Country}"), info.rgData[QString("country")]);

    }

    int cityIndex = returnedFormat.indexOf("{City}");
    if(cityIndex != -1)
    {

        returnedFormat.replace(cityIndex, strlen("{City}"), info.rgData[QString("city")]);

    }

    kDebug()<<returnedFormat;


    return returnedFormat;

}

void TestRGParsing::testVerifyRG()
{
    {
        RGInfo info;
        QString country = QString::fromUtf8("France");
        QString city = QString::fromUtf8("Paris");

        const QString howToFormat = "My Tags/{Country}/{City}";  

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;  

        QCOMPARE( MakeTagString(howToFormat, info), QString("My Tags/France/Paris"));
    }

    {
        RGInfo info;
        QString country = QString::fromUtf8("Germany");
        QString city = QString::fromUtf8("Düsseldorf");

        const QString howToFormat = "My Tags/{Country}/{City}";  

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;  

        QCOMPARE( MakeTagString(howToFormat, info), QString("My Tags/"+QString::fromUtf8("Germany")+"/"+QString::fromUtf8("Düsseldorf")));
    } 

    {
        RGInfo info;
        QString country = QString::fromUtf8("中国");
        QString city = QString::fromUtf8("北京市");

        const QString howToFormat = "My Tags/{Country}/{City}";  

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;  

        QCOMPARE( MakeTagString(howToFormat, info), QString("My Tags/"+QString::fromUtf8("中国")+"/"+QString::fromUtf8("北京市")));
    }
    
}
