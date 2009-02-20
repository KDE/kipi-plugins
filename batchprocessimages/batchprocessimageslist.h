/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef BATCHPROCESSIMAGE_H
#define BATCHPROCESSIMAGE_H

// Include files for Qt

#include <qobject.h>
//Added by qt3to4:
#include <QDropEvent>
#include <QDragEnterEvent>

// Include files for KDE

#include <k3listview.h>

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesList : public K3ListView
{
Q_OBJECT

public:
    BatchProcessImagesList(QWidget *parent=0, const char *name=0);

signals:
    void addedDropItems(QStringList filesPath);

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent* e);
    bool acceptDrag(QDropEvent* e) const;
    void contentsDropEvent(QDropEvent* e);
    void droppedImagesItems(QDropEvent *e);
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif
