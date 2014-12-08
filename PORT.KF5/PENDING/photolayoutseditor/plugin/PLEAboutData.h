/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a tool to create photo layouts by fusion of several images.
 *
 * Copyright (C) 2011 by �ukasz Spas <lukasz dot spas at gmail dot com>
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

#ifndef PLEABOUTDATA_H
#define PLEABOUTDATA_H

// Local includes

#include "kpaboutdata.h"

namespace KIPIPhotoLayoutsEditor
{

class PLEAboutData : public KIPIPlugins::KPAboutData
{
    public:

        PLEAboutData() :
            KPAboutData(ki18n("Photo Layouts Editor"),
                       0,
                       KAboutData::License_GPL,
                       ki18n("A tool to create photo layouts by fusion of several images"),
                       ki18n("(c) 2011, Lukasz Spas"))
        {
            this->setAppName("photolayoutseditor");
            addAuthor(ki18n("Lukasz Spas"),
                      ki18n("Author and Developer"),
                      "lukasz dot spas at gmail dot com");
        }
};

} // namespace KIPIPhotoLayoutsEditor

#endif // PLEABOUTDATA_H
