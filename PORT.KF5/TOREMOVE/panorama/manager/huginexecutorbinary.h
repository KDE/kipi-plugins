/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-11-02
 * Description : Autodetect hugin_executor binary program and version
 *
 * Copyright (C) 2015-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef HUGINEXECUTORBINARY_H
#define HUGINEXECUTORBINARY_H

// Local includes

#include "kpbinaryiface.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

class HuginExecutorBinary : public KPBinaryIface
{

public:

    HuginExecutorBinary()
        : KPBinaryIface(QStringLiteral("hugin_executor"),
                        QStringLiteral("Hugin"),
                        QStringLiteral("http://hugin.sourceforge.net"),
                        QStringLiteral("Panorama"),
                        QStringList(QStringLiteral("-h"))
                       )
        {
            setup(); 
        }

    ~HuginExecutorBinary()
    {
    }
};

} // namespace KIPIPanoramaPlugin

#endif  // HUGINEXECUTORBINARY_H
