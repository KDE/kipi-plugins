/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include "kptooldialog.h"

using namespace KIPIPlugins;

namespace KIPIAdvancedSlideshowPlugin
{

class SharedContainer;

class SlideShowConfig : public KPPageDialog
{
    Q_OBJECT

public:

    SlideShowConfig(QWidget* const parent, SharedContainer* const sharedData);
    ~SlideShowConfig();

Q_SIGNALS:

     // Signal needed by plugin_slideshow class
    void buttonStartClicked();

private:

    void readSettings();
    void saveSettings();

private Q_SLOTS:

    void slotStartClicked();
    void slotClose();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif //SLIDESHOWCONFIG_H
