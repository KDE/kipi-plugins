/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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
class PicasaWebAlbum
{

public:

    PicasaWebAlbum()
    {
        id="-1";
        canComment = true;
    }

    QString   id;
    QString   title;
    QString   timestamp;
    QString   description;
    QString   location;
    QString   access;
    bool      canComment;
    QStringList tags;
};

// -----------------------------------------------------------------

class PicasaWebPhoto
{

public:

    PicasaWebPhoto()
    {
        id="-1";
        canComment = true;
        gpsLon = "";
        gpsLat = "";
    }

    QString     id;
    QString     title;
    QString     timestamp;
    QString     description;
    QString     location;
    QString     access;
    bool        canComment;
    QStringList tags;
    QString     mimeType;
    QString     gpsLon;
    QString     gpsLat;
    KUrl        originalURL;
    KUrl        thumbURL;
    KUrl        editUrl;
};

} // namespace KIPIPicasawebExportPlugin

#endif /* PICASAWEBITEM_H */
