/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-06-01
 * @brief  UnitTests for ImageMagick API
 *
 * @author Copyright (C) 2012      by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *         Copyright (C) 2012-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef TESTMAGICKIFACE_H
#define TESTMAGICKIFACE_H

// Qt includes

#include <QObject>

class QSignalSpy;
class QFile;

namespace KIPIPlugins
{
    class MagickApi;
}

using namespace KIPIPlugins;

class TestMagickIface : public QObject
{
    Q_OBJECT

public:

    TestMagickIface();
    ~TestMagickIface();

private Q_SLOTS:

    void testCreateImage();
    void testDuplicateImage();
    void testOverlayImage();
    void testBlendImage();
    void testBitblitImage();
    void testScaleImage();
    void testScaleblitImage();
    void testGeoScaleImage();
    void testBorderImage();

    void testLoadAndSaveFile();
    void testLoadAndSaveStream();

private:

    MagickApi*  api;

    /// used to check the signals emitted by a class
    QSignalSpy* spy;

    QFile*      file;
};

#endif // TESTMAGICKIFACE_H
