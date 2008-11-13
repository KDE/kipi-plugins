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

#include "eyelocator.h"

// OpenCV includes.

#include <BlobResult.h>

#ifdef WIN32
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

namespace KIPIRemoveRedEyesPlugin
{
// size to scale down to
const int minSize = 600;

class EyeLocatorPriv
{
public:

    EyeLocatorPriv()
    {
        aChannel          = 0;
        gray              = 0;
        lab               = 0;
        redMask           = 0;
        temporary         = 0;
        original          = 0;
        minRoundness      = 0.0;
        scaleFactor       = 0.0;
        minBlobsize       = 0;
        neighborGroups    = 0;
        possible_eyes     = 0;
        red_eyes          = 0;
    };

    int     getMinimumSize(IplImage* src);
    int     findPossibleEyes(double csf, int ngf, const char* classifierFile);

    void    removeRedEyes();
    void    findBlobs(IplImage* i_mask, int minsize);
    void    generateMask(int i_v, CvSeq* i_eyes);

    IplImage* scaleDownImage(IplImage* src);

public:

    IplImage*       aChannel;
    IplImage*       gray;
    IplImage*       lab;
    IplImage*       redMask;
    IplImage*       temporary;
    IplImage*       original;

    bool            scaleDown;
    double          minRoundness;
    double          scaleFactor;
    int             minBlobsize;
    int             neighborGroups;
    int             possible_eyes;
    int             red_eyes;
};

int EyeLocatorPriv::getMinimumSize(IplImage* src)
{
    int minimum = src->width < src->height ? src->width : src->height;
    return minimum;
}

IplImage* EyeLocatorPriv::scaleDownImage(IplImage* src)
{
    int minimum = getMinimumSize(src);
    float factor = minSize / (float) minimum;

    IplImage* out = cvCreateImage(cvSize((int) src->width  * factor,
                                         (int) src->height * factor),
                                  src->depth,
                                  src->nChannels);
    cvResize(src, out);
    return out;
}

int EyeLocatorPriv::findPossibleEyes(double csf, int ngf, const char* classifierFile)
{
    // eyes sequence will reside in the storage
    CvMemStorage* storage=cvCreateMemStorage(0);
    CvSeq* eyes;
    int numEyes = 0;

    // load classifier cascade from XML file
    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad(classifierFile);

    // get the sequence of eyes rectangles
    cvCvtColor(temporary, gray, CV_BGR2GRAY);
    eyes = cvHaarDetectObjects(gray,
                               cascade, storage,
                               csf,
                               ngf,
                               CV_HAAR_DO_CANNY_PRUNING, // use Canny edge detector
                               cvSize(0,0));

    // extract each region as a new image
    numEyes = eyes ? eyes->total : 0;
    if (numEyes > 0)
    {
        // generate LAB color space image
        cvCvtColor(temporary, lab, CV_BGR2Lab);

        // create aChannel image
        cvSplit(lab, 0, aChannel, 0, 0);

        for (int v = 0; v < numEyes; v++)
            generateMask(v, eyes);
    }

    cvReleaseMemStorage(&storage);
    cvReleaseHaarClassifierCascade(&cascade);

    return numEyes;
}

void EyeLocatorPriv::removeRedEyes()
{
    IplImage* removed_redchannel = cvCreateImage(cvGetSize(original), original->depth, 3);
    cvCopy(original, removed_redchannel);

    // remove red channel
    uchar* c_data  = (uchar*) removed_redchannel->imageData;
    int c_step     = removed_redchannel->widthStep / sizeof(uchar);
    int c_channels = removed_redchannel->nChannels;

    for (int i = 0; i < removed_redchannel->height - 1; i++)
    {
        for (int j=0; j<removed_redchannel->width-1; j++)
        {
            c_data[i*c_step+j*c_channels+2] = uchar(uchar(c_data[i*c_step+j*c_channels+2])*0.02 +
                                                    uchar(c_data[i*c_step+j*c_channels+1])*0.68 +
                                                    uchar(c_data[i*c_step+j*c_channels+0])*0.3);
        }
    }

    // smooth the mask
    cvSmooth(redMask, redMask, CV_GAUSSIAN);

    // scale up mask if needed
    if (scaleDown)
    {
        IplImage* upscaledMasked = cvCreateImage(cvGetSize(original), original->depth, 1);
        cvResize(redMask, upscaledMasked);
        cvCopy(removed_redchannel, original, upscaledMasked);
        cvReleaseImage(&upscaledMasked);
    }
    else
        cvCopy(removed_redchannel, original, redMask);

    // release temp image again
    cvReleaseImage(&removed_redchannel);
}

void EyeLocatorPriv::generateMask(int i_v, CvSeq* i_eyes)
{
    // get ROI
    CvRect* r = (CvRect*)cvGetSeqElem(i_eyes, i_v);
    cvSetImageROI(aChannel, *r);
    cvSetImageROI(redMask, *r);

    // treshold on aChannel
    cvThreshold(aChannel, redMask, 150, 255, CV_THRESH_BINARY);

    // reset ROI
    cvResetImageROI(aChannel);
    cvResetImageROI(redMask);

    // close masks
    cvDilate(redMask, redMask, 0, 1);
    cvErode(redMask, redMask, 0, 1);

    int minSize = minBlobsize * minBlobsize;
    findBlobs(redMask, minSize);
}

void EyeLocatorPriv::findBlobs(IplImage* i_mask, int minsize)
{
    CBlobResult blobs;
    blobs = CBlobResult(i_mask,0,0,true);
    blobs.Filter( blobs,
                  B_INCLUDE,
                  CBlobGetArea(),
                  B_GREATER, minsize);
    blobs.Filter( blobs,
                  B_INCLUDE,
                  CBlobGetCompactness(),
                  B_LESS_OR_EQUAL,
                  minRoundness);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetExterior(), B_EQUAL, 0);

