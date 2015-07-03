/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect enfuse binary program and version
 *
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef ENFUSEBINARY_H
#define ENFUSEBINARY_H

// Local includes

#include "kpbinaryiface.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class EnfuseBinary : public KPBinaryIface
{
    Q_OBJECT

public:

    EnfuseBinary()
    : KPBinaryIface(QStringLiteral("enfuse"),
                    QStringLiteral("3.2"),
                    QStringLiteral("enfuse "),
                    0, 
                    QStringLiteral("Enblend"),
                    QStringLiteral("http://enblend.sourceforge.net"),
                    QStringLiteral("ExpoBlending"),
                    QStringList(QStringLiteral("-V"))),
      versionDouble(0)
        {
            setup();
        }

    ~EnfuseBinary()
    {
    }

    double getVersion() const;

Q_SIGNALS:

    void signalEnfuseVersion(double version);

protected:

    bool parseHeader(const QString& output);

private:

    double versionDouble;
};

} // namespace KIPIExpoBlendingPlugin

#endif  // ENFUSEBINARY_H
