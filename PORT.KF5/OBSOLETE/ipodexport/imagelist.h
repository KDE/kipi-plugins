/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2009 by Seb Ruiz <ruiz at kde dot org>
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

// Qt includes

#include <QTreeWidget>

namespace KIPIIpodExportPlugin
{

class ImageList : public QTreeWidget
{
    Q_OBJECT

public:

    enum ListType
    {
        UploadType,
        IpodType
    };

public:

    explicit ImageList(ListType = UploadType, QWidget* const parent = 0);

    ListType getType() const
    {
        return m_type;
    };

Q_SIGNALS:

    void signalAddedDropItems(const QStringList& filesPath);

protected:

    virtual bool dropMimeData(QTreeWidgetItem*, int, const QMimeData*, Qt::DropAction);
    void droppedImagesItems(const QList<QUrl>&);

private:

    ListType m_type;
};

} // namespace KIPIIpodExportPlugin

#endif // IMAGELIST_H
