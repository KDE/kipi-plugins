/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-01-17
 * @brief  Test parsing gpx data.
 *
 * @author Copyright (C) 2010, 2014 by Michael G. Hansen
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

#include "test_correlator.moc"

// Qt includes

#include <QDateTime>

// KDE includes

#include <qtest_kde.h>
#include <kdebug.h>
#include <kurl.h>

// libkgeomap includes

#include <libkgeomap/track_reader.h>

// local includes

#include "../track_correlator.h"

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
 * @brief Test correlator
 */
void TestGPXParsing::testCorrelator1()
{
    const KUrl testDataDir = GetTestDataDirectory();

    KUrl::List fileList;
    fileList << KUrl(testDataDir, "gpxfile-1.gpx");

    KGeoMap::TrackManager myParser;

    QSignalSpy spyTrackFiles(&myParser, SIGNAL(signalTracksChanged(const QList<TrackManager::TrackChanges>)));
    QSignalSpy spyAllDone(&myParser, SIGNAL(signalAllTrackFilesReady()));
    
    myParser.loadTrackFiles(fileList);

    // wait until the files are loaded:
    while (spyAllDone.isEmpty())
    {
        QTest::qWait(100);
    }
    
    QCOMPARE(spyAllDone.count(), 1);
    QCOMPARE(spyTrackFiles.count(), 1);

    const KGeoMap::TrackManager::Track& file1 = myParser.getTrack(0);
    QVERIFY(!file1.points.isEmpty());

    // items to correlate:
    TrackCorrelator::Correlation::List itemsToCorrelate;
    {
        TrackCorrelator::Correlation myItem;
        myItem.dateTime = KGeoMap::TrackReader::ParseTime("2009-07-26T18:00:00Z");
        itemsToCorrelate << myItem;
    }

    TrackCorrelator correlator(&myParser);
    QSignalSpy spyItemsFinished(&correlator, SIGNAL(signalAllItemsCorrelated()));
    QVERIFY(spyItemsFinished.isValid());
    QSignalSpy spyItemsCorrelated(&correlator, SIGNAL(signalItemsCorrelated(KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List)));
    QVERIFY(spyItemsCorrelated.isValid());

    TrackCorrelator::CorrelationOptions correlationOptions;
    correlationOptions.maxGapTime = 0;
    correlator.correlate(itemsToCorrelate, correlationOptions);

    while (spyItemsFinished.isEmpty())
    {
        QTest::qWait(100);
    }
    QCOMPARE(spyItemsFinished.count(), 1);
    QCOMPARE(spyItemsCorrelated.count(), 1);

    KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List myCorrelatedItems = spyItemsCorrelated.first().first().value<KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List>();
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

    KGeoMap::TrackManager myParser;

    QSignalSpy spyTrackFiles(&myParser, SIGNAL(signalTracksChanged(const QList<TrackManager::TrackChanges>)));
    QSignalSpy spyAllDone(&myParser, SIGNAL(signalAllTrackFilesReady()));

    myParser.loadTrackFiles(fileList);

    // wait until the files are loaded:
    while (spyAllDone.isEmpty())
    {
        QTest::qWait(100);
    }

    QCOMPARE(spyAllDone.count(), 1);
    QCOMPARE(spyTrackFiles.count(), 1);

    const KGeoMap::TrackManager::Track& file1 = myParser.getTrack(0);
    QVERIFY(!file1.points.isEmpty());

    // items to correlate:
    TrackCorrelator::Correlation::List itemsToCorrelate;
    {
        TrackCorrelator::Correlation myItem;
        myItem.dateTime = KGeoMap::TrackReader::ParseTime("2009-11-29T17:00:30Z");
        itemsToCorrelate << myItem;
    }

    TrackCorrelator correlator(&myParser);

    QSignalSpy spyItemsFinished(&correlator, SIGNAL(signalAllItemsCorrelated()));
    QVERIFY(spyItemsFinished.isValid());
    QSignalSpy spyItemsCorrelated(&correlator, SIGNAL(signalItemsCorrelated(KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List)));
    QVERIFY(spyItemsCorrelated.isValid());

    TrackCorrelator::CorrelationOptions correlationOptions;
    correlationOptions.maxGapTime = 0;
    correlationOptions.interpolate = true;
    correlationOptions.interpolationDstTime = 31;
    correlator.correlate(itemsToCorrelate, correlationOptions);

    while (spyItemsFinished.isEmpty())
    {
        QTest::qWait(100);
    }
    QCOMPARE(spyItemsFinished.count(), 1);
    QCOMPARE(spyItemsCorrelated.count(), 1);

    KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List myCorrelatedItems = spyItemsCorrelated.first().first().value<KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List>();
    QCOMPARE(myCorrelatedItems.count(), 1);
    QCOMPARE(myCorrelatedItems.first().coordinates, KGeoMap::GeoCoordinates::fromGeoUrl("geo:17.5,0.5,3"));
    QCOMPARE(myCorrelatedItems.first().nSatellites, -1);
    QCOMPARE(myCorrelatedItems.first().hDop, -1.0);
}
