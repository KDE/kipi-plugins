/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at gmx dot net>
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

// KDE includes.

#include <kpagedialog.h>

namespace KIPIAdvancedSlideshowPlugin
{

class SharedContainer;

class SlideShowConfigPrivate;

class SlideShowConfig : public KPageDialog
{
    Q_OBJECT

public:

    SlideShowConfig (QWidget *parent, SharedContainer* sharedData);
    ~SlideShowConfig();

Q_SIGNALS:

    void buttonStartClicked(); // Signal needed by plugin_slideshow class

private:

    void readSettings();
    void saveSettings();

private Q_SLOTS:

    void slotStartClicked();
    void slotHelp();
    void slotClose();

private:

    SlideShowConfigPrivate* const d;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif //SLIDESHOWCONFIG_H
