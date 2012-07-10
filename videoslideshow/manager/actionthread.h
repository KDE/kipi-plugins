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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes

#include <QObject>
#include <QThread>
#include <QFile>

// Local includes

#include "processimage.h"
#include "myimagelist.h"
#include "actions.h"

using namespace KIPIPlugins;

namespace KIPIVideoSlideShowPlugin
{
 
class ActionThread : public QThread
{
    Q_OBJECT

public:

    ActionThread();
    ~ActionThread();
        
    void doPreProcessing(int framerate, ASPECTCORRECTION_TYPE type, int frameWidth, int frameHeight, QString& path, MyImageListViewItem& item);
    int getTotalFrames(MyImageListViewItem* item);
    
    void cancel();
           
Q_SIGNALS:

    void signalProcessError(const QString& errMess);
    void frameCompleted(const ActionData& ad);
    void finished();
    
private:
  
    void run();
    
    int getTransitionFrames(MyImageListViewItem* item);
    void processItem(int upperBound, MagickImage& img, MagickImage& imgNext, Action action);
    
    MagickImage* getDynamicImage(MyImageListViewItem& imgItem, MagickImage& img, int step);
    MagickImage* loadImage(MyImageListViewItem& img);
    
    struct Frame {
        Action               action;
        int                  number;
    
        MyImageListViewItem* item;
    
        MagickImage*         img;
        MagickImage*         imgnext;
        MagickImage*         imgout; 
    };
    
    void ProcessFrame(Frame& frame);
    void WriteFrame(Frame& frame);
    
    Frame* getFrame(MyImageListViewItem& item, MagickImage& img, MagickImage& imgNext, int number, Action action);    

private:

    class Private;
    Private* const d;
    
};

} // namespace KIPIVideoSlideShowPlugin

#endif // ACTIONTHREAD_H