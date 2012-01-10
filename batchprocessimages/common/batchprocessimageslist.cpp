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

#include "batchprocessimageslist.moc"

// Qt includes

#include <QDragEnterEvent>
#include <QDropEvent>

// KDE includes

#include <klocale.h>
#include <kurl.h>

namespace KIPIBatchProcessImagesPlugin
{

BatchProcessImagesList::BatchProcessImagesList(QWidget *parent)
                      : QTreeWidget(parent)
{
    setAcceptDrops(true);
    setDropIndicatorShown(false);
    setColumnCount(5);
    setColumnHidden(4, /*hide=*/true); // Column holding the sort key
    setRootIsDecorated(false);
    setHeaderLabels(QStringList()
                    << i18n("Source Album")
                    << i18n("Source Image")
                    << i18n("Target Image")
                    << i18n("Result")
                   );
    sortByColumn(3);
    setAllColumnsShowFocus(true);
    this->setWhatsThis(i18n("<p>You can see here the operations' results "
                            "during the process. Double-click on an item for more "
                            "information once the process has ended.</p>"
                            "<p>You can use the \"Add\" button or drag-and-drop "
                            "to add some new items to the list.</p>"
                            "<p>If the items are taken from different Albums "
                            "the process' results will be merged to the target Album.</p>"));
}

void BatchProcessImagesList::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void BatchProcessImagesList::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void BatchProcessImagesList::dropEvent(QDropEvent* event)
{
    const KUrl::List urlList = KUrl::List::fromMimeData(event->mimeData());
    QStringList paths;
    Q_FOREACH(const KUrl& url, urlList) {
        paths << url.toLocalFile();
    }

    if (!paths.isEmpty()) {
        emit addedDropItems(paths);
    }
}

}  // namespace KIPIBatchProcessImagesPlugin
