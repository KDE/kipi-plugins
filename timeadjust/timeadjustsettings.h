/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-19
 * Description : time adjust settings container.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef TIMEADJUSTSETTING_H
#define TIMEADJUSTSETTING_H

// Qt includes

#include <QList>
#include <QString>
#include <QStringList>

// KDE includes

#include <kurl.h>

namespace KIPITimeAdjustPlugin
{

class TimeAdjustSettings
{

public:

    TimeAdjustSettings()
    {
        updAppDate     = false;
        updEXIFModDate = false;
        updEXIFOriDate = false;
        updEXIFDigDate = false;
        updIPTCDate    = false;
        updXMPDate     = false;
        updFileName    = false;
        updFileModDate = false;
    };

    ~TimeAdjustSettings()
    {
    };

public:

    bool updAppDate;
    bool updEXIFModDate;
    bool updEXIFOriDate;
    bool updEXIFDigDate;
    bool updIPTCDate;
    bool updXMPDate;
    bool updFileName;
    bool updFileModDate;
};

}  // namespace KIPITimeAdjustPlugin

#endif  // TIMEADJUSTSETTING_H
