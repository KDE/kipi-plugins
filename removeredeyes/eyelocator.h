/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the eyelocator detects and removes red eyes
 *               from images
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef EYELOCATOR_H
#define EYELOCATOR_H

// OpenCV includes.

#ifdef WIN32 
#include <cv.h> 
#include <highgui.h>
#else 	 
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

class KUrl;

namespace KIPIRemoveRedEyesPlugin
{

class EyeLocator
{
public:

    EyeLocator(const char* filename,
               const char* classifierFile,
               double scaleFactor,
               int neighborGroups,
               double minRoundness,
               int minBlobsize);
    ~EyeLocator();

public:

    int             redEyes() const;
    void            saveImage(const char* path);

private:

    IplImage*       m_aChannel;
    IplImage*       m_gray;
    IplImage*       m_lab;
    IplImage*       m_redMask;
    IplImage*       m_src;

    double          m_minRoundness;
    double          m_scaleFactor;
    int             m_minBlobsize;
    int             m_neighborGroups;
    int             m_possible_eyes;
    int             m_red_eyes;

private:

    int             findPossibleEyes(double csf, int ngf, const char* classifierFile);

    void            removeRedEyes();
    void            findBlobs(IplImage* i_mask, int minsize);
    void            generateMask(int i_v, CvSeq* i_eyes);
};
} // namespace KIPIRemoveRedEyesPlugin

#endif
