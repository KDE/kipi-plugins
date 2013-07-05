/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-05-16
 * Description : Autodetects pano_modify binary program and version
 *
 * Copyright (C) 2013 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef PANOMODIFYBINARY_H
#define PANOMODIFYBINARY_H

// Local includes

#include "kpbinaryiface.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

class PanoModifyBinary : public KPBinaryIface
{

public:

    PanoModifyBinary()
        : KPBinaryIface(QString("pano_modify"),
                        QString("2012.0"),
                        QString("pano_modify version "),
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

#endif  // PANOMODIFYBINARY_H
