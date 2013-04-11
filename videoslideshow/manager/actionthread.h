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
#include "encoderdecoder.h"

using namespace KIPIPlugins;

namespace KIPIVideoSlideShowPlugin
{
 
class ActionThread : public QThread
{
    Q_OBJECT

public:

    explicit ActionThread(QObject* const parent);
    ~ActionThread();

    void doPreProcessing(ASPECTCORRECTION_TYPE type, ASPECT_RATIO ratio, int frameWidth, int frameHeight,
                         const QString& path, MyImageListViewItem* const item, VIDEO_FORMAT format,
                         VIDEO_TYPE videeotype, const QString& audioPath, const QString& savePath);
    int  getTotalFrames(MyImageListViewItem* const item) const;

    void cancel();

Q_SIGNALS:

    void signalProcessError(const QString& errMess);
    void frameCompleted(const KIPIVideoSlideShowPlugin::ActionData& ad);
    void finished();

private:

    struct Frame
    {
        Action               action;
        int                  number;

        MyImageListViewItem* item;

        MagickImage*         img;
        MagickImage*         imgnext;
        MagickImage*         imgout; 
    };

private:

    void run();

    int  getTransitionFrames(MyImageListViewItem* const item) const;
    void processItem(int upperBound, MagickImage* const img, MagickImage* const imgNext, Action action);

    MagickImage* getDynamicImage(MyImageListViewItem* const imgItem, MagickImage* const img, int step) const;
    MagickImage* loadImage(MyImageListViewItem* const img) const;


    void ProcessFrame(Frame* const frame);
    void WriteFrame(Frame* const frame);

    Frame* getFrame(MyImageListViewItem* const item, MagickImage* const img, MagickImage* const imgNext,
                    int number, Action action) const;

    void cleanTempDir();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIVideoSlideShowPlugin

#endif // ACTIONTHREAD_H

