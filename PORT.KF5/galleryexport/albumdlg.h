/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ALBUMDLG_H
#define ALBUMDLG_H

#include "ui_galleryalbumwidget.h"

// Qt includes

#include <QDialog>
#include <QWidget>

namespace KIPIGalleryExportPlugin
{

class AlbumDlg : public QDialog , public Ui::GalleryAlbumWidget
{
    Q_OBJECT

public:

    explicit AlbumDlg(QWidget* const parent = 0);
    ~AlbumDlg();
};

} // namespace KIPIGalleryExportPlugin

#endif // ALBUMDLG_H
