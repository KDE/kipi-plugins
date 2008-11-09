/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <me at sebruiz dot net>
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

#ifndef IMAGELISTITEM_H
#define IMAGELISTITEM_H

extern "C"
{
#include <gpod/itdb.h>
}

#include <qstring.h>

#include <k3listview.h>
#include <klocale.h>

namespace KIPIIpodExportPlugin
{

class ImageListItem : public K3ListViewItem
{

public:

    ImageListItem(Q3ListView *parent, QString const& pathSrc, QString const& name)
        : K3ListViewItem( parent, QString::null/*set below*/, name )
        , m_pathSrc( pathSrc )
    {
        setText( 0, pathSrc.section('/', -2, -2) );
    }

    QString pathSrc()    const { return m_pathSrc;    }

private:

    QString m_pathSrc;
};

} // namespace KIPIIpodExportPlugin

#endif  // IMAGELISTITEM_H
