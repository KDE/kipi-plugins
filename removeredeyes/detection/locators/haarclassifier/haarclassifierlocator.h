/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-02-12
 * Description : locator that uses an OpenCV haar cascades classifier
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at googlemail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef HAARCLASSIFIERLOCATOR_H
#define HAARCLASSIFIERLOCATOR_H

// OpenCV includes

// To avoid broken compilation with OpenCV >= 2.0
#undef HAVE_CONFIG_H

#if defined (__APPLE__)
#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#else
#include <opencv/cv.h>
#include <opencv2/highgui/highgui_c.h>
#endif

// Local includes

#include "locator.h"

class QString;

namespace KIPIRemoveRedEyesPlugin
{

class HaarClassifierLocator : public Locator
{
    Q_OBJECT

public:

    HaarClassifierLocator();
    virtual ~HaarClassifierLocator();

    int startCorrection(const QString& src, const QString& dest);
    int startTestrun(const QString& src);
    int startPreview(const QString& src);

    QWidget* settingsWidget();

    void readSettings();
    void writeSettings();

private Q_SLOTS:

    void updateSettings();

private:

    int  findPossibleEyes(double csf, int ngf, const char* classifierFile);
    void removeRedEyes();
    void findBlobs(IplImage* i_mask, int minsize);
    void generateMask(int i_v, CvSeq* i_eyes);
    void allocateBuffers();
    void clearBuffers();
    void saveImage(const QString& path, SaveResult type);

private:

    struct HaarClassifierLocatorPriv;
    HaarClassifierLocatorPriv* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* HAARCLASSIFIERLOCATOR_H */
