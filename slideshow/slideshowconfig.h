/* ============================================================
 * File  : slideshowconfig.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-17
 * Description : Digikam slideshow plugin.
 * 
 * Copyright 2003-2004 by Renchi Raju
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

#ifndef SLIDESHOWCONFIG_H
#define SLIDESHOWCONFIG_H

// Qt includes.

#include <qstring.h>

// SlideShow includes

#include "slideshowconfigbase.h"


namespace KIPISlideShowPlugin
{

class SlideShowConfig : public SlideShowConfigBase
{
    Q_OBJECT

public:

    SlideShowConfig(bool allowSelectedOnly, QWidget *parent, const char* name);
    ~SlideShowConfig();

private:

    void loadEffectNames();
    void loadEffectNamesGL();
    void readSettings();
    void saveSettings();

private slots:

    void slotOkClicked();
    void slotHelp();
    void slotOpenGLToggled();

signals:
    void okButtonClicked(); // Signal needed by plugin_slideshow class

};

}  // NameSpace KIPISlideShowPlugin

#endif
