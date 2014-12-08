/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-05-12
 * @brief  Test parsing rg data.
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

#include "test_rgparsing.moc"

// Qt includes

#include <QDateTime>

// KDE includes

#include <qtest_kde.h>
#include <kdebug.h>
#include <kurl.h>

// local includes

#include "../backend-rg.h"
#include "../parseTagString.h"

using namespace KIPIGPSSyncPlugin;

QTEST_KDEMAIN_CORE(TestRGParsing)


/**
 * @brief Dummy test that does nothing
 */
void TestRGParsing::testNoOp()
{
}

void TestRGParsing::testVerifyRG()
{
    {
        //testing tag parsing
        RGInfo info;
        QString country = QString::fromUtf8("France");
        QString city = QString::fromUtf8("Paris");

        const QString howToFormat = "My Tags/{Country}/{City}";  

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;  

        QCOMPARE( makeTagString(info, howToFormat, QString("OSM")).last(), QString("My Tags/France/Paris"));
    }

    {
        //german special characters
        RGInfo info;
        QString country = QString::fromUtf8("Germany");
        QString city = QString::fromUtf8("Düsseldorf");

        const QString howToFormat = "My Tags/{Country}/{City}";  

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;  

        QCOMPARE( makeTagString(info, howToFormat, QString("OSM")).last(), QString::fromUtf8("My Tags/Germany/Düsseldorf") );
    } 

    {
        //chinese special characters
        RGInfo info;
        QString country = QString::fromUtf8("中国");
        QString city = QString::fromUtf8("北京市");

        const QString howToFormat = "My Tags/{Country}/{City}";  

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;  

        QCOMPARE( makeTagString(info, howToFormat, QString("OSM")).last(), QString::fromUtf8("My Tags/中国/北京市") );
    }

    {
        //romanian special characters
        RGInfo info;
        QString country = QString::fromUtf8("România");
        QString city = QString::fromUtf8("București");

        const QString howToFormat = "My Tags/{Country}/{City}";

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;

        QCOMPARE( makeTagString(info, howToFormat, QString("OSM")).last(), QString::fromUtf8("My Tags/România/București"));
    }

    {
        //No {Country}
        RGInfo info;
        QString country = QString::fromUtf8("România");
        QString city = QString::fromUtf8("București");
    
        const QString howToFormat = "My Tags/{City}";

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;

        QCOMPARE( makeTagString(info, howToFormat, QString("OSM")).last(), QString::fromUtf8("My Tags/București"));
    }
    
    {
        //No {City}
        RGInfo info;
        QString country = QString::fromUtf8("România");
        QString city = QString::fromUtf8("");
    
        const QString howToFormat = "My Tags/{Country}/{City}";

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;

        QCOMPARE( makeTagString(info, howToFormat, QString("OSM")).last(), QString::fromUtf8("My Tags/România"));
    }

    {
        //{Ctonury} instead of {Country} -> parsing is aborted
        RGInfo info;
        QString country = QString::fromUtf8("România");
        QString city = QString::fromUtf8("București");

        const QString howToFormat = "My Tags/{Ctonury}/{City}";

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;
        QCOMPARE( makeTagString(info, howToFormat, QString("OSM")).last(), QString::fromUtf8("My Tags/"));
    }

    {
        //no retrieved data
        RGInfo info;
        QString country = QString::fromUtf8("");
        QString city = QString::fromUtf8("");

        const QString howToFormat = "My Tags/{Country}/{City}";

        info.rgData[QString("country")] = country;
        info.rgData[QString("city")] = city;

        QCOMPARE( makeTagString(info, howToFormat, QString("OSM")).last(), QString::fromUtf8("My Tags"));
    }




    // TODO:
    // - some fields are missing (no country)
    // * simply deletes {Country}
    // - invalid field names
    // * if we have {Contry} or {bla} it deletes the etiquette
    // - how do we handle these cases in the UI?
    // * now that we have makeTagString separate, these are handled same in tests and UI
}
