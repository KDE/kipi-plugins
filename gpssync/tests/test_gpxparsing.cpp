/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-01-17
 * @brief  Test parsing gpx data.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

// Qt includes

#include <QDateTime>

// KDE includes

#include <qtest_kde.h>
#include <kdebug.h>
#include <kurl.h>

// local includes

#include "test_gpxparsing.moc"
#include "../gpsdataparser.h"
#include "../gpsdataparser_p.h"

using namespace KIPIGPSSyncPlugin;

QTEST_KDEMAIN_CORE(TestGPXParsing)

/**
 * @brief Return the path of the directory containing the test data
 */
KUrl GetTestDataDirectory()
{
    // any better ideas on how to get the path?
    const KUrl thisCPPFile(__FILE__);
    KUrl testDataDir = thisCPPFile.upUrl();
    testDataDir.addPath("data/");
    return testDataDir;
}

/**
 * @brief Dummy test that does nothing
 */
void TestGPXParsing::testNoOp()
{
}

/**
 * @brief Test how well QDateTime deals with various string representations
 */
void TestGPXParsing::testQDateTimeParsing()
{
    {
        // strings ending with a 'Z' are taken to be in UTC, regardless of milliseconds
        QDateTime time1 = QDateTime::fromString("2009-03-11T13:39:55.622Z", Qt::ISODate);
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QDateTime time2 = QDateTime::fromString("2009-03-11T13:39:55Z", Qt::ISODate);
        QCOMPARE(time2.timeSpec(), Qt::UTC);
    }

    {
        // eCoach in N900: 2010-01-14T09:26:02.287+02:00
        QDateTime time1 = QDateTime::fromString("2010-01-14T09:26:02.287+02:00", Qt::ISODate);

#if QT_VERSION>=0x040700
        // Qt >= 4.7: both date and time are parsed fine
        /// @todo What about the timezone?
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(9, 26, 2, 287));
#else
        // Qt < 4.7: the date is parsed fine, but the time fails:
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(0, 0, 0));
#endif

        // when we omit the time zone data, parsing succeeds
        // time is interpreted as local time
        QDateTime time2 = QDateTime::fromString("2010-01-14T09:26:02.287"/*"+02:00"*/, Qt::ISODate);
        QCOMPARE(time2.date(), QDate(2010, 01, 14));
        QCOMPARE(time2.time(), QTime(9, 26, 2, 287));
        QCOMPARE(time2.timeSpec(), Qt::LocalTime);
    }
}

/**
 * @brief Test our custom parsing function
 */
void TestGPXParsing::testCustomDateTimeParsing()
{
    {
        // this should work as usual:
        const QDateTime time1 = GPXFileReader::ParseTime("2009-03-11T13:39:55.622Z");
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QCOMPARE(time1.date(), QDate(2009, 03, 11));
        QCOMPARE(time1.time(), QTime(13, 39, 55, 622));
    }

    {
        // eCoach in N900: 2010-01-14T09:26:02.287+02:00
        const QDateTime time1 = GPXFileReader::ParseTime("2010-01-14T09:26:02.287+02:00");
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(7, 26, 02, 287));
    }

    {
        // test negative time zone offset: 2010-01-14T09:26:02.287+02:00
        const QDateTime time1 = GPXFileReader::ParseTime("2010-01-14T09:26:02.287-02:00");
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(11, 26, 02, 287));
    }

    {
        // test negative time zone offset with minutes: 2010-01-14T09:26:02.287+03:15
        const QDateTime time1 = GPXFileReader::ParseTime("2010-01-14T09:26:02.287-03:15");
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(12, 41, 02, 287));
    }
}

/**
 * @brief Test loading of gpx files using GPSDataParser (threaded)
 */
void TestGPXParsing::testFileLoading()
{
    const KUrl testDataDir = GetTestDataDirectory();

    KUrl::List fileList;
    fileList << KUrl(testDataDir, "gpxfile-1.gpx");

    GPSDataParser myParser;

    QSignalSpy spyGPXFiles(&myParser, SIGNAL(signalGPXFilesReadyAt(int, int)));
    QSignalSpy spyAllDone(&myParser, SIGNAL(signalAllGPXFilesReady()));
    
    myParser.loadGPXFiles(fileList);

    // wait until the files are loaded:
    while (spyAllDone.isEmpty())
    {
        QTest::qWait(100);
    }
    
    QCOMPARE(spyAllDone.count(), 1);
    QCOMPARE(spyGPXFiles.count(), 1);

    const GPSDataParser::GPXFileData& file1 = myParser.fileData(0);
    QVERIFY(file1.isValid);

    // items to correlate:
    GPSDataParser::GPXCorrelation::List itemsToCorrelate;
    {
        GPSDataParser::GPXCorrelation myItem;
        myItem.dateTime = GPXFileReader::ParseTime("2009-07-26T18:00:00Z");
        itemsToCorrelate << myItem;
    }

    QSignalSpy spyItemsFinished(&myParser, SIGNAL(signalAllItemsCorrelated()));
    QVERIFY(spyItemsFinished.isValid());
    QSignalSpy spyItemsCorrelated(&myParser, SIGNAL(signalItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List&)));
    QVERIFY(spyItemsCorrelated.isValid());

    GPSDataParser::GPXCorrelationOptions correlationOptions;
    correlationOptions.maxGapTime = 0;
    myParser.correlate(itemsToCorrelate, correlationOptions);

    while (spyItemsFinished.isEmpty())
    {
        QTest::qWait(100);
    }
    QCOMPARE(spyItemsFinished.count(), 1);
    QCOMPARE(spyItemsCorrelated.count(), 1);

    KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List myCorrelatedItems = spyItemsCorrelated.first().first().value<KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List>();
    QCOMPARE(myCorrelatedItems.count(), 1);
    QCOMPARE(myCorrelatedItems.first().coordinates, KGeoMap::GeoCoordinates::fromGeoUrl("geo:18,7,0"));
    QCOMPARE(myCorrelatedItems.first().nSatellites, 3);
    QCOMPARE(myCorrelatedItems.first().hDop, 2.5);
    QCOMPARE(myCorrelatedItems.first().speed, 3.14);
}

