/***************************************************************************
                          tphoto.h  -  description
                             -------------------
    begin                : Thu Sep 12 2002
    copyright            : (C) 2002 by Todd Shoemaker
                         : (C) 2007 Angelo Naselli
    email                : jtshoe11@yahoo.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TPHOTO_H
#define TPHOTO_H

// Qt includes.

#include <qstring.h>
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
