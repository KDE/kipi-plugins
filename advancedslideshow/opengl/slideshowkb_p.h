/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Parts of this code are based on
 * smoothslidesaver by Carsten Weinhold <carsten dot weinhold at gmx dot de>
 * and slideshowgl by Renchi Raju <renchi dot raju at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

namespace KIPIAdvancedSlideshowPlugin
{

class KBImageLoader;
class ScreenProperties;
class PlaybackWidget;

class SlideShowKB::Private
{

public:

    Private()
    {
        zoomIn              = qrand() < RAND_MAX / 2;
        effect              = 0;
        initialized         = false;
        haveImages          = true;
        endOfShow           = false;
        showingEnd          = false;
        deskX               = 0;
        deskY               = 0;
        deskWidth           = 0;
        deskHeight          = 0;
        imageLoadThread     = 0;
        mouseMoveTimer      = 0;
        timer               = 0;
        image[0]            = 0;
        image[1]            = 0;
        numKBEffectRepeated = 0;
        step                = 0.0;
        delay               = 0;
        disableFadeInOut    = false;
        disableCrossFade    = false;
        forceFrameRate      = 0;
        sharedData          = 0;
        playbackWidget      = 0;
    }

    int                 deskX;
    int                 deskY;
    int                 deskWidth;
    int                 deskHeight;

    QStringList         commentsList;

    KBImageLoader*      imageLoadThread;
    QTimer*             mouseMoveTimer;
    QTimer*             timer;
    bool                haveImages;

    Image*              image[2];
    KBEffect*           effect;
    int                 numKBEffectRepeated;
    bool                zoomIn;
    bool                initialized;
    float               step;

    bool                endOfShow;
    bool                showingEnd;

    int                 delay;
    bool                disableFadeInOut;
    bool                disableCrossFade;
    unsigned            forceFrameRate;

    PresentationContainer*    sharedData;

    PlaybackWidget*     playbackWidget;
};

}  // namespace KIPIAdvancedSlideshowPlugin
