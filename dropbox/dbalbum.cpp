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

#include "dbalbum.h"

// Qt includes

#include <QFormLayout>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>
#include <kdialog.h>
#include <klineedit.h>

// Local includes

#include "kipiplugins_debug.h"
#include "dbitem.h"

namespace KIPIDropboxPlugin
{

DBNewAlbum::DBNewAlbum(QWidget* const parent, const QString& pluginName)
    : KPNewAlbumDialog(parent,pluginName)
{
        hideDateTime();
        hideDesc();
        hideLocation();
        getMainWidget()->setMinimumSize(300,0);
}

DBNewAlbum::~DBNewAlbum()
{
}

void DBNewAlbum::getFolderTitle(DBFolder& folder)
{
    folder.title = QString("/") + getTitleEdit()->text();
    qCDebug(KIPIPLUGINS_LOG) << "getFolderTitle " << folder.title;
}

} // namespace KIPIDropboxPlugin
