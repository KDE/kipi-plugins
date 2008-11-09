/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IMAGELISTITEM_H
#define IMAGELISTITEM_H

extern "C" {
#include <gpod/itdb.h>
}

#include <qstring.h>
#include <k3listview.h>
#include <klocale.h>

namespace IpodExport
{

    class ImageListItem : public K3ListViewItem
    {
    public:

        ImageListItem( Q3ListView *parent, QString const & pathSrc, QString const & name )
            : K3ListViewItem( parent, QString::null/*set below*/, name )
            , m_pathSrc( pathSrc )
        {
            setText( 0, pathSrc.section('/', -2, -2) );
        }

        QString          pathSrc()    const { return m_pathSrc;    }

    private:
        QString          m_pathSrc;
    };

}

#endif  // IMAGELISTITEM_H
