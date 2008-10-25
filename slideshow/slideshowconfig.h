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

// Local includes.
#include "common.h"

namespace KIPISlideShowPlugin
{

class SharedData;

class SlideShowConfigPrivate;

class SlideShowConfig : public KPageDialog
{
    Q_OBJECT

public:
    SlideShowConfig (QWidget *parent, SharedData* sharedData);
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

} // namespace KIPISlideShowPlugin

#endif //SLIDESHOWCONFIG_H
