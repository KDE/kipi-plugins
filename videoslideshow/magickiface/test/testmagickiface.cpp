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

#include "testmagickiface.moc"

// To run QTEST_MAIN as a GUI application
#define QT_GUI_LIB

// Qt includes

#include <QtTest>
#include <QFileDialog>
#include <QDir>

// Local includes

#include "magickiface.h"

QTEST_MAIN (TestMagickIface)

TestMagickIface::TestMagickIface()
{
    api  = new MagickApi(QDir::tempPath());
    file = new QFile(QFileDialog::getOpenFileName(NULL, tr("SelectImage"), "/home", tr("Image Files (*.png *.jpg)")));
    spy  = new QSignalSpy(api, SIGNAL(signalsAPIError(QString)));
    QVERIFY (spy->isValid());
}

TestMagickIface::~TestMagickIface()
{
    delete spy;
    delete api;

    if (file->isOpen())
        file->close();

    delete file;
}

void TestMagickIface::testCreateImage()
{
    spy->clear();

    MagickImage* const img = api->createImage(QString("green"), 300, 300);
    QVERIFY (img != 0);

    bool done = api->displayImage(*img);
    QVERIFY (done);

    done      = api->freeImage(*img);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testDuplicateImage()
{
    spy->clear();

    QVERIFY (file != 0);

    if (file->isOpen())
        file->close();

    MagickImage* const img = api->loadImage(file->fileName());
    QVERIFY (img != 0 );

    MagickImage* const dupImg = api->duplicateImage(*img);
    QVERIFY (dupImg != 0);

    bool done = api->displayImage(*dupImg);
    QVERIFY (done);

    done = api->freeImage(*img);
    QVERIFY (done);

    done = api->freeImage(*dupImg);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testOverlayImage()
{
    spy->clear();

    MagickImage* const src = api->createImage(QString("red"), 100, 100);
    QVERIFY (src != 0);

    MagickImage* const dst = api->createImage(QString("yellow"), 300, 300);
    QVERIFY (dst != 0);

    bool done = api->overlayImage(*dst,1,1,*src);
    QVERIFY (done);

    done = api->displayImage(*dst);
    QVERIFY (done);

    done = api->freeImage(*src);
    QVERIFY (done);

    done = api->freeImage(*dst);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testBlendImage()
{
    spy->clear();

    MagickImage* const src0 = api->createImage(QString("green"), 100, 100);
    QVERIFY (src0 != 0);

    MagickImage* const src1 = api->createImage(QString("white"), 100, 100);
    QVERIFY (src1 != 0);

    MagickImage* const dst = api->createImage(QString("yellow"), 100, 100);
    QVERIFY (dst != 0);

    int done = api->blendImage(*dst, *src0, *src1, 0.75);
    QVERIFY (done != -1);

    done = api->displayImage(*dst);
    QVERIFY (done);

    done = api->freeImage(*src0);
    QVERIFY (done);

    done = api->freeImage(*src1);
    QVERIFY (done);

    done = api->freeImage(*dst);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testBitblitImage()
{
    spy->clear();

    MagickImage* const src = api->createImage(QString("green"), 200, 200);
    QVERIFY (src != 0);

    MagickImage* const dst = api->createImage(QString("blue"), 300, 300);
    QVERIFY (dst != 0);

    int done = api->bitblitImage(*dst, 100, 100, *src, 0, 0, 100, 100);
    QVERIFY (done != -1);

    done = api->displayImage(*dst);
    QVERIFY (done);

    done = api->freeImage(*src);
    QVERIFY (done);

    done = api->freeImage(*dst);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testScaleImage()
{
    spy->clear();

    MagickImage* const src = api->createImage(QString("white"), 150, 150);
    QVERIFY (src != 0);

    int done = api->scaleImage(*src, 300, 300);
    QVERIFY (done != -1);

    done = api->displayImage(*src);
    QVERIFY (done);

    done = api->freeImage(*src);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testScaleblitImage()
{
    spy->clear();

    MagickImage* const src = api->createImage(QString("green"), 200, 200);
    QVERIFY (src != 0);

    MagickImage* const dst = api->createImage(QString("blue"), 300, 300);
    QVERIFY (dst != 0);

    int done = api->scaleblitImage(*dst, 0, 0, 200, 200, *src, 0, 0, 100, 100);
    QVERIFY (done != -1);

    done = api->displayImage(*dst);
    QVERIFY (done);

    done = api->freeImage(*src);
    QVERIFY (done);

    done = api->freeImage(*dst);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testBorderImage()
{
    spy->clear();

    QVERIFY (file != 0);

    MagickImage* const src = api->loadStream(*file);
    QVERIFY (src != 0);

    MagickImage* const bimg = api->borderImage(*src, QString("green"), 50, 50);
    QVERIFY (bimg != 0);

    bool done = api->displayImage(*bimg);
    QVERIFY (done);

    done = api->freeImage(*src);
    QVERIFY (done);

    done = api->freeImage(*bimg);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testGeoScaleImage()
{
    spy->clear();

    QVERIFY (file != 0);

    MagickImage* const src = api->loadImage(file->fileName());
    QVERIFY (src != 0);

    MagickImage* const gsimg = api->geoscaleImage(*src, 0, 0, 300, 300, 600, 600);
    QVERIFY (gsimg != 0);

    bool done = api->freeImage(*src);
    QVERIFY (done);

    done = api->displayImage(*gsimg);
    QVERIFY (done);

    done = api->freeImage(*gsimg);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testLoadAndSaveFile()
{
    spy->clear();

    QVERIFY (file != 0);

    MagickImage* const testImage = api->loadImage(file->fileName());
    QVERIFY (testImage != 0);

    int isSaved = api->saveToFile(*testImage, QString("test.ppm"));
    QVERIFY (isSaved);

    bool done = api->displayImage(*testImage);
    QVERIFY (done);

    done = api->freeImage(*testImage);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}

void TestMagickIface::testLoadAndSaveStream()
{
    spy->clear();

    QVERIFY (file != 0);

    QFile file2("test2.ppm");

    MagickImage* const testImage = api->loadStream(*file);
    QVERIFY (testImage != 0);

    int isSaved = api->saveToStream(*testImage,file2);
    QVERIFY (isSaved);

    bool done = api->displayImage(*testImage);
    QVERIFY (done);

    if (file2.isOpen())
        file2.close();

    done = api->freeImage(*testImage);
    QVERIFY (done);

    QCOMPARE (spy->count(), 0);
}
