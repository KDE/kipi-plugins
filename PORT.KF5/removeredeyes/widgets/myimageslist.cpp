/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-05-21
 * Description : a widget to display the imagelist
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#define ICONSIZE 64

#include "myimageslist.moc"

// Qt includes

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>

// KDE includes

#include <klocale.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>

// Local includes

#include "kpimagedialog.h"

namespace KIPIRemoveRedEyesPlugin
{

MyImagesList::MyImagesList(QWidget* const parent)
    : KPImagesList(parent)
{
    setAllowRAW(false);

    // --------------------------------------------------------

    listView()->setColumn(KPImagesListView::User1, i18n("Corrected Eyes"), true);
    listView()->header()->setResizeMode(QHeaderView::Stretch);
    listView()->setWhatsThis(i18n("This is the list of images from which to remove red-eye."));
}

MyImagesList::~MyImagesList()
{
}

void MyImagesList::addEyeCounterByUrl(const KUrl& url, int eyes)
{
    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item && item->url() == url)
        {
            item->setText(KPImagesListView::User1, QString::number(eyes));
            break;
        }

        ++it;
    }

    emit signalImageListChanged();
}

void MyImagesList::resetEyeCounterColumn()
{
    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item)
        {
            item->setText(KPImagesListView::User1, QString(""));
            ++it;
        }
    }

    emit signalImageListChanged();
}

bool MyImagesList::hasUnprocessedImages()
{
    bool hasNone = false;

    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item && item->text(KPImagesListView::User1).toInt() <= 0)
        {
            hasNone = true;
            break;
        }

        ++it;
    }

    return hasNone;
}

void MyImagesList::removeUnprocessedImages()
{
    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item)
        {
            // first, deselect item if selected
            item->setSelected(false);

            // select the item if no corrections were made
            if (item->text(KPImagesListView::User1).toInt() <= 0 && !item->text(KPImagesListView::User1).isEmpty())
            {
                item->setSelected(true);
            }
        }

        ++it;
    }

    slotRemoveItems();
}

}  // namespace KIPIRemoveRedEyesPlugin
