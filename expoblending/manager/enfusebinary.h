/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect enfuse binary program and version
 *
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "binaryiface.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class EnfuseBinary : public BinaryIface
{

public:

    EnfuseBinary()
    : BinaryIface(QString("enfuse"), QString("3.2"), QString("enfuse "),
                      0, QString("Enblend"), QString("http://enblend.sourceforge.net"),
                      QString("ExpoBlending"), QStringList("-V"))
        { setup(); }

    ~EnfuseBinary() {}

protected:

    bool parseHeader(const QString& output);

};

} // namespace KIPIExpoBlendingPlugin

#endif  // ENFUSEBINARY_H
