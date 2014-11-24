/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-07-01
 * @brief  convert images to ppm format
 *
 * @author Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#include "actionthread.moc"

// Qt includes

#include <QDir>

// KDE includes

#include <kdebug.h>

// libkdcraw

#include <libkdcraw/kdcraw.h>

// Local includes

#include "processimage.h"
#include "magickiface.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIVideoSlideShowPlugin
{

class ActionThread::Private
{

public:

    Private()
    {
        running          = false;
        api              = 0;
        processImg       = 0;
        framerate        = 25;
        number           = 0;
        encoder          = 0;
        item             = 0;
        frameWidth       = 0;
        frameHeight      = 0;
        aspectcorrection = ASPECTCORRECTION_TYPE_AUTO;
        aspectRatio      = ASPECT_RATIO_DEFAULT;
        videoType        = VIDEO_VCD;
        videoFormat      = VIDEO_FORMAT_PAL;
    }

    MagickApi*            api;
    ProcessImage*         processImg;
    EncoderDecoder*       encoder;
    int                   framerate;
    ASPECTCORRECTION_TYPE aspectcorrection;
    ASPECT_RATIO          aspectRatio;
    VIDEO_TYPE            videoType;
    VIDEO_FORMAT          videoFormat;
    int                   frameWidth;
    int                   frameHeight;
    int                   number;
    QString               path;
    QString               audioPath;
    QString               savePath;
    MyImageListViewItem*  item;
    bool                  running;
    QDir                  dir;
};

ActionThread::ActionThread(QObject* const parent)
    : QThread(parent), d(new Private)
{
    qRegisterMetaType<ActionData>();
    d->running = true;
}

ActionThread::~ActionThread()
{
    cleanTempDir();
    delete d;
}

void ActionThread::run()
{
    MagickImage* img     = 0;
    MagickImage* imgnext = 0;

    // have to keep dummy items at first and last
    imgnext              = loadImage(d->item);
    int upperBound       = 0;

    while (d->item->getNextImageItem() && d->running)
    {
        if (img)
        {
            d->api->freeImage(*img);
        }

        img             = imgnext;
        d->item         = d->item->getNextImageItem();
        imgnext         = loadImage(d->item);
        upperBound      = d->item->getTime() * d->framerate;

        processItem(upperBound, img, imgnext, TYPE_IMAGE);

        ActionData ad;
        ad.action       = TYPE_IMAGE;
        ad.fileUrl      = d->item->getPrevImageItem()->url();
        ad.totalFrames  = upperBound;
        Q_EMIT frameCompleted(ad);

        upperBound      = getTransitionFrames(d->item);
        processItem(upperBound, img, imgnext, TYPE_TRANSITION); 

        ActionData tad;
        tad.action      = TYPE_TRANSITION;
        tad.fileUrl     = d->item->url();
        tad.totalFrames = upperBound;
        Q_EMIT frameCompleted(tad);
    }

    if (img)
    {
        d->api->freeImage(*img);
    }

    img            = imgnext;
    upperBound     = d->item->getTime() * d->framerate;
    processItem(upperBound, img, imgnext, TYPE_IMAGE);

    ActionData ad;
    ad.action      = TYPE_IMAGE;
    ad.fileUrl     = d->item->url();
    ad.totalFrames = upperBound;
    Q_EMIT frameCompleted(ad);

    if (img)
    {
        d->api->freeImage(*img);
    }

    if (!d->savePath.isNull())
    {
        d->encoder->encodeVideo(d->savePath, d->audioPath, d->videoFormat, d->videoType, d->path, d->aspectRatio);
        connect(d->encoder, SIGNAL(finished()),
                this, SLOT(quit()));
        exec();
    }

    Q_EMIT finished();
}

void ActionThread::cleanTempDir()
{
    d->dir.setPath(d->path);

    QStringList tempFiles = d->dir.entryList(QDir::Files);
    QString     tempFile;

    for (int i = 0; i < tempFiles.size(); i++)
    {
        tempFile = tempFiles.at(i);

        if (tempFile.endsWith(QLatin1String(".ppm")))
            d->dir.remove(tempFile);
    }

    d->dir.rmdir(d->savePath);
}

void ActionThread::processItem(int upperBound, MagickImage* const img, MagickImage* const imgNext, Action action)
{
/*
    // need to reimplement using appsrc plugin of gstreamer
    if(action == TYPE_IMAGE)
    {
        if(d->item->EffectName() == EFFECT_NONE)
            upperBound = 1;
    }
*/

    for (int n = 0; n < upperBound && d->running; n++)
    {
        Frame* const frm = getFrame(d->item, img, imgNext, n, action);
        ProcessFrame(frm);
        WriteFrame(frm);
        delete frm;
    }
}

void ActionThread::cancel()
{
    d->running = false;
    d->encoder->cancel();
    cleanTempDir();
}

void ActionThread::doPreProcessing(ASPECTCORRECTION_TYPE type, ASPECT_RATIO aspectRatio,int frameWidth, int frameHeight,
                                   const QString& path, MyImageListViewItem* const item, VIDEO_FORMAT format,
                                   VIDEO_TYPE videotype, const QString& audioPath, const QString& savePath)
{
    d->aspectcorrection = type;
    d->aspectRatio      = aspectRatio;
    d->frameHeight      = frameHeight;
    d->frameWidth       = frameWidth;
    d->item             = item;
    d->number           = 0;
    d->audioPath        = audioPath;
    d->videoFormat      = format;
    d->videoType        = videotype;

    switch (d->videoFormat)
    {
        case VIDEO_FORMAT_NTSC:
            d->framerate = 30;
            break;

        case VIDEO_FORMAT_SECAM:
        case VIDEO_FORMAT_PAL:
        default:
            d->framerate = 25;
            break;
    };

    if (!d->api)
    {
        d->api        = new MagickApi(path);
        d->processImg = new ProcessImage(d->api);

        connect(d->api, SIGNAL(signalsAPIError(QString)),
                this, SIGNAL(signalProcessError(QString)));

        connect(d->processImg, SIGNAL(signalProcessError(QString)),
                this, SIGNAL(signalProcessError(QString)));
    }

    if (!d->encoder)
    {
        d->encoder = new EncoderDecoder();

        connect(d->encoder,SIGNAL(encoderError(QString)),
                this, SIGNAL(signalProcessError(QString)));
    }

    d->dir.setPath(path);
    d->dir.mkdir("vss");
    d->path     = path + QDir::separator() + "vss";
    d->savePath = savePath;
}

MagickImage* ActionThread::loadImage(MyImageListViewItem* const imgItem) const
{
    MagickImage* img = 0;

    if (KPMetadata::isRawFile(imgItem->url()))
    {
        QImage image;
        KDcrawIface::KDcraw::loadEmbeddedPreview(image, imgItem->url().path());

        if (!(img = d->api->loadQImage(image)))
           return 0;
    }
    else if (!(img = d->api->loadImage(imgItem->url().path())))
    {
        return 0;
    }

    double ratio = (double)d->frameWidth/d->frameHeight;

    switch (d->aspectRatio)
    {
        case ASPECT_RATIO_4_3:
            ratio = (double)4/3;
            break;
        case ASPECT_RATIO_16_9:
            ratio = (double)16/9;
            break;
        default:
            break;
    };

    if (!(img = d->processImg->aspectRatioCorrection(*img, ratio, d->aspectcorrection)))
    {
        return 0;
    }

    if (d->api->scaleImage(*img, d->frameWidth, d->frameHeight) != 1)
    {
        return 0;
    }

    return img;
}

ActionThread::Frame* ActionThread::getFrame(MyImageListViewItem* const item, MagickImage* const img, MagickImage* const imgNext,
                                            int number, Action action) const
{
    Frame* const frame = new Frame();
    frame->item        = item;
    frame->img         = img;
    frame->imgnext     = imgNext;
    frame->action      = action;
    frame->number      = number;

    return frame;
}

MagickImage* ActionThread::getDynamicImage(MyImageListViewItem* const imgItem, MagickImage* const img, int step) const
{
    MagickImage* imgout = 0;
    int steps           = imgItem->getTime() * d->framerate + getTransitionFrames(imgItem->getPrevImageItem()) +
                                                              getTransitionFrames(imgItem->getNextImageItem());

    switch (imgItem->EffectName())
    {
        case EFFECT_KENBURN:
        {
            GeoImage fromGeo(0, 0, img->getWidth(), img->getHeight());
            GeoImage toGeo(0, 0, img->getWidth() * 0.8, img->getHeight() * 0.8);
            GeoImage* currentGeo = d->processImg->getGeometry(fromGeo, toGeo, img->getWidth(),
                                                              img->getHeight(), step, steps);

            imgout               = d->api->geoscaleImage(*img, currentGeo->x, currentGeo->y, currentGeo->w,
                                                         currentGeo->h, d->frameWidth, d->frameHeight);
            delete(currentGeo);
        }
        break;

        case EFFECT_NONE:
            return imgout;
        break;
    }

    return imgout;
}

int ActionThread::getTransitionFrames(MyImageListViewItem* const item) const
{
    if(item == 0 || item->getTransition() == TRANSITION_TYPE_NONE)
    {
        return 0;
    }

    int noOfFrames = 0;

    switch(item->getTransitionSpeed())
    {
        case TRANSITION_SLOW:
            noOfFrames = 2 * d->framerate;
            break;
        case TRANSITION_MEDIUM:
            noOfFrames = 1 * d->framerate;
            break;
        case TRANSITION_FAST:
            noOfFrames = d->framerate / 2;
            break;
    }

    return noOfFrames;
}

void ActionThread::ProcessFrame(Frame* const frm)
{
    if (!frm)
    {
        kDebug() << "Frame to process is null";
        return;
    }

    switch (frm->action)
    {
        case TYPE_TRANSITION:
        {
            MagickImage* transImg   = 0;
            MagickImage* imgout     = 0;
            MagickImage* imgnextout = 0;

            // we may have to geoscale the current and next image
            int step   = getTransitionFrames(frm->item->getPrevImageItem()) + frm->item->getPrevImageItem()->getTime() * 
                                             d->framerate + frm->number;
            imgout     = getDynamicImage(frm->item->getPrevImageItem(), frm->img, step);
            imgnextout = getDynamicImage(frm->item, frm->imgnext, frm->number);

            transImg   = d->processImg->transition(imgout ? *imgout : *frm->img,
                                                   imgnextout ? *imgnextout : *frm->imgnext,
                                                   frm->item->getTransition(), frm->number, getTransitionFrames(frm->item));

            if (imgout)
            {
                d->api->freeImage(*imgout);
            }

            if (imgnextout)
            {
                d->api->freeImage(*imgnextout);
            }

            frm->imgout = transImg;
            break;
        }

        case TYPE_IMAGE:
        {
            int step    = getTransitionFrames(frm->item) + frm->number;
            frm->imgout = getDynamicImage(frm->item, frm->img, step);
            break;
        }

        default:
            break;
    }
}

void ActionThread::WriteFrame(Frame* const frame)
{
    QString path = QString("%1" + QDir::separator() + "tempvss%2.ppm").arg(d->path).arg(QString::number(d->number));
    d->api->saveToFile(frame->imgout ? *(frame->imgout) : *(frame->img), path);
    ++d->number;
}

int ActionThread::getTotalFrames(MyImageListViewItem* const item) const
{
    MyImageListViewItem* pi = item;
    int total_frames        = 0;

    while (pi)
    {
        total_frames += getTransitionFrames(pi);
        total_frames += pi->getTime() * d->framerate;
        pi           =  pi->getNextImageItem();
    }

    return total_frames;
}

} // namespace KIPIVideoSlideShowPlugin
