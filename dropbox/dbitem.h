/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Dropbox web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#ifndef DBITEM_H
#define DBITEM_H

// Qt includes

#include <QString>

namespace KIPIDropboxPlugin
{

class DBPhoto
{

public:

    DBPhoto()
    {
    }

    QString title;
};

class DBFolder
{

public:

    DBFolder()
    {
    }

    QString title;
};

} // namespace KIPIDropboxPlugin

#endif /* DBITEM_H */