    // fill the mask
    cvFillImage(i_mask, 0);
    red_eyes = 0;
    for (int i = 0; i < blobs.GetNumBlobs(); i++)
    {
        CBlob tmp = blobs.GetBlob(i);
        tmp.FillBlob(i_mask, CV_RGB(255, 255, 255));
        red_eyes++;
    }
}

// --------------------------------------------------------------------

EyeLocator::EyeLocator(const char* filename, const char* classifierFile,
                       double scaleFactor, int neighborGroups,
                       double minRoundness, int minBlobsize, bool scaleDown)
          : d(new EyeLocatorPriv)
{
    d->scaleFactor       = scaleFactor;
    d->neighborGroups    = neighborGroups;
    d->minBlobsize       = minBlobsize;
    d->minRoundness      = double(1) / (double(minRoundness) / 100.0);

    d->original           = cvLoadImage(filename);

    if (scaleDown && minSize < d->getMinimumSize(d->original))
    {
        d->scaleDown         = true;
        d->temporary         = d->scaleDownImage(d->original);
    }
    else
    {
        d->scaleDown         = false;
        d->temporary         = cvCreateImage(cvGetSize(d->original), d->original->depth, d->original->nChannels);
        cvCopy(d->original, d->temporary);
    }

    // allocate all buffers
    d->lab               = cvCreateImage(cvGetSize(d->temporary), d->temporary->depth, 3);
    d->gray              = cvCreateImage(cvGetSize(d->temporary), d->temporary->depth, 1);
    d->aChannel          = cvCreateImage(cvGetSize(d->temporary), d->temporary->depth, 1);
    d->redMask           = cvCreateImage(cvGetSize(d->temporary), d->temporary->depth, 1);

    // reset masks
    cvFillImage(d->aChannel, 0);
    cvFillImage(d->redMask, 0);

    // find possible eyes
    d->possible_eyes = d->findPossibleEyes(d->scaleFactor, d->neighborGroups, classifierFile);

    // remove red-eye effect
    if (d->possible_eyes > 0)
        d->removeRedEyes();

}

EyeLocator::~EyeLocator()
{
    cvReleaseImage(&d->aChannel);
    cvReleaseImage(&d->gray);
    cvReleaseImage(&d->lab);
    cvReleaseImage(&d->redMask);
    cvReleaseImage(&d->temporary);
    cvReleaseImage(&d->original);
    delete d;
}

int EyeLocator::redEyes() const
{
    return d->red_eyes;
}

void EyeLocator::saveImage(const char * path)
{
    cvSaveImage(path, d->original);
}

} // namespace KIPIRemoveRedEyesPlugin
