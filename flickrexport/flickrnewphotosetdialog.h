/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-10-27
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2008      by Andi Clemens <andi dot clemens at googlemail dot com>
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

#ifndef NEWPHOTOSETDIALOG_H
#define NEWPHOTOSETDIALOG_H

// Qt includes

#include <QDialog>

// Local includes

#include "ui_flickralbumdialog.h"

namespace KIPIFlickrExportPlugin
{

class FlickrNewPhotoSetDialog : public QDialog, public Ui::FlickrAlbumDialog
{
public:

    explicit FlickrNewPhotoSetDialog(QWidget* const parent)
        : QDialog(parent)
    {
        setupUi(this);
    }
};

} // namespace KIPIFlickrExportPlugin

#endif // NEWALBUMDIALOG_H
