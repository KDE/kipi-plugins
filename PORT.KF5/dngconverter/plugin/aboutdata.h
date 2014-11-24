/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-07
 * Description : DNG converter about data.
 *
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2012 by Jens Mueller <tschenser at gmx dot de>
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

#include <QString>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "kpaboutdata.h"
#include "XMP_Version.h"

using namespace KDcrawIface;
using namespace KIPIPlugins;

namespace KIPIDNGConverterPlugin
{

class DNGConverterAboutData : public KPAboutData
{

public:

    DNGConverterAboutData()
        : KPAboutData(ki18n("DNG Image Converter"), 
                      0,
                      KAboutData::License_GPL,
                      ki18n("A tool to batch convert RAW camera images to DNG\n"
                            "Using LibRaw %1\n"
                            "Using LibKdcraw %2\n"
                            "Using XMP toolkit %3\n"
                            "Using DNG toolkit 1.3")
                            .subs(KDcraw::librawVersion())
                            .subs(KDcraw::version())
                            .subs(QString(XMP_API_VERSION_STRING)),
                      ki18n("(c) 2008-2013, Gilles Caulier\n"
                            "(c) 2010-2011, Jens Mueller\n"
                            "(c) 2012, Smit Mehta"))
    {
         addAuthor(ki18n("Gilles Caulier"),
                   ki18n("Author"),
                   "caulier dot gilles at gmail dot com");

         addAuthor(ki18n("Jens Mueller"),
                   ki18n("Developer"),
                   "tschenser at gmx dot de");

         addAuthor(ki18n("Smit Mehta"),
                   ki18n("Developer"),
                   "smit dot meh at gmail dot com");

        setHandbookEntry("dngconverter");
    }

    ~DNGConverterAboutData()
    {
    }
};

} // namespace KIPIDNGConverterPlugin

#endif /* ABOUTDATA_H */
