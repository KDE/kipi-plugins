/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz@kde.org>
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

#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <QPaintEvent>
#include <QDropEvent>
#include <QDragEnterEvent>

#include <k3listview.h>

namespace KIPIIpodExportPlugin
{

class ImageList : public K3ListView
{
Q_OBJECT

public:

    enum ListType { UploadType, IpodType };

    ImageList( ListType=UploadType, QWidget *parent=0, const char *name=0 );

    ListType getType() const { return m_type; }

signals:

    void addedDropItems( QStringList filesPath );

protected:

    bool acceptDrag( QDropEvent *e ) const;
    void contentsDropEvent( QDropEvent *e );
    void dragEnterEvent( QDragEnterEvent *e );
    void dropEvent( QDropEvent *e );
    void droppedImagesItems( QDropEvent *e );
    void viewportPaintEvent( QPaintEvent *e );

private:

    ListType m_type;
};

} // namespace KIPIIpodExportPlugin

#endif // IMAGELIST_H
