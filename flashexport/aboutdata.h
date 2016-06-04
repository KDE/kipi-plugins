/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-19
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011-2013 by Veaceslav Munteanu <slavuttici at gmail dot com>
 * Copyright (C) 2009-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
                      KAboutLicense::GPL,
                      ki18n("A Kipi plugin to export images to Flash using the SimpleViewer's components."),
                      ki18n("(c) 2005-2006, Joern Ahrens\n"
                            "(c) 2008-2016, Gilles Caulier\n"
                            "(c) 2011-2013, Veaceslav Munteanu"))

    {
        setHandbookEntry(QLatin1String("flashexport"));

        addAuthor(i18n("Joern Ahrens"),
                  i18n("Author"),
                  QLatin1String("joern dot ahrens at kdemail dot net"));

        addAuthor(i18n("Gilles Caulier"),
                  i18n("Developer and maintainer"),
                  QLatin1String("caulier dot gilles at gmail dot com"));

        addAuthor(i18n("Veaceslav Munteanu"),
                  i18n("Developer and maintainer"),
                  QLatin1String("slavuttici at gmail dot com"));

        addCredit(i18n("Felix Turner"),
                  i18n("Author of the Simple Viewer Flash component"),
                  QString(),
                  QLatin1String("http://www.simpleviewer.net"));

        addCredit(i18n("Mikkel B. Stegmann"),
                  i18n("Basis for the index.html template"),
                  QString(),
                  QLatin1String("http://www.stegmann.dk/mikkel/porta"));
    }

    ~FlashExportAboutData()
    {
    }
};

} // namespace KIPIFlashExportPlugin

#endif // ABOUTDATA_H
