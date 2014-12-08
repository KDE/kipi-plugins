/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-02-12
 * Description : common settings for redeyes removal
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at googlemail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef COMMONSETTINGS_H
#define COMMONSETTINGS_H

// Qt includes

#include <QString>

namespace KIPIRemoveRedEyesPlugin
{

class CommonSettings
{
public:

    CommonSettings()
        : addKeyword(true),
          storageMode(0),
          unprocessedMode(0)
    {
    };

public:

    bool    addKeyword;
    int     storageMode;
    int     unprocessedMode;
    QString extraName;
    QString keywordName;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* COMMONSETTINGS_H */
