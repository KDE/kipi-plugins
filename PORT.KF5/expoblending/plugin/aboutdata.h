/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ABOUTDATA_H
#define ABOUTDATA_H

// Local includes

#include "kpaboutdata.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class ExpoBlendingAboutData : public KPAboutData
{

public:

    ExpoBlendingAboutData()
        : KPAboutData(ki18n("Exposure Blending"), 0,
                      KAboutData::License_GPL,
                      ki18n("A tool to blend bracketed images"),
                      ki18n("(c) 2009-2014, Gilles Caulier"))
    {
         addAuthor(ki18n("Gilles Caulier"),
                   ki18n("Author"),
                   "caulier dot gilles at gmail dot com");

         addAuthor(ki18n("Johannes Wienke"),
                   ki18n("Developer"),
                   "languitar at semipol dot de");

         addAuthor(ki18n("Benjamin Girault"),
                   ki18n("Developer"),
                   "benjamin dot girault at gmail dot com");

         setHandbookEntry("expoblending");
    }

    ~ExpoBlendingAboutData()
    {
    }
};

} // namespace KIPIExpoBlendingPlugin

#endif /* ABOUTDATA_H */
