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

class EyeLocatorPriv
{
public:

    EyeLocatorPriv()
    {
        aChannel          = 0;
        gray              = 0;
        lab               = 0;
        redMask           = 0;
        src               = 0;
        minRoundness      = 0.0;
        scaleFactor       = 0.0;
        minBlobsize       = 0;
        neighborGroups    = 0;
        possible_eyes     = 0;
        red_eyes          = 0;
    };

    int     findPossibleEyes(double csf, int ngf, const char* classifierFile);

    void    removeRedEyes();
    void    findBlobs(IplImage* i_mask, int minsize);
    void    generateMask(int i_v, CvSeq* i_eyes);

public:

    IplImage*       aChannel;
    IplImage*       gray;
    IplImage*       lab;
    IplImage*       redMask;
    IplImage*       src;

    double          minRoundness;
    double          scaleFactor;
    int             minBlobsize;
    int             neighborGroups;
    int             possible_eyes;
    int             red_eyes;
};

int EyeLocatorPriv::findPossibleEyes(double csf, int ngf, const char* classifierFile)
{
    // eyes sequence will reside in the storage
    CvMemStorage* storage=cvCreateMemStorage(0);
    CvSeq* eyes;
    int numEyes = 0;

    // load classifier cascade from XML file
    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad(classifierFile);

    // get the sequence of eyes rectangles
    eyes = cvHaarDetectObjects(gray,
                               cascade, storage,
                               csf,
                               ngf,
                               CV_HAAR_DO_CANNY_PRUNING, // use Canny edge detector
                               cvSize(0,0));

    // extract each region as a new image
    numEyes = eyes ? eyes->total : 0;

    for (int v=0; v < numEyes; v++)
        generateMask(v, eyes);

    cvReleaseMemStorage(&storage);
    cvReleaseHaarClassifierCascade(&cascade);

    return numEyes;
}

void EyeLocatorPriv::removeRedEyes()
{
    IplImage* removed_redchannel = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCopy(src, removed_redchannel);

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

    // copy corrected image over src image
    cvCopy(removed_redchannel, src, redMask);

    // release temp image again
    cvReleaseImage(&removed_redchannel);
}

void EyeLocatorPriv::generateMask(int i_v, CvSeq* i_eyes)
{
    // get ROI
    CvRect* r = (CvRect*)cvGetSeqElem(i_eyes, i_v);
    cvSetImageROI(aChannel, *r);
    cvSetImageROI(redMask, *r);

    // treshold on a channel
    cvThreshold(aChannel, redMask, 150, 255, CV_THRESH_BINARY);

    // reset ROI
    cvResetImageROI(aChannel);
    cvResetImageROI(redMask);

    // close masks
    cvDilate(redMask, redMask, 0, 1);
    cvErode(redMask, redMask, 0, 1);

    findBlobs(redMask, (minBlobsize * minBlobsize));
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
                       double minRoundness, int minBlobsize)
          : d(new EyeLocatorPriv)
{
    d->scaleFactor       = scaleFactor;
    d->neighborGroups    = neighborGroups;
    d->minBlobsize       = minBlobsize;
    d->minRoundness      = minRoundness;

    // open the image
    d->src               = cvLoadImage(filename);

    // allocate all buffers
    d->gray              = cvCreateImage(cvGetSize(d->src), IPL_DEPTH_8U, 1);
    d->lab               = cvCreateImage(cvGetSize(d->src), IPL_DEPTH_8U, 3);
    d->aChannel          = cvCreateImage(cvGetSize(d->src), IPL_DEPTH_8U, 1);
    d->redMask           = cvCreateImage(cvGetSize(d->src), IPL_DEPTH_8U, 1);

    // reset eye counter
    d->red_eyes          = 0;
    d->possible_eyes     = 0;

    // convert color spaces
    cvCvtColor(d->src, d->gray, CV_BGR2GRAY);
    cvCvtColor(d->src, d->lab, CV_BGR2Lab);

    // reset masks
    cvFillImage(d->aChannel, 0);
    cvFillImage(d->redMask, 0);

    // create a-channel image
    cvSplit(d->lab, 0, d->aChannel, 0, 0);

    // find possible eyes
    d->possible_eyes = d->findPossibleEyes(d->scaleFactor, d->neighborGroups, classifierFile);

    // remove red eyes effect
    if (d->possible_eyes > 0)
        d->removeRedEyes();

}

EyeLocator::~EyeLocator()
{
    cvReleaseImage(&d->aChannel);
    cvReleaseImage(&d->gray);
    cvReleaseImage(&d->lab);
    cvReleaseImage(&d->redMask);
    cvReleaseImage(&d->src);
    delete d;
}

int EyeLocator::redEyes() const
{
    return d->red_eyes;
}

void EyeLocator::saveImage(const char * path)
{
    cvSaveImage(path, d->src);
}

} // namespace KIPIRemoveRedEyesPlugin
