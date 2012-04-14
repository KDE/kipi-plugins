/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetects cpfind binary program and version
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

#ifndef CPFINDBINARY_H
#define CPFINDBINARY_H

// Local includes

#include "kpbinaryiface.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

class CPFindBinary : public KPBinaryIface
{

public:

    CPFindBinary()
        : KPBinaryIface(QString("cpfind"),
                        QString("2010.4"),
                        QString(),
                        0,
                        QString("Hugin"),
                        QString("http://hugin.sourceforge.net"),
                        QString("Panorama"),
                        QStringList("--version")
                       ),
          headerRegExp("^Hugin'?s cpfind( Pre-Release)? (\\d+\\.\\d+(\\.\\d+)?)(\\D.*)?$")
        {
            setup();
        }

    ~CPFindBinary()
    {
    }

protected:

    virtual bool parseHeader(const QString& output);

private:

    QRegExp headerRegExp;
};

} // namespace KIPIPanoramaPlugin

#endif  // CPFINDBINARY_H
