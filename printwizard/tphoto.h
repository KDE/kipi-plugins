/***************************************************************************
                          tphoto.h  -  description
                             -------------------
    begin                : Thu Sep 12 2002
    copyright            : (C) 2002 by Todd Shoemaker
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
#include <qptrlist.h>
#include <qrect.h>
#include <qiconview.h>

// KDE includes.

#include <kurl.h>

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

    KURL filename; // full path

    QPixmap & thumbnail();

    int m_thumbnailSize;

    int width();
    int height();

    QRect cropRegion;
    int copies;
    int rotation;

private:
    QPixmap *m_thumbnail;
    void loadCache();
    QSize & size();
    QSize * m_size;
};

bool paintOnePage(QPainter &p, QPtrList<TPhoto> photos, QPtrList<QRect> layouts,
                  int captionType, unsigned int &current, bool useThumbnails = false);

bool paintOnePage(QImage &p, QPtrList<TPhoto> photos, QPtrList<QRect> layouts,
                  int captionType, unsigned int &current);

}  // NameSpace KIPIPrintWizardPlugin
                  
#endif // TPHOTO_H

