/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-19
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011 by Veaceslav Munteanu
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

namespace KIPIFlashExportPlugin
{

class FlashExportAboutData : public KPAboutData
{

public:

    FlashExportAboutData()
        : KPAboutData(ki18n("Flash Export"), 0,
                      KAboutData::License_GPL,
                      ki18n("A Kipi plugin to export images to Flash using the SimpleViewer's components."),
                      ki18n("(c) 2005-2006, Joern Ahrens\n"
                            "(c) 2008-2012, Gilles Caulier\n"
                            "(c) 2011, Veaceslav Munteanu"))

    {
        setHandbookEntry("flashexport");

        addAuthor(ki18n("Joern Ahrens"),
                ki18n("Author"),
                "joern dot ahrens at kdemail dot net");

        addAuthor(ki18n("Gilles Caulier"),
                ki18n("Developer and maintainer"),
                "caulier dot gilles at gmail dot com");

        addAuthor(ki18n("Veaceslav Munteanu"),
                ki18n("Developer and maintainer"),
                "slavuttici at gmail dot com");

        addCredit(ki18n("Felix Turner"),
                ki18n("Author of the Simple Viewer Flash component"),
                0,
                "http://www.simpleviewer.net");

        addCredit(ki18n("Mikkel B. Stegmann"),
                ki18n("Basis for the index.html template"),
                0,
                "http://www.stegmann.dk/mikkel/porta");
    }

    ~FlashExportAboutData()
    {
    }
};

} // namespace KIPIFlashExportPlugin

#endif /* ABOUTDATA_H */
