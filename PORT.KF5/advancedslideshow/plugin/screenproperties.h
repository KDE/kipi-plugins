/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007-2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * Parts of this code are based on smoothslidesaver by Carsten Weinhold
 * <carsten dot weinhold at gmx dot de>
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

#ifndef SCREENPROPERTIES_H
#define SCREENPROPERTIES_H

class QWidget;

namespace KIPIAdvancedSlideshowPlugin
{

class ScreenProperties
{

public:

    explicit ScreenProperties(QWidget* const mainWidget);
    virtual ~ScreenProperties()
    {
    };

    virtual unsigned suggestFrameRate();
    virtual bool     enableVSync();

protected:

    unsigned activeScreen;
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif // SCREENPROPERTIES_H
