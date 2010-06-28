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

#ifndef TEST_GPSEXIF_H
#define TEST_GPSEXIF_H

// Qt includes

#include <QtTest/QtTest>

// KDE includes

// local includes

class TestGPSExif : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testNoOp();
    void testBasicLoading();

};

#endif /* TEST_GPSEXIF_H */

