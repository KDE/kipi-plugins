/* ============================================================
 * File  : screenproperties.cpp
 * Author: Valerio Fuoglio <valerio.fuoglio@gmail.com>
 * Date  : 2007-11-14
 * Description : 
 * 
 * Copyright 2007 by Valerio Fuoglio <valerio.fuoglio@gmail.com>
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

/***************************************************************************
 *   Parts of this code are based on smoothslidesaver by Carsten Weinhold  *
 *   <carsten.weinhold@gmx.de>                                             *
 **************************************************************************/

#include <qapplication.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "screenproperties.h"

namespace KIPISlideShowPlugin
{

  ScreenProperties::ScreenProperties(QWidget *mainWidget) {
  
      activeScreen = QApplication::desktop()->screenNumber(mainWidget);
  }
  
  
  unsigned ScreenProperties::suggestFrameRate() {
  
      int eventBase, errorBase;
      if ( !XRRQueryExtension(qt_xdisplay(), &eventBase, &errorBase)) {
          // No information, make a lucky guess on based on that ;)
          return 25;
      }
  
      // ask X11 for the refresh rate of the current screen
      XRRScreenConfiguration* config;
      int screenRate;
  
      config        = XRRGetScreenInfo(qt_xdisplay(), RootWindow(qt_xdisplay(),
                                      activeScreen));
      screenRate    = XRRConfigCurrentRate(config);
      XRRFreeScreenConfigInfo(config);
      //qDebug("monitor refresh rate %d Hz", screenRate);
  
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
      int bestRate = candidateRate[0];
      int smallestError = 1000, i = 0;
      do {
          int r     = candidateRate[i];
          int error = QMIN(screenRate % r, (screenRate + r) % r);
          
          if (error < smallestError) {
              smallestError = error;
              bestRate      = r;
          }
      } while (++i < 3);
  
      //qDebug("using %d Hz as framerate for effects", bestRate);
      return bestRate;
  }
  
  
  bool ScreenProperties::enableVSync() {
  
      // currently only supported on NVidia hardware using the
      // proprietary driver
  
      // For NVidia graphics cards: always use sync-to-vblank
    //  return (setenv("__GL_SYNC_TO_VBLANK", "1", 1) == 0);
  }

}  // NameSpace KIPISlideShowPlugin
