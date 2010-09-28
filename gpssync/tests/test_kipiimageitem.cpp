/* ============================================================
 *
 * Date        : 2010-06-28
 * Description : test loading and saving of data in KipiImageItem
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
#include <QScopedPointer>

// KDE includes

#include <qtest_kde.h>
#include <kdebug.h>
#include <kurl.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// local includes

#include "test_kipiimageitem.moc"
#include "../gpsdatacontainer.h"
#include "../kipiimageitem.h"

using namespace KIPIGPSSyncPlugin;

QTEST_KDEMAIN_CORE(TestKipiImageItem)

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
        QVERIFY(!container.hasHDop());
        QVERIFY(!container.hasPDop());
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
        QVERIFY(!container.hasHDop());
        QVERIFY(!container.hasPDop());
        QVERIFY(!container.hasFixType());
    }
}

