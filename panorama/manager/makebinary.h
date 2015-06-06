/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect make binary program
 *
 * Copyright (C) 2011-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef MAKEBINARY_H
#define MAKEBINARY_H

// Local includes

#include "kpbinaryiface.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

class MakeBinary : public KPBinaryIface
{

public:

    MakeBinary()
        : KPBinaryIface(QString::fromUtf8("make"),
                        QString::fromUtf8("3.80"),
                        QString::fromUtf8("GNU Make "),
                        0,
                        QString::fromUtf8("GNU"),
                        QString::fromUtf8("http://www.gnu.org/software/make/"),
                        QString::fromUtf8("Panorama"),
                        QStringList(QString::fromUtf8("-v"))
                       )
        {
            setup();
        }

    ~MakeBinary()
    {
    }
};

} // namespace KIPIPanoramaPlugin

#endif  // MAKEBINARY_H
