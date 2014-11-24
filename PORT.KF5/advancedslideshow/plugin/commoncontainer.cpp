/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-10-02
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

// Local includes

#include "commoncontainer.h"

namespace KIPIAdvancedSlideshowPlugin
{

SharedContainer::SharedContainer()
{
    setIface(0);

    delayMsMaxValue               = 0;
    delayMsMinValue               = 0;
    delayMsLineStep               = 0;
    ImagesHasComments             = false;
    urlList                       = KUrl::List();
    mainPage                      = 0;
    captionPage                   = 0;
    advancedPage                  = 0;
    soundtrackPage                = 0;

    opengl                        = false;
    openGlFullScale               = false;
    delay                         = 0;
    printFileName                 = false;
    printProgress                 = false;
    printFileComments             = false;
    loop                          = false;
    shuffle                       = false;
    showSelectedFilesOnly         = false;
    commentsFontColor             = 0;
    commentsBgColor               = 0;
    commentsDrawOutline           = false;
    bgOpacity                     = 10;
    commentsLinesLength           = 0;
    captionFont                   = 0;

    soundtrackLoop                = false;
    soundtrackRememberPlaylist    = false;
    soundtrackPlayListNeedsUpdate = false;

    useMilliseconds               = false;
    enableMouseWheel              = false;
    enableCache                   = false;
    kbDisableFadeInOut            = false;
    kbDisableCrossFade            = false;
    cacheSize                     = 0;

    page_main                     = 0;
    page_caption                  = 0;
    page_soundtrack               = 0;
    page_advanced                 = 0;
}

SharedContainer::~SharedContainer()
{
    delete captionFont;
}

KIPI::Interface* SharedContainer::iface() const
{
    return kipiIface;
}

void SharedContainer::setIface(KIPI::Interface* const iface)
{
    kipiIface = iface;
}

// -------------------------------------------------------------

VolumeSlider::VolumeSlider(QWidget* const parent)
            : Phonon::VolumeSlider(parent)
{
}

} // namespace KIPIAdvancedSlideshowPlugin
