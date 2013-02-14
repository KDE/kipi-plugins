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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GALLERYCONFIG_H
#define GALLERYCONFIG_H

// Qt includes

#include <QLabel>
#include <QCheckBox>

// KDE includes

#include <kdialog.h>
#include <klineedit.h>

namespace KIPIGalleryExportPlugin
{

class Gallery;

class GalleryEdit : public KDialog
{
    Q_OBJECT

public:

    GalleryEdit(QWidget* const pParent, Gallery* const pGallery, const QString& title);
    ~GalleryEdit();

private Q_SLOTS:

    void slotOk();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIGalleryExportPlugin

#endif /* GALLERYCONFIG_H */
