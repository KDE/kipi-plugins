/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#ifndef PICASAWEBITEM_H
#define PICASAWEBITEM_H

// Qt includes

#include <QString>
#include <QStringList>

namespace KIPIPicasawebExportPlugin
{

// ------------------------------------------------------

class FPhotoInfo
{

public:

    FPhotoInfo()
    {
        is_public = false;
        is_family = false;
        is_friend = false;
    }

    bool        is_public;
    bool        is_friend;
    bool        is_family;

    QString     title;
    QString     description;
    QStringList tags;
};

// ------------------------------------------------------

class GAlbum
{

public:

    GAlbum()
    {
        ref_num        = -1;
        parent_ref_num = -1;

        add            = false;
        write          = false;
        del_item       = false;
        del_alb        = false;
        create_sub     = false;
    }

    int                ref_num;
    int                parent_ref_num;

    bool               add;
    bool               write;
    bool               del_item;
    bool               del_alb;
    bool               create_sub;

    QString            name;
    QString            parentName;
    QString            title;
    QString            summary;
    QString            baseurl;
};

// ------------------------------------------------------

class PicasaWebAlbum
{

public:

    PicasaWebAlbum()
    {
        id="-1";
    }

    QString  id;
    QString primary;  //="2483"
    QString secret;   //="abcdef"
    QString server;
    QString photos;
    QString title;
    QString description;
};

} // namespace KIPIPicasawebExportPlugin

#endif /* PICASAWEBITEM_H */
