/* ============================================================
 * File  : galleryitem.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-04
 * Description : 
 * 
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef GALLERYITEM_H
#define GALLERYITEM_H

#include <qstring.h>

namespace KIPIGalleryExportPlugin
{

class GPhoto
{
public:

    GPhoto()
    {
        ref_num = -1;
    }
    
    int           ref_num;
    int           album_num;
    QString       name;
    QString       caption;
    QString       thumbName;
    QString       albumURL;
};    

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
            return ref_num < rhs.ref_num;

        return parent_ref_num < rhs.parent_ref_num;
    }

    int                ref_num;
    int                parent_ref_num;
    QString            name;
    QString            parentName;
    QString            title;
    QString            summary;
    QString            baseurl;
                       
    bool               add;
    bool               write;
    bool               del_item;
    bool               del_alb;
    bool               create_sub;
};

}

#endif /* GALLERYITEM_H */
