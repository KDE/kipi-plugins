/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#ifndef COMMON_H
#define COMMON_H

// Qt includes

#include <QString>
#include <QFont>

// KDE includes

#include <kurl.h>
#include <kpagedialog.h>
#include <kpagewidgetmodel.h>

// Phonon includes

#include <Phonon/VolumeSlider>

// Local includes

#include "maindialog.h"
#include "captiondialog.h"
#include "soundtrackdialog.h"
#include "advanceddialog.h"
#include "playbackwidget.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIAdvancedSlideshowPlugin
{

class MainDialog;
class CaptionDialog;
class SoundtrackDialog;
class AdvancedDialog;
class SharedData
{

public:

    SharedData();
    ~SharedData();

public:

    KIPI::Interface*     interface;

    int                  delayMsMaxValue;
    int                  delayMsMinValue;
    int                  delayMsLineStep;

    bool                 ImagesHasComments;
    KUrl::List           urlList;

    MainDialog*          mainPage;
    CaptionDialog*       captionPage;
    SoundtrackDialog*    soundtrackPage;
    AdvancedDialog*      advancedPage;

    KPageWidgetItem*     page_main;
    KPageWidgetItem*     page_caption;
    KPageWidgetItem*     page_soundtrack;
    KPageWidgetItem*     page_advanced;

    // Config file data
    // --> Main page
    bool                 opengl;
    bool                 openGlFullScale;
    int                  delay;
    bool                 printFileName;
    bool                 printProgress;
    bool                 printFileComments;
    bool                 loop;
    bool                 shuffle;
    bool                 showSelectedFilesOnly;
    QString              effectName;
    QString              effectNameGL;
    // --> Captions page
    uint                 commentsFontColor;
    uint                 commentsBgColor;
    bool                 transparentBg;
    int                  commentsLinesLength;
    QFont*               captionFont;
    // --> Soundtrack page
    bool                 soundtrackLoop;
    bool                 soundtrackRememberPlaylist;
    bool                 soundtrackPlayListNeedsUpdate;
    KUrl                 soundtrackPath;
    KUrl::List           soundtrackUrls;

    // --> Advanced page
    bool                 useMilliseconds;
    bool                 enableMouseWheel;
    bool                 enableCache;
    bool                 kbDisableFadeInOut;
    bool                 kbDisableCrossFade;
    uint                 cacheSize;
};

// ----------------------------------------------------------

class VolumeSlider : public Phonon::VolumeSlider
{

public:

    VolumeSlider(QWidget* parent);
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif // COMMON_H
