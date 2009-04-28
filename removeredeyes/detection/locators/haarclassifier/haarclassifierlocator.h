/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-02-12
 * Description : locator that uses an OpenCV haar cascades classifier
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifdef WIN32
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

// Local includes

#include "locator.h"

class QString;

namespace KIPIRemoveRedEyesPlugin
{

struct HaarClassifierLocatorPriv;

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

    HaarClassifierLocatorPriv* const d;
};

}

#endif /* HAARCLASSIFIERLOCATOR_H */
