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

// Local includes

#include "albumdlg.moc"

namespace KIPIGalleryExportPlugin
{

AlbumDlg::AlbumDlg(QWidget* const parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(buttonOk, SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(buttonCancel, SIGNAL(clicked()),
            this, SLOT(reject()));
}

AlbumDlg:: ~AlbumDlg()
{
}

} // namespace KIPIGalleryExportPlugin
