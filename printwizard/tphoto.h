/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2002-12-09
 * Description : a kipi plugin to print images
 *
 * Copyright 2002-2003 by Todd Shoemaker <todd@theshoemakers.net>
 * Copyright 2007-2008 by Angelo Naselli <anaselli at linux dot it>
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

#ifndef TPHOTO_H
#define TPHOTO_H

// Qt includes.

#include <QString>
//#include <qptrlist.h>
//#include <qrect.h>
//#include <qiconview.h>

// KDE includes.

#include <kurl.h>
#include <libkexiv2/kexiv2.h>

class QImage;
class QPixmap;

class KPrinter;

namespace KIPIPrintWizardPlugin
{

class TPhoto
{

public:

    TPhoto(int thumbnailSize);
    ~TPhoto();

    KUrl filename; // full path

    QPixmap & thumbnail();
    QImage    loadPhoto();

    int m_thumbnailSize;

    int width();
    int height();

    QRect cropRegion;
    int copies;
    int rotation;
    KExiv2Iface::KExiv2 *exiv2Iface();

private:

    void   loadCache();
    QSize& size();

private:

    QPixmap             *m_thumbnail;
    QSize               *m_size;
    KExiv2Iface::KExiv2 *m_exiv2Iface;
};

}  // NameSpace KIPIPrintWizardPlugin

#endif // TPHOTO_H
