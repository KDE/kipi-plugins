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

namespace KIPIRemoveRedEyesPlugin
{

EyeLocator::EyeLocator(const char* filename, const char* classifierFile,
                       double scaleFactor, int neighborGroups,
                       double minRoundness, int minBlobsize)
{
    m_scaleFactor       = scaleFactor;
    m_neighborGroups    = neighborGroups;
    m_minBlobsize       = minBlobsize;
    m_minRoundness      = minRoundness;

    // open the image
    m_src               = cvLoadImage(filename);

    // allocate all buffers
    m_gray              = cvCreateImage(cvGetSize(m_src), IPL_DEPTH_8U, 1);
    m_lab               = cvCreateImage(cvGetSize(m_src), IPL_DEPTH_8U, 3);
    m_aChannel          = cvCreateImage(cvGetSize(m_src), IPL_DEPTH_8U, 1);
    m_redMask           = cvCreateImage(cvGetSize(m_src), IPL_DEPTH_8U, 1);

    // reset eye counter
    m_red_eyes          = 0;
    m_possible_eyes     = 0;

    // convert color spaces
    cvCvtColor(m_src, m_gray, CV_BGR2GRAY);
    cvCvtColor(m_src, m_lab, CV_BGR2Lab);

    // reset masks
    cvFillImage(m_aChannel, 0);
    cvFillImage(m_redMask, 0);

    // create a-channel image
    cvSplit(m_lab, 0, m_aChannel, 0, 0);

    // find possible eyes
    m_possible_eyes = findPossibleEyes(m_scaleFactor, m_neighborGroups, classifierFile);

    // remove red eyes effect
    if (m_possible_eyes > 0)
        removeRedEyes();

}


EyeLocator::~EyeLocator()
{
    cvReleaseImage(&m_aChannel);
    cvReleaseImage(&m_gray);
    cvReleaseImage(&m_lab);
    cvReleaseImage(&m_redMask);
    cvReleaseImage(&m_src);
}


int EyeLocator::findPossibleEyes(double csf, int ngf, const char* classifierFile)
{
    // eyes sequence will reside in the storage
    CvMemStorage* storage=cvCreateMemStorage(0);
    CvSeq* eyes;
    int numEyes = 0;

    // load classifier cascade from XML file
    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad(classifierFile);

    // get the sequence of eyes rectangles
    eyes = cvHaarDetectObjects(m_gray,
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

void EyeLocator::generateMask(int i_v, CvSeq* i_eyes)
{
    // get ROI
    CvRect* r = (CvRect*)cvGetSeqElem(i_eyes, i_v);
    cvSetImageROI(m_aChannel, *r);
    cvSetImageROI(m_redMask, *r);

    // treshold on a channel
    cvThreshold(m_aChannel, m_redMask, 150, 255, CV_THRESH_BINARY);

    // reset ROI
    cvResetImageROI(m_aChannel);
    cvResetImageROI(m_redMask);

    // close masks
    cvDilate(m_redMask, m_redMask, 0, 1);
    cvErode(m_redMask, m_redMask, 0, 1);

    findBlobs(m_redMask, (m_minBlobsize * m_minBlobsize));
}


void EyeLocator::findBlobs(IplImage* i_mask, int minsize)
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
                  m_minRoundness);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetExterior(), B_EQUAL, 0);

    // fill the mask
    cvFillImage(i_mask, 0);
    m_red_eyes = 0;
    for (int i = 0; i < blobs.GetNumBlobs(); i++)
    {
        CBlob tmp = blobs.GetBlob(i);
        tmp.FillBlob(i_mask, CV_RGB(255, 255, 255));
        m_red_eyes++;
    }
}


int EyeLocator::redEyes() const
{
    return m_red_eyes;
}


void EyeLocator::removeRedEyes()
{
    IplImage* removed_redchannel = cvCreateImage(cvGetSize(m_src), IPL_DEPTH_8U, 3);
    cvCopy(m_src, removed_redchannel);

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
    cvSmooth(m_redMask, m_redMask, CV_GAUSSIAN);

    // copy corrected image over src image
    cvCopy(removed_redchannel, m_src, m_redMask);

    // release temp image again
    cvReleaseImage(&removed_redchannel);
}


void EyeLocator::saveImage(const char * path)
{
    cvSaveImage(path, m_src);
}

} // namespace KIPIRemoveRedEyesPlugin
