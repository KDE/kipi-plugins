/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QObject>
#include <QTreeWidget>

// KDE includes

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesList : public QTreeWidget
{
    Q_OBJECT

public:

    BatchProcessImagesList(QWidget *parent = 0);

Q_SIGNALS:

    void addedDropItems(QStringList filesPath);

protected:

    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent* e);
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif // BATCHPROCESSIMAGE_H
