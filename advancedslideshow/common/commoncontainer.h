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

#ifndef COMMONCONTAINER_H
#define COMMONCONTAINER_H

// Qt includes

#include <QString>
#include <QFont>
#include <QUrl>

// libkipi includes

#include <KIPI/Interface>

namespace KIPIAdvancedSlideshowPlugin
{

class MainDialog;
class CaptionPage;
class AdvancedPage;

#ifdef HAVE_AUDIO
class SoundtrackDialog;
#endif

class SharedContainer
{

public:

    SharedContainer();
    ~SharedContainer();

    KIPI::Interface* iface() const;
    void             setIface(KIPI::Interface* const iface);

public:

    int                  delayMsMaxValue;
    int                  delayMsMinValue;
    int                  delayMsLineStep;

    bool                 ImagesHasComments;
    QList<QUrl>          urlList;

    MainDialog*          mainPage;
    CaptionPage*       captionPage;
#ifdef HAVE_AUDIO
    SoundtrackDialog*    soundtrackPage;
#endif
    AdvancedPage*      advancedPage;

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
    bool                 commentsDrawOutline;
    uint                 bgOpacity;
    int                  commentsLinesLength;
    QFont*               captionFont;

    // --> Soundtrack page
    bool                 soundtrackLoop;
    bool                 soundtrackRememberPlaylist;
    bool                 soundtrackPlayListNeedsUpdate;
    QUrl                 soundtrackPath;
    QList<QUrl>          soundtrackUrls;

    // --> Advanced page
    bool                 useMilliseconds;
    bool                 enableMouseWheel;
    bool                 enableCache;
    bool                 kbDisableFadeInOut;
    bool                 kbDisableCrossFade;
    uint                 cacheSize;

private :

    KIPI::Interface*     kipiIface;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif // COMMONCONTAINER_H
