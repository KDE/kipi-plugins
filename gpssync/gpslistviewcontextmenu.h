/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-05-07
 * Description : Context menu for GPS list view which can be used
 *               in the track list editor and the sync dialog
 *
 * Copyright (C) 2009 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef GPSLISTVIEWCONTEXTMENU_H
#define GPSLISTVIEWCONTEXTMENU_H

// Qt includes:

#include <QObject>

// local includes:

#include "imageslist.h"

namespace KIPIGPSSyncPlugin
{

class GPSListViewContextMenuPriv;

class GPSListViewContextMenu : public QObject
{
    Q_OBJECT

public:

    GPSListViewContextMenu(KIPIPlugins::ImagesList *imagesList);
    ~GPSListViewContextMenu();

protected:

    virtual bool eventFilter(QObject *watched, QEvent *event);

public Q_SLOTS:

    void copyActionTriggered();
    void pasteActionTriggered();

private:

    GPSListViewContextMenuPriv* const d;
};

} // namespace KIPIGPSSyncPlugin

#endif /* GPSLISTVIEWCONTEXTMENU_H */
