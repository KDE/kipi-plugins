/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-02
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

// local includes
#include "common.h"
 
namespace KIPISlideShowPlugin
{

SharedData::SharedData() 
{
    interface = 0;
    delayMsMaxValue = 0;
    delayMsMinValue = 0;
    delayMsLineStep = 0;
    ImagesHasComments = false;
    urlList = 0;
    mainPage = 0;
    captionPage = 0;
    advancedPage = 0;
    soundtrackPage = 0;

    opengl = false;
    delay = 0;
    printFileName = false;
    printProgress = false;
    printFileComments = false;
    loop = false;
    shuffle = false;
    showSelectedFilesOnly = false;
    commentsFontColor = 0;
    commentsBgColor = 0;
    transparentBg = false;
    commentsLinesLength = 0;
    captionFont = 0;

    soundtrackLoop = false;

    useMilliseconds = false;
    enableMouseWheel = false;
    enableCache = false; 
    kbDisableFadeInOut = false;
    kbDisableCrossFade = false;
    cacheSize = 0;

    page_main = 0;
    page_caption = 0;
    page_soundtrack = 0;
    page_advanced = 0;
}

SharedData::~SharedData() 
{
    if ( mainPage ) delete mainPage;
    if ( captionPage ) delete captionPage;
    if ( soundtrackPage ) delete soundtrackPage;
    if ( advancedPage ) delete advancedPage;

    if ( page_main ) delete page_main;
    if ( page_caption ) delete page_caption;
    if ( page_soundtrack ) delete page_soundtrack;
    if ( page_advanced ) delete page_advanced;

    if ( captionFont ) delete captionFont;
}


VolumeSlider::VolumeSlider(QWidget* parent)
            : Phonon::VolumeSlider(parent) 
{
}

}
