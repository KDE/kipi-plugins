/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-09-09
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#ifndef KPMULTIPART_H
#define KPMULTIPART_H

// Qt includes

#include <QHttpMultiPart>
#include <QString>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPMultiPart
{

public:

    KPMultiPart();
    ~KPMultiPart();

    QHttpMultiPart* multiPart() const;

    bool appendFile(const QString &header, const QString& path);
    bool appendPair(const QString& name, const QString& value, const QString& contentType);

private:

    QHttpMultiPart* m_multiPart;
};

} // namespace KIPIPlugins

#endif //KPMULTIPART_H
