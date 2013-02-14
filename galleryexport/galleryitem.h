/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2003-10-01
* Description : a plugin to export to a remote Gallery server.
*
* Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
* Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
* Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
* Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
*
* This program is free software; you can redistribute it
* and/or modify it under the terms of the GNU General
* Public License as published by the Free Software Foundation;
* either version 2, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* ============================================================ */

#ifndef GALLERYITEM_H
#define GALLERYITEM_H

// Qt includes

#include <QString>

namespace KIPIGalleryExportPlugin
{

class GPhoto
{
public:

    GPhoto()
    {
        ref_num   = -1;
        album_num = -1;
    }

    int     ref_num;
    int     album_num;

    QString name;
    QString caption;
    QString thumbName;
    QString albumURL;
};

// ------------------------------------------------------------------------

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

    bool operator<(const GAlbum& rhs) const 
    {
        if (parent_ref_num == rhs.parent_ref_num)
        {
            return ref_num < rhs.ref_num;
        }

        return parent_ref_num < rhs.parent_ref_num;
    }

    // permissions
    bool    add;
    bool    write;
    bool    del_item;
    bool    del_alb;
    bool    create_sub;

    int     ref_num;         // album reference number
    int     parent_ref_num;  // parent's album reference number

    QString name;            // ?
    QString parentName;      // ?
    QString title;           // album title
    QString summary;         // album summary
    QString baseurl;         // album partial (gallery) URL
};

} // namespace KIPIGalleryExportPlugin

#endif /* GALLERYITEM_H */
