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

// Local includes

#include "processimage.h"
#include "magickiface.h"

#include <QDir>

using namespace KIPIPlugins;

namespace KIPIVideoSlideShowPlugin
{
  
class ActionThread::Private
{
  
public:

    Private()
    {
        api        = 0;
        processImg = 0;
        framerate  = 25;
        number     = 0;
    }
    
    MagickApi*            api;
    ProcessImage*         processImg;
    int                   framerate;
    ASPECTCORRECTION_TYPE aspectcorrection;
    int                   frameWidth;
    int                   frameHeight;
    int                   number;
    QString               path;
    MyImageListViewItem*  item;
    
    bool                  running;
};

ActionThread::ActionThread()
    : d(new Private)
{
    d->running    = true;
}

ActionThread::~ActionThread()
{
    delete d;
}

void ActionThread::run()
{  
    MagickImage *img = NULL, *imgnext = NULL;
    
    // have to keep dummy items at first and last
    imgnext = loadImage(*d->item);
    int upperBound = 0;
 
    while(d->item->getNextImageItem() && d->running)
    {
        if(img)
            d->api->freeImage(*img);
        
        img = imgnext;
        d->item = d->item->getNextImageItem();
        imgnext = loadImage(*d->item);

        upperBound = d->item->getTime() * d->framerate;
        processItem(upperBound, *img, *imgnext, TYPE_IMAGE);
        
        ActionData ad;
        ad.action      = TYPE_IMAGE;
        ad.fileUrl     = d->item->getPrevImageItem()->url();
        ad.totalFrames = upperBound;
        emit frameCompleted(ad);

        upperBound = getTransitionFrames(d->item);
        processItem(upperBound, *img, *imgnext, TYPE_TRANSITION); 
        
        ActionData tad;
        tad.action      = TYPE_TRANSITION;
        tad.fileUrl     = d->item->url();
        tad.totalFrames = upperBound;
        emit frameCompleted(tad);
    }
    
    if(img)
        d->api->freeImage(*img);
    img = imgnext;

    upperBound = d->item->getTime() * d->framerate;
    processItem(upperBound, *img, *imgnext, TYPE_IMAGE);
     
    ActionData ad;
    ad.action      = TYPE_IMAGE;
    ad.fileUrl     = d->item->url();
    ad.totalFrames = upperBound;
    emit frameCompleted(ad);
      
    if(img)
        d->api->freeImage(*img);
        
    emit finished();
}

void ActionThread::processItem(int upperBound, MagickImage& img, MagickImage& imgNext, Action action)
{
    if(action == TYPE_IMAGE)
    {
        if(d->item->EffectName() == EFFECT_NONE)
            upperBound = 1;
    }
    
    for(int n = 0; n < upperBound && d->running; n++)
    {
        Frame* frm = getFrame(*d->item, img, imgNext, n, action);
        ProcessFrame(*frm);
        WriteFrame(*frm);
        delete frm;
    }
}

void ActionThread::cancel()
{
    d->running = false;
}


void ActionThread::doPreProcessing(int framerate, ASPECTCORRECTION_TYPE type, int frameWidth, int frameHeight, QString& path, MyImageListViewItem& item)
{
    d->framerate        = framerate;
    d->aspectcorrection = type;
    d->frameHeight      = frameHeight;
    d->frameWidth       = frameWidth;
    d->item             = &item;
    d->number           = 0;
    
    if(!d->api)
    {
        d->api        = new MagickApi(path);
        d->processImg = new ProcessImage(d->api);

        connect(d->api, SIGNAL(signalsAPIError(QString)),
                this, SIGNAL(signalProcessError(QString)));

        connect(d->processImg, SIGNAL(signalProcessError(QString)),
                this, SIGNAL(signalProcessError(QString)));
    }
    
    d->path = path;
}


MagickImage* ActionThread::loadImage(MyImageListViewItem& imgItem)
{
    MagickImage* img = NULL;
        
    if(!(img = d->api->loadImage(imgItem.url().path())))
        return 0;
    
    if(!(img = d->processImg->aspectRatioCorrection(*img, (double)d->frameHeight/d->frameWidth, d->aspectcorrection)))
        return 0;
    
    if(d->api->scaleImage(*img, d->frameWidth, d->frameHeight) != 1)
        return 0;
    
    return img;
}

ActionThread::Frame* ActionThread::getFrame(MyImageListViewItem& item, MagickImage& img, MagickImage& imgNext, int number, Action action)
{
    Frame* frame = new Frame();
    
    frame->item    = &item;
    frame->img     = &img;
    frame->imgnext = &imgNext;
    frame->action  = action;
    frame->number  = number;
    
    return frame;
}

MagickImage* ActionThread::getDynamicImage(MyImageListViewItem& imgItem, MagickImage &img, int step)
{
    MagickImage* imgout = NULL;
          
    int steps = imgItem.getTime() * d->framerate + getTransitionFrames(imgItem.getPrevImageItem())
                       + getTransitionFrames(imgItem.getNextImageItem());
    
    switch(imgItem.EffectName())
    {
        case EFFECT_KENBURN:
        {  
            GeoImage fromGeo(0, 0, img.getWidth(), img.getHeight());
            GeoImage toGeo(0, 0, img.getWidth() * 0.8, img.getHeight() * 0.8);
            GeoImage *currentGeo = d->processImg->getGeometry(fromGeo, toGeo, img.getWidth(),
                img.getHeight(), step, steps);

            imgout = d->api->geoscaleImage(img, currentGeo->x, currentGeo->y, currentGeo->w, 
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

int ActionThread::getTransitionFrames(MyImageListViewItem* item)
{
    if(item == NULL || item->getTransition() == TRANSITION_TYPE_NONE)
        return 0;

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

void ActionThread::ProcessFrame(Frame& frm)
{
    switch(frm.action)
    {
        case TYPE_TRANSITION:
        {
            MagickImage* transImg;
            MagickImage* imgout = NULL;
            MagickImage* imgnextout = NULL;
            
            // we may have to geoscale the current and next image
            int step   = getTransitionFrames(frm.item->getPrevImageItem()) + frm.item->getPrevImageItem()->getTime() * d->framerate
                           + frm.number;
            imgout     = getDynamicImage(*frm.item->getPrevImageItem(), *frm.img, step);
            imgnextout = getDynamicImage(*frm.item, *frm.imgnext, frm.number);

            transImg   = d->processImg->transition(imgout ? *imgout : *frm.img,
                           imgnextout ? *imgnextout : *frm.imgnext,
                           frm.item->getTransition(), frm.number, getTransitionFrames(frm.item));
            
            if(imgout)
                d->api->freeImage(*imgout);
            if(imgnextout)
                d->api->freeImage(*imgnextout);
            frm.imgout = transImg;
            break;
        }
        case TYPE_IMAGE:
        {
            int step   = getTransitionFrames(frm.item) + frm.number;
            frm.imgout = getDynamicImage(*frm.item, *frm.img, step);
            break;
        }
        default:
            break;
    }
}

void ActionThread::WriteFrame(ActionThread::Frame& frame)
{
    ++d->number;
    d->api->saveToFile(frame.imgout ? *frame.imgout : *frame.img, d->path + QString("/tempvss%1.ppm").arg(QString::number(d->number)));
}

int ActionThread::getTotalFrames(MyImageListViewItem* item)
{
    int total_frames = 0;
    
    while(item)
    {
        total_frames += getTransitionFrames(item);
        total_frames += item->getTime() * d->framerate;
        item = item->getNextImageItem();
    }
    
    return total_frames;
}


} // namespace KIPIVideoSlideShowPlugin
