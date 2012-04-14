/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect align_image_stack binary program and version
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef ALIGNBINARY_H
#define ALIGNBINARY_H

// Local includes

#include "kpbinaryiface.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class AlignBinary : public KPBinaryIface
{

public:

    AlignBinary()
        : KPBinaryIface(QString("align_image_stack"), 
                        QString("0.8"), 
                        QString("align_image_stack version "),
                        1, 
                        QString("Hugin"), 
                        QString("http://hugin.sourceforge.net"),
                        QString("ExpoBlending"), 
                        QStringList("-h")
                       )
        {
            setup();
        }

    ~AlignBinary()
    {
    }
};

} // namespace KIPIExpoBlendingPlugin

#endif  // ALIGNBINARY_H
