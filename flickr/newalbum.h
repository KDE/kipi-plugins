/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-08-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#ifndef NEWALBUM_H
#define NEWALBUM_H

// Local includes

#include "kpnewalbumdialog.h"

using namespace KIPIPlugins;

namespace KIPIFlickrPlugin
{
class FPhotoSet;

class NewAlbum : public KPNewAlbumDialog
{
    Q_OBJECT

public:

    NewAlbum(QWidget* const parent, const QString& pluginName);
    ~NewAlbum();

    void getFolderProperties(FPhotoSet& folder);
};

} //namespace KIPIFlickrPlugin

#endif /*NEWALBUM_H*/
