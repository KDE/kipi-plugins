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

#include "haarclassifierlocator.moc"

// Qt includes

#include <QByteArray>
#include <QFile>
#include <QString>

// KDE includes

#include <kconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "BlobResult.h"
#include "haarsettings.h"
#include "haarsettingswidget.h"
#include "simplesettings.h"

namespace KIPIRemoveRedEyesPlugin
{

struct HaarClassifierLocator::Private
{
    Private() :
        aChannel(0),
        gray(0),
        lab(0),
        redMask(0),
        original(0),
        possible_eyes(0),
        red_eyes(0),
        settingsWidget(0)
    {};

    static const QString  configGroupName;
    static const QString  configSimpleModeEntry;
    static const QString  configMinimumBlobSizeEntry;
    static const QString  configMinimumRoundnessEntry;
    static const QString  configNeighborGroupsEntry;
    static const QString  configScalingFactorEntry;
    static const QString  configUseStandardClassifierEntry;
    static const QString  configClassifierEntry;

    IplImage*             aChannel;
    IplImage*             gray;
    IplImage*             lab;
    IplImage*             redMask;
    IplImage*             original;

    int                   possible_eyes;
    int                   red_eyes;
    QString               classifierFile;
    HaarSettingsWidget*   settingsWidget;
    HaarSettings          settings;
};

const QString HaarClassifierLocator::Private::configGroupName("RemoveRedEyes %1 Settings");
const QString HaarClassifierLocator::Private::configSimpleModeEntry("Simple Mode");
const QString HaarClassifierLocator::Private::configMinimumBlobSizeEntry("Minimum Blob Size");
const QString HaarClassifierLocator::Private::configMinimumRoundnessEntry("Minimum Roundness");
const QString HaarClassifierLocator::Private::configNeighborGroupsEntry("Neighbor Groups");
const QString HaarClassifierLocator::Private::configScalingFactorEntry("Scaling Factor");
const QString HaarClassifierLocator::Private::configUseStandardClassifierEntry("Use Standard Classifier");
const QString HaarClassifierLocator::Private::configClassifierEntry("Classifier");

// --------------------------------------------------------

int HaarClassifierLocator::findPossibleEyes(double csf, int ngf, const char* classifierFile)
{
    // eyes sequence will reside in the storage
    CvMemStorage* storage            = cvCreateMemStorage(0);
    CvSeq* eyes                      = 0;
    int numEyes                      = 0;

    // load classifier cascade from XML file
    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad(classifierFile);

    // get the sequence of eyes rectangles
    cvCvtColor(d->original, d->gray, CV_BGR2GRAY);
    eyes = cvHaarDetectObjects(d->gray,
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
        cvCvtColor(d->original, d->lab, CV_BGR2Lab);

        // create aChannel image
        cvSplit(d->lab, 0, d->aChannel, 0, 0);

        for (int v = 0; v < numEyes; ++v)
        {
            generateMask(v, eyes);
        }
    }

    cvReleaseMemStorage(&storage);
    cvReleaseHaarClassifierCascade(&cascade);

    return numEyes;
}

void HaarClassifierLocator::removeRedEyes()
{
    IplImage* removed_redchannel = cvCreateImage(cvGetSize(d->original), d->original->depth, 3);
    cvCopy(d->original, removed_redchannel);

    // number of channels
    int nc                       = removed_redchannel->nChannels;
    uchar* ptr                   = 0;

    for (int y = 0; y < removed_redchannel->height; ++y)
    {
        ptr = (uchar*)(removed_redchannel->imageData + y * removed_redchannel->widthStep);

        for (int x = 0; x < removed_redchannel->width; ++x)
        {
            // remember: we are in BGR colorspace
            ptr[nc* x+2] = (uchar)((ptr[nc*x+2] * 0.02)
                                   + (ptr[nc*x+1] * 0.68)
                                   + (ptr[nc*x+0] * 0.3));
        }
    }

    // smooth the mask
    cvSmooth(d->redMask, d->redMask, CV_GAUSSIAN);
    cvCopy(removed_redchannel, d->original, d->redMask);

    // release temp image again
    cvReleaseImage(&removed_redchannel);
}

void HaarClassifierLocator::generateMask(int i_v, CvSeq* const i_eyes)
{
    // get ROI
    CvRect* const r = reinterpret_cast<CvRect*>(cvGetSeqElem(i_eyes, i_v));
    cvSetImageROI(d->aChannel, *r);
    cvSetImageROI(d->redMask, *r);

    // treshold on aChannel
    cvThreshold(d->aChannel, d->redMask, 150, 255, CV_THRESH_BINARY);

    // reset ROI
    cvResetImageROI(d->aChannel);
    cvResetImageROI(d->redMask);

    // close masks
    cvDilate(d->redMask, d->redMask, 0, 1);
    cvErode(d->redMask, d->redMask, 0, 1);

    int minSize = d->settings.minBlobsize * d->settings.minBlobsize;
    findBlobs(d->redMask, minSize);
}

void HaarClassifierLocator::findBlobs(IplImage* const i_mask, int minsize)
{
    CBlobResult blobs;
    blobs = CBlobResult(i_mask,0,0,true);

    // filter out mask regions with a minimum size
    blobs.Filter( blobs,
                  B_INCLUDE,
                  CBlobGetArea(),
                  B_GREATER, minsize);

    // filter out mask regions with a minimum roundness
    blobs.Filter( blobs,
                  B_INCLUDE,
                  CBlobGetCompactness(),
                  B_LESS_OR_EQUAL,
                  d->settings.minRoundness);

    // filter out mask background object
    blobs.Filter(blobs,
                 B_INCLUDE,
                 CBlobGetExterior(),
                 B_EQUAL,
                 0);

    // fill the mask
    cvFillImage(i_mask, 0);
    d->red_eyes = 0;

    for (int i = 0; i < blobs.GetNumBlobs(); ++i)
    {
        CBlob tmp = blobs.GetBlob(i);
        tmp.FillBlob(i_mask, CV_RGB(255, 255, 255));
        d->red_eyes++;
    }
}

void HaarClassifierLocator::allocateBuffers()
{
    if (!d->original)
    {
        return;
    }

    // allocate all buffers
    d->lab      = cvCreateImage(cvGetSize(d->original), d->original->depth, 3);
    d->gray     = cvCreateImage(cvGetSize(d->original), d->original->depth, 1);
    d->aChannel = cvCreateImage(cvGetSize(d->original), d->original->depth, 1);
    d->redMask  = cvCreateImage(cvGetSize(d->original), d->original->depth, 1);

    // reset masks
    cvFillImage(d->aChannel, 0);
    cvFillImage(d->redMask, 0);
}

void HaarClassifierLocator::clearBuffers()
{
    cvReleaseImage(&d->aChannel);
    cvReleaseImage(&d->gray);
    cvReleaseImage(&d->lab);
    cvReleaseImage(&d->redMask);
    cvReleaseImage(&d->original);
}

// --------------------------------------------------------------------

HaarClassifierLocator::HaarClassifierLocator()
    : Locator(), d(new Private)
{
    setObjectName("HaarClassifierLocator");
    d->settingsWidget = new HaarSettingsWidget;
    readSettings();
}

HaarClassifierLocator::~HaarClassifierLocator()
{
    clearBuffers();
    writeSettings();
    delete d;
}

void HaarClassifierLocator::saveImage(const QString& path, SaveResult type)
{
    QByteArray dest      = QFile::encodeName(path);
    const char* savePath = dest.data();

    switch (type)
    {
        case Final:
            cvSaveImage(savePath, d->original);
            break;
        case OriginalPreview:
            cvSaveImage(savePath, d->original);
            break;
        case CorrectedPreview:
            cvSaveImage(savePath, d->original);
            break;
        case MaskPreview:
            cvSaveImage(savePath, d->redMask);
            break;
    }
}

QWidget* HaarClassifierLocator::settingsWidget()
{
    return d->settingsWidget;
}

int HaarClassifierLocator::startCorrection(const QString& src, const QString& dest)
{
    if (src.isEmpty())
    {
        return -1;
    }

    // update settings
    updateSettings();

    // clear buffers to make sure no old data is still remaining
    clearBuffers();

    // load source image
    QByteArray srcBA = QFile::encodeName(src);
    d->original      = cvLoadImage(srcBA.data());

    // allocate all buffers
    allocateBuffers();

    // find possible eyes
    QByteArray clsFile = QFile::encodeName(d->settings.classifierFile);
    d->possible_eyes   = findPossibleEyes(d->settings.scaleFactor,
                                          d->settings.neighborGroups,
                                          clsFile.data());

    // remove red-eye effect
    if (d->possible_eyes > 0)
    {
        removeRedEyes();
    }

    // save image
    if (!dest.isEmpty())
    {
        saveImage(dest, Final);
    }

    clearBuffers();
    return (d->red_eyes > 0) ? d->red_eyes : 0;
}

int HaarClassifierLocator::startTestrun(const QString& src)
{
    return startCorrection(src, QString());
}

int HaarClassifierLocator::startPreview(const QString& src)
{
    return startCorrection(src, QString());
}

void HaarClassifierLocator::readSettings()
{
    KConfig config("kipirc");
    QString configGroup = d->configGroupName.arg(this->objectName());
    KConfigGroup group = config.group(configGroup);

    d->settings.simpleMode            = group.readEntry(d->configSimpleModeEntry,            (int)SimpleSettings::Standard);
    d->settings.minBlobsize           = group.readEntry(d->configMinimumBlobSizeEntry,       10);
    d->settings.minRoundness          = group.readEntry(d->configMinimumRoundnessEntry,      3.2);
    d->settings.neighborGroups        = group.readEntry(d->configNeighborGroupsEntry,        2);
    d->settings.scaleFactor           = group.readEntry(d->configScalingFactorEntry,         1.2);
    d->settings.useStandardClassifier = group.readEntry(d->configUseStandardClassifierEntry, true);
    d->settings.classifierFile        = group.readEntry(d->configClassifierEntry,            STANDARD_CLASSIFIER);

    d->settingsWidget->loadSettings(d->settings);
}

void HaarClassifierLocator::writeSettings()
{
    KConfig config("kipirc");
    QString configGroup = d->configGroupName.arg(this->objectName());
    KConfigGroup group = config.group(configGroup);

    d->settings = d->settingsWidget->readSettingsForSave();

    group.writeEntry(d->configSimpleModeEntry,            d->settings.simpleMode);
    group.writeEntry(d->configMinimumBlobSizeEntry,       d->settings.minBlobsize);
    group.writeEntry(d->configMinimumRoundnessEntry,      d->settings.minRoundness);
    group.writeEntry(d->configNeighborGroupsEntry,        d->settings.neighborGroups);
    group.writeEntry(d->configScalingFactorEntry,         d->settings.scaleFactor);
    group.writeEntry(d->configUseStandardClassifierEntry, d->settings.useStandardClassifier);
    group.writeEntry(d->configClassifierEntry,            d->settings.classifierFile);

    config.sync();
}

void HaarClassifierLocator::updateSettings()
{
    d->settings = d->settingsWidget->readSettings();
}

} // namespace HaarClassifierLocator