/**
 * @brief Test interpolation
 */
void TestGPXParsing::testInterpolation()
{
    const KUrl testDataDir = GetTestDataDirectory();

    KUrl::List fileList;
    fileList << KUrl(testDataDir, "gpxfile-1.gpx");

    GPSDataParser myParser;

    QSignalSpy spyGPXFiles(&myParser, SIGNAL(signalGPXFilesReadyAt(int, int)));
    QSignalSpy spyAllDone(&myParser, SIGNAL(signalAllGPXFilesReady()));

    myParser.loadGPXFiles(fileList);

    // wait until the files are loaded:
    while (spyAllDone.isEmpty())
    {
        QTest::qWait(100);
    }

    QCOMPARE(spyAllDone.count(), 1);
    QCOMPARE(spyGPXFiles.count(), 1);

    const GPSDataParser::GPXFileData& file1 = myParser.fileData(0);
    QVERIFY(file1.isValid);

    // items to correlate:
    GPSDataParser::GPXCorrelation::List itemsToCorrelate;
    {
        GPSDataParser::GPXCorrelation myItem;
        myItem.dateTime = GPXFileReader::ParseTime("2009-11-29T17:00:30Z");
        itemsToCorrelate << myItem;
    }

    QSignalSpy spyItemsFinished(&myParser, SIGNAL(signalAllItemsCorrelated()));
    QVERIFY(spyItemsFinished.isValid());
    QSignalSpy spyItemsCorrelated(&myParser, SIGNAL(signalItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List&)));
    QVERIFY(spyItemsCorrelated.isValid());

    GPSDataParser::GPXCorrelationOptions correlationOptions;
    correlationOptions.maxGapTime = 0;
    correlationOptions.interpolate = true;
    correlationOptions.interpolationDstTime = 31;
    myParser.correlate(itemsToCorrelate, correlationOptions);

    while (spyItemsFinished.isEmpty())
    {
        QTest::qWait(100);
    }
    QCOMPARE(spyItemsFinished.count(), 1);
    QCOMPARE(spyItemsCorrelated.count(), 1);

    KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List myCorrelatedItems = spyItemsCorrelated.first().first().value<KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List>();
    QCOMPARE(myCorrelatedItems.count(), 1);
    QCOMPARE(myCorrelatedItems.first().coordinates, KGeoMap::GeoCoordinates::fromGeoUrl("geo:17.5,0.5,3"));
    QCOMPARE(myCorrelatedItems.first().nSatellites, -1);
    QCOMPARE(myCorrelatedItems.first().hDop, -1.0);
}

/**
 * @brief Test loading of a GPX file directly
 */
void TestGPXParsing::testSaxLoader()
{
    const KUrl testDataDir = GetTestDataDirectory();

    GPSDataParser::GPXFileData fileData = GPXFileReader::loadGPXFile(KUrl(testDataDir, "gpxfile-1.gpx"));
    QVERIFY(fileData.isValid);
    QVERIFY(fileData.loadError.isEmpty());

    // verify that the points are sorted by date:
    for (int i = 1; i<fileData.gpxDataPoints.count(); ++i)
    {
        QVERIFY(GPSDataParser::GPXDataPoint::EarlierThan(fileData.gpxDataPoints.at(i-1), fileData.gpxDataPoints.at(i)));
    }
}

/**
 * @brief Test loading of invalid GPX files
 */
void TestGPXParsing::testSaxLoaderError()
{
    const KUrl testDataDir = GetTestDataDirectory();

    {
        GPSDataParser::GPXFileData fileData = GPXFileReader::loadGPXFile(KUrl(testDataDir, "gpx-invalid-empty.gpx"));
        QVERIFY(!fileData.isValid);
        QVERIFY(!fileData.loadError.isEmpty());
        kDebug()<<fileData.loadError;
    }

    {
        GPSDataParser::GPXFileData fileData = GPXFileReader::loadGPXFile(KUrl(testDataDir, "gpx-invalid-xml-error.gpx"));
        QVERIFY(!fileData.isValid);
        QVERIFY(!fileData.loadError.isEmpty());
        kDebug()<<fileData.loadError;
    }

    {
        GPSDataParser::GPXFileData fileData = GPXFileReader::loadGPXFile(KUrl(testDataDir, "gpx-invalid-no-points.gpx"));
        QVERIFY(!fileData.isValid);
        QVERIFY(!fileData.loadError.isEmpty());
        kDebug()<<fileData.loadError;
    }
}
