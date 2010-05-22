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

#include "test_gpxparsing.moc"
#include "../gpsdataparser.h"

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
        // the date is parsed fine, but the time fails:
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(0, 0, 0));

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
void TestGPXParsing::testCustomParsing()
{
    {
        // this should work as usual:
        const QDateTime time1 = GPSDataParser::ParseTime("2009-03-11T13:39:55.622Z");
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QCOMPARE(time1.date(), QDate(2009, 03, 11));
        QCOMPARE(time1.time(), QTime(13, 39, 55, 622));
    }

    {
        // eCoach in N900: 2010-01-14T09:26:02.287+02:00
        const QDateTime time1 = GPSDataParser::ParseTime("2010-01-14T09:26:02.287+02:00");
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(7, 26, 02, 287));
    }

    {
        // test negative time zone offset: 2010-01-14T09:26:02.287+02:00
        const QDateTime time1 = GPSDataParser::ParseTime("2010-01-14T09:26:02.287-02:00");
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(11, 26, 02, 287));
    }

    {
        // test negative time zone offset with minutes: 2010-01-14T09:26:02.287+03:15
        const QDateTime time1 = GPSDataParser::ParseTime("2010-01-14T09:26:02.287-03:15");
        QCOMPARE(time1.timeSpec(), Qt::UTC);
        QCOMPARE(time1.date(), QDate(2010, 01, 14));
        QCOMPARE(time1.time(), QTime(12, 41, 02, 287));
    }
}

/**
 * @brief Test loading of gpx files via GPSDataParser::LoadGPXFile
 */
void TestGPXParsing::testFileLoadingFunction()
{
    KUrl testDataDir = GetTestDataDirectory();

    const KUrl gpxFileUrl = KUrl(testDataDir, "gpxfile-1.gpx");

    GPSDataParser::GPXFileData myFileData = GPSDataParser::LoadGPXFile(gpxFileUrl);
    QVERIFY(myFileData.isValid);

    // verify that the points are sorted by date:
    for (int i = 1; i<myFileData.gpxDataPoints.count(); ++i)
    {
        QVERIFY(GPSDataParser::GPXDataPoint::EarlierThan(myFileData.gpxDataPoints.at(i-1), myFileData.gpxDataPoints.at(i)));
    }
}

/**
 * @brief Test loading of gpx files using GPSDataParser (threaded)
 */
void TestGPXParsing::testFileLoading()
{
    KUrl testDataDir = GetTestDataDirectory();

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
        myItem.dateTime = GPSDataParser::ParseTime("2009-07-26T18:00:00Z");
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
    QCOMPARE(myCorrelatedItems.first().coordinates, WMW2::WMWGeoCoordinate::fromGeoUrl("geo:18,7,0"));
}

/**
 * @brief Test interpolation
 */
void TestGPXParsing::testInterpolation()
{
    KUrl testDataDir = GetTestDataDirectory();

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
        myItem.dateTime = GPSDataParser::ParseTime("2009-11-29T17:00:30Z");
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
    QCOMPARE(myCorrelatedItems.first().coordinates, WMW2::WMWGeoCoordinate::fromGeoUrl("geo:17.5,0.5,3"));
}