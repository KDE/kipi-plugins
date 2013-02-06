/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2011-2013 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

namespace KIPIPanoramaPlugin
{

class PanoramaAboutData : public KPAboutData
{

public:

    PanoramaAboutData()
        : KPAboutData(ki18n("Panorama Stitching"), 0,
                      KAboutData::License_GPL,
                      ki18n("A tool to automatically stitch images into panorama"),
                      ki18n("(c) 2011-2013, Benjamin Girault"))
    {
         addAuthor(ki18n("Benjamin Girault"),
                   ki18n("Author"),
                   "benjamin dot girault at gmail dot com");

         addAuthor(ki18n("Gilles Caulier"),
                   ki18n("Developer"),
                   "caulier dot gilles at gmail dot com");

         setHandbookEntry("panorama");
    }

    ~PanoramaAboutData()
    {
    }
};

} // namespace KIPIPanoramaPlugin

#endif /* ABOUTDATA_H */
