/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-09
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

#ifndef SLIDESHOWCONFIG_H
#define SLIDESHOWCONFIG_H

// QT includes
#include <QString>
#include <QFont>


// KDE includes
#include <kpagedialog.h>
#include <kpagewidgetmodel.h>
#include <kurl.h>
#include <kdebug.h>

 
// libkipi includes
#include <libkipi/interface.h>

// Local includes
#include "maindialog.h"
#include "captiondialog.h"
#include "advanceddialog.h"

namespace KIPISlideShowPlugin
{

class MainDialog;
class CaptionDialog;
class AdvancedDialog;

class SharedData
{
  public:
    SharedData() {
      interface = 0;
      delayMsMaxValue = 0;
      delayMsMinValue = 0;
      delayMsLineStep = 0;
      ImagesHasComments = false;
      urlList = 0;
      mainPage = 0;
      captionPage = 0;
      advancedPage = 0;

      opengl = false;
      delay = 0;
      printFileName = false;
      printProgress = false;
      printFileComments = false;
      loop = false;
      shuffle = false;
      showSelectedFilesOnly = false;
   // QString          effectName;
   // QString          effectNameGL;
      commentsFontColor = 0;
      commentsBgColor = 0;
      transparentBg = false;
      commentsLinesLength = 0;
      captionFont = 0;

      useMilliseconds = false;
      enableMouseWheel = false;
      enableCache = false; 
      kbDisableFadeInOut = false;
      kbDisableCrossFade = false;
      cacheSize = 0;
    }

  public:

    KIPI::Interface*     interface;

    int                  delayMsMaxValue;
    int                  delayMsMinValue;
    int                  delayMsLineStep;

    bool                 ImagesHasComments;
    KUrl::List*          urlList;

    MainDialog*          mainPage;
    CaptionDialog*       captionPage;
    AdvancedDialog*      advancedPage;

    KPageWidgetItem*     page_main;
    KPageWidgetItem*     page_caption;
    KPageWidgetItem*     page_advanced;

    // Config file data
    // --> Main page
    bool             opengl;
    int              delay;
    bool             printFileName;
    bool             printProgress;
    bool             printFileComments;
    bool             loop;
    bool             shuffle;
    bool             showSelectedFilesOnly;
    QString          effectName;
    QString          effectNameGL;
    // --> Captions page
    uint             commentsFontColor;
    uint             commentsBgColor;
    bool             transparentBg;
    int              commentsLinesLength;
    QFont*           captionFont;
    // --> Advanced page
    bool             useMilliseconds;
    bool             enableMouseWheel;
    bool             enableCache; 
    bool             kbDisableFadeInOut;
    bool             kbDisableCrossFade;
    uint             cacheSize;

};

class SlideShowConfigPrivate;

class SlideShowConfig : public KPageDialog
{
    Q_OBJECT

  public: 
    SlideShowConfig (bool allowSelectedOnly, KIPI::Interface* interface,
                      QWidget *parent, bool ImagesHasComments,
                      KUrl::List* urlList);
    ~SlideShowConfig();

  private:
    void readSettings();
    void saveSettings();
  
  private slots:
    void slotStartClicked();
    void slotHelp();
    void slotClose();

  signals:
    void buttonStartClicked(); // Signal needed by plugin_slideshow class

  private:
    SlideShowConfigPrivate* d;
};

} // NameSpace KIPISlideShowPlugin

#endif //SLIDESHOWCONFIG_H
