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

#ifndef TEST_RGPARSING_H
#define TEST_RGPARSING_H

// Qt includes

#include <QtTest/QtTest>

// KDE includes

// local includes

class TestRGParsing : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testNoOp();
    void testVerifyRG();   
 
};

#endif /* TEST_RGPARSING_H */

