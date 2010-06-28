/* ============================================================
 *
 * Date        : 2010-06-28
 * Description : test for reading GPS related exif data using exiv2
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

#include "test_gpsexif.moc"
#include "../gpsdatacontainer.h"

using namespace KIPIGPSSyncPlugin;

QTEST_KDEMAIN_CORE(TestGPSExif)

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

bool LoadContainerFromFile(KUrl url, GPSDataContainer* const container)
{
    if (!container)
        return false;

    QScopedPointer<KExiv2Iface::KExiv2> exiv2Iface(new KExiv2Iface::KExiv2);
    if (!exiv2Iface->load(url.path()))
        return false;

    double alt, lat, lng;
    bool hasCoordinates = exiv2Iface->getGPSInfo(alt, lat, lng);
    kDebug()<<hasCoordinates;
    if (hasCoordinates)
    {
        container->setCoordinates(WMW2::WMWGeoCoordinate(lat, lng, alt));
    }

    return true;
}

/**
 * @brief Dummy test that does nothing
 */
void TestGPSExif::testNoOp()
{
}

void TestGPSExif::testBasicLoading()
{
    const KUrl testDataDir = GetTestDataDirectory();

    {
        // test failure on not-existing file
        GPSDataContainer container;
        QVERIFY(!LoadContainerFromFile(KUrl(testDataDir, "not-existing"), &container));
    }

    {
        // load a file without GPS info
        GPSDataContainer container;
        QVERIFY(LoadContainerFromFile(KUrl(testDataDir, "exiftest-nogps.png"), &container));
        QVERIFY(!container.hasCoordinates());
        QVERIFY(!container.hasAltitude());
        QVERIFY(!container.hasNSatellites());
        QVERIFY(!container.hasHDop());
        QVERIFY(!container.hasPDop());
        QVERIFY(!container.hasFixType());
    }

    {
        // load a file with geo:5,15,25
        GPSDataContainer container;
        QVERIFY(LoadContainerFromFile(KUrl(testDataDir, "exiftest-5_15_25.jpg"), &container));
        QVERIFY(container.hasCoordinates());
        QVERIFY(container.hasAltitude());
        QVERIFY(!container.hasNSatellites());
        QVERIFY(!container.hasHDop());
        QVERIFY(!container.hasPDop());
        QVERIFY(!container.hasFixType());
    }
}

