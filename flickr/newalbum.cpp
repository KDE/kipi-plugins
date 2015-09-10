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

#include "newalbum.h"

// Qt includes

#include <QFormLayout>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>
#include <klineedit.h>

// Local includes

#include "kipiplugins_debug.h"
#include "flickritem.h"

namespace KIPIFlickrPlugin
{

NewAlbum::NewAlbum(QWidget* const parent, const QString& pluginName)
    : KPNewAlbumDialog(parent,pluginName)
{
        hideDateTime();
        hideLocation();
        getMainWidget()->setMinimumSize(350,0);
}

NewAlbum::~NewAlbum()
{
}

void NewAlbum::getFolderProperties(FPhotoSet& folder)
{
    folder.title       = getTitleEdit()->text();
    folder.description = getDescEdit()->toPlainText();
    qCDebug(KIPIPLUGINS_LOG) << "Folder Title " << folder.title<<" Folder Description "<<folder.description;
}

} // namespace KIPIFlickrPlugin
