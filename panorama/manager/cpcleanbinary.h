/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetects cpclean binary program and version
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef CPCLEANBINARY_H
#define CPCLEANBINARY_H

// Local includes

#include "kpbinaryiface.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

class CPCleanBinary : public KPBinaryIface
{

public:

    CPCleanBinary()
        : KPBinaryIface(QString("cpclean"),
                        QString("2010.4"),
                        QString("cpclean version "),
                        1, 
                        QString("Hugin"),
                        QString("http://hugin.sourceforge.net"),
                        QString("Panorama"), 
                        QStringList("-h")
                       )
        { 
            setup();
        }
};

} // namespace KIPIPanoramaPlugin

#endif  // CPCLEANBINARY_H
