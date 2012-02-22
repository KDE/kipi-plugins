/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-28
 * @brief  Test loading and saving of data in KipiImageItem.
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

#include "test_kipiimageitem.moc"

// Qt includes

#include <QDateTime>
#include <QScopedPointer>

// KDE includes

#include <qtest_kde.h>
#include <kdebug.h>
#include <kurl.h>

// local includes

#include "kpmetadata.h"
#include "../gpsdatacontainer.h"
#include "../kipiimageitem.h"

using namespace KIPIPlugins;
using namespace KIPIGPSSyncPlugin;

QTEST_KDEMAIN_CORE(TestKipiImageItem)

void TestKipiImageItem::initTestCase()
{
    // initialize exiv2 before doing any multitasking
    KPMetadata::initializeExiv2();
}

void TestKipiImageItem::cleanupTestCase()
{
    // clean up the exiv2 memory:
    KPMetadata::cleanupExiv2();
}

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


KipiImageItem* ItemFromFile(const KUrl& url)
{
    QScopedPointer<KipiImageItem> imageItem(new KipiImageItem(0, url));

    if (imageItem->loadImageData(true, true))
    {
        return imageItem.take();
    }

    return 0;
}

/**
 * @brief Dummy test that does nothing
 */
void TestKipiImageItem::testNoOp()
{
}

void TestKipiImageItem::testBasicLoading()
{
    const KUrl testDataDir = GetTestDataDirectory();

    {
        // test failure on not-existing file
        QScopedPointer<KipiImageItem> imageItem(ItemFromFile(KUrl(testDataDir, "not-existing")));
        QVERIFY(!imageItem);
    }

    {
        // load a file without GPS info
        QScopedPointer<KipiImageItem> imageItem(ItemFromFile(KUrl(testDataDir, "exiftest-nogps.png")));
        QVERIFY(imageItem);

        const GPSDataContainer container = imageItem->gpsData();
        QVERIFY(!container.hasCoordinates());
        QVERIFY(!container.hasAltitude());
        QVERIFY(!container.hasNSatellites());
        QVERIFY(!container.hasDop());
        QVERIFY(!container.hasFixType());
    }

    {
        // load a file with geo:5,15,25
        QScopedPointer<KipiImageItem> imageItem(ItemFromFile(KUrl(testDataDir, "exiftest-5_15_25.jpg")));
        QVERIFY(imageItem);

        const GPSDataContainer container = imageItem->gpsData();
        QVERIFY(container.hasCoordinates());
        QVERIFY(container.hasAltitude());
        QVERIFY(container.getCoordinates().lat()==5.0);
        QVERIFY(container.getCoordinates().lon()==15.0);
        QVERIFY(container.getCoordinates().alt()==25.0);
        QVERIFY(!container.hasNSatellites());
        QVERIFY(!container.hasDop());
        QVERIFY(!container.hasFixType());
    }
}
