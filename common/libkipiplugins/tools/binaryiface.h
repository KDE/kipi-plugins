/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect binary program and version
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef BINARYIFACE_H
#define BINARYIFACE_H

// Qt includes

#include <QString>

// KDE includes

#include <kurl.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT BinaryIface
{

public:

    BinaryIface();
    virtual ~BinaryIface();

    bool    isAvailable()    const;
    QString version()        const;
    bool    versionIsRight() const;
    bool    showResults()    const;

    virtual void checkSystem() = 0;
    virtual QString path() const = 0;
    virtual QString minimalVersion() const = 0;
    virtual KUrl url() const = 0;
    virtual QString projectName() const = 0;

protected:

    QString findHeader(const QStringList& output, const QString& header) const;

protected:

    bool    m_available;

    QString m_version;
};

} // namespace KIPIPlugins

#endif  // BINARYIFACE_H
