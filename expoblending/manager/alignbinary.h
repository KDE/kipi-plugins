/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect align_image_stack binary program and version
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QString>

namespace KIPIExpoBlendingPlugin
{

class AlignBinaryPriv;

class AlignBinary
{

public:

    AlignBinary();
    ~AlignBinary();

    static const char* path();
    bool               isAvailable()    const;
    QString            version()        const;
    bool               versionIsRight() const;
    QString            minimalVersion() const;

    void               checkSystem();

private:

    AlignBinaryPriv* const d;
};

} // namespace KIPIExpoBlendingPlugin

#endif  // ALIGNBINARY_H
