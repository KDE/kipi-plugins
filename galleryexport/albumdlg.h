
/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 *
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
 * ============================================================ */

#ifndef ALBUMDLG_H
#define ALBUMDLG_H

#include "ui_galleryalbumwidget.h"

#include <KDialog>
#include <QWidget>

namespace KIPIGalleryExportPlugin
{

class AlbumDlg : public KDialog , public Ui::GalleryAlbumWidget
{
Q_OBJECT

public:
  AlbumDlg(QWidget *parent = 0);
  ~AlbumDlg();
};

}
#endif // ALBUMDLG_H
