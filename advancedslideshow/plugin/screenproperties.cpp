/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007-2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * Parts of this code are based on smoothslidesaver by Carsten Weinhold
 * <carsten dot weinhold at gmx dot de>
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

#include "screenproperties.h"

// Qt includes

#include <QApplication>
#include <QDesktopWidget>

#ifdef Q_WS_X11
#include <QX11Info>

// X11 includes

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <fixx11h.h>
#endif // Q_WS_X11

namespace KIPIAdvancedSlideshowPlugin
{

ScreenProperties::ScreenProperties(QWidget* const mainWidget)
{
    activeScreen = QApplication::desktop()->screenNumber(mainWidget);
}

unsigned ScreenProperties::suggestFrameRate()
{
// use XRandR only in X11
#ifndef Q_WS_X11
    return 25;
#else
    int eventBase, errorBase;

    if ( !XRRQueryExtension(QX11Info::display(), &eventBase, &errorBase))
    {
        // No information, make a lucky guess on based on that ;)
        return 25;
    }

    // ask X11 for the refresh rate of the current screen
    XRRScreenConfiguration* config = 0;

    int screenRate;

    config     = XRRGetScreenInfo(QX11Info::display(), RootWindow(QX11Info::display(), activeScreen));
    screenRate = XRRConfigCurrentRate(config);

    XRRFreeScreenConfigInfo(config);

    //kDebug() << "monitor refresh rate " << screenRate << " Hz";

    // Find the frame rate, that matches the monitor's refresh rate best.
    // We will choose between 25, 28 and 30 Hz, to get smooth animations.
    // The following frame rate will be chosen according to the monitor's
    // refresh rate:
    //
    // Frame rate:   Monitor refresh rate
    // 25 Hz         (50), 75, 100 Hz (PAL compliant setups)
    // 28 Hz         85 Hz, 110 Hz    (seems to work ok)
    // 30 Hz         60, 90, 120 Hz   (NTSC compliant setups)
    //
    // However, this will only work, if the kernel can schedule the
    // screensaver at the right time (a 2.6.x kernel should work fine,
    // because of the high HZ value).
    int candidateRate[3] = { 30, 25, 28 };

    int bestRate         = candidateRate[0];
    int smallestError    = 1000, i = 0;

    do
    {
        int r     = candidateRate[i];
        int error = qMin(screenRate % r, (screenRate + r) % r);

        if (error < smallestError)
        {
            smallestError = error;
            bestRate      = r;
        }
    }
    while (++i < 3);

    //kDebug() << "using " << bestRate << " Hz as framerate for effects";
    return bestRate;

#endif // Q_WS_X11
}

bool ScreenProperties::enableVSync()
{
    // currently only supported on NVidia hardware using the
    // proprietary driver

    // For NVidia graphics cards: always use sync-to-vblank
    //  return (setenv("__GL_SYNC_TO_VBLANK", "1", 1) == 0);

    return false; // To please compiler.
}

}  // namespace KIPIAdvancedSlideshowPlugin
