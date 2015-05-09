/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to import/export images to/from 
                 SmugMug web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#ifndef SMUGITEM_H
#define SMUGITEM_H

// Qt includes

#include <QString>

namespace KIPISmugPlugin
{

class SmugUser
{

public:

    SmugUser()
    {
        fileSizeLimit = 0;
    }

    void clear()
    {
        email.clear();
        nickName.clear();
        displayName.clear();
        accountType.clear();
        fileSizeLimit = 0;
    }

    QString     email;
    QString     nickName;
    QString     displayName;
    QString     accountType;
    int         fileSizeLimit;
};

// ---------------------------------------------------------------------------------

class SmugAlbum
{

public:

    SmugAlbum()
    {
        id            = -1;
        categoryID    = -1;
        subCategoryID = -1;
        tmplID        = -1;
        isPublic      = true;
        imageCount    = 0;
    }

    qint64  id;

    QString key;
    QString title;
    QString description;
    QString keywords;

    qint64  categoryID;
    QString category;

    qint64  subCategoryID;
    QString subCategory;

    bool    isPublic;
    QString password;
    QString passwordHint;

    int     imageCount;

    // below fields only used by createAlbum (and not by listAlbums)
    qint64  tmplID;
    QString tmpl;

    static bool lessThan(SmugAlbum& a, SmugAlbum& b)
    {
        return a.title.toLower() < b.title.toLower();
    }
};

// ---------------------------------------------------------------------------------

class SmugPhoto
{

public:

    SmugPhoto()
    {
        id = -1;
    }

    qint64  id;

    QString key;
    QString caption;
    QString keywords;

    QString thumbURL;
    QString originalURL;
};

// ---------------------------------------------------------------------------------

class SmugAlbumTmpl
{

public:

    SmugAlbumTmpl()
    {
        id       = -1;
        isPublic = true;
    }

    qint64  id;
    QString name;

    bool    isPublic;
    QString password;
    QString passwordHint;
};

// ---------------------------------------------------------------------------------

class SmugCategory
{

public:

    SmugCategory()
    {
        id = -1;
    }

    qint64  id;
    QString name;
};

} // namespace KIPISmugPlugin

#endif /* SMUGITEM_H */
