/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-07
 * Description : Scan Dialog about data.
 *
 * Copyright (C) 2009-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

namespace KIPIAcquireImagesPlugin
{

class ScanDialogAboutData : public KPAboutData
{

public:

    ScanDialogAboutData()
        : KPAboutData(ki18n("Acquire images"), 0,
                      KAboutData::License_GPL,
                      ki18n("A tool to acquire images using a flat scanner"),
                      ki18n("(c) 2003-2013, Gilles Caulier\n"))
    {
         addAuthor(ki18n("Gilles Caulier"),
                   ki18n("Author"),
                   "caulier dot gilles at gmail dot com");

         addAuthor(ki18n("Kare Sars"),
                   ki18n("Developer"),
                   "kare dot sars at kolumbus dot fi");

         addAuthor(ki18n("Angelo Naselli"),
                   ki18n("Developer"),
                   "anaselli at linux dot it");

         setHandbookEntry("acquireimages");
    }

    ~ScanDialogAboutData()
    {
    }
};

} // namespace KIPIAcquireImagesPlugin

#endif /* ABOUTDATA_H */
