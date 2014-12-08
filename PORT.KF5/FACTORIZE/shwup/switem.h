/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009      by Timothée Groleau <kde at timotheegroleau dot com>
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

#ifndef SWITEM_H
#define SWITEM_H

// Qt includes

#include <QString>

namespace KIPIShwupPlugin
{

class SwUser
{

public:

    SwUser()
    {
        id = 0;
    }

    void clear()
    {
        id = 0;
        name.clear();
        email.clear();
        password.clear();
        profileURL.clear();
    }

    long long id;

    QString   name;
    QString   email;
    QString   password;
    QString   profileURL;
};

// ------------------------------------------------------

class SwAlbum
{

public:

    SwAlbum()
    {
        id                = -1;
        description       = "";
        title             = "";
        token             = "";
        albumUrl          = "";
        albumThumbnailUrl = "";

        canUpload         = false;
        canDownload       = false;
        canInvite         = false;
    }

    long long id;

    QString   token;
    QString   title;
    QString   description;
    QString   albumUrl;
    QString   albumThumbnailUrl;

    bool      canUpload;
    bool      canDownload;
    bool      canInvite;
};

// ------------------------------------------------------

class SwPhoto
{

public:

    SwPhoto()
    {
        id = -1;
    }

    long long id;

    QString   caption;
    QString   thumbURL;
    QString   originalURL;
};

} // namespace KIPIShwupPlugin

#endif /* SWITEM_H */

