/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-10-18
 * Description : images list settings page.
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "myimagelist.moc"

namespace KIPISendimagesPlugin
{

MyImageList::MyImageList(QWidget* const parent)
    : KPImagesList(parent)
{
    setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
}

MyImageList::~MyImageList()
{
}

QList<EmailItem> MyImageList::imagesList() const
{
    QList<EmailItem> list;

    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item)
        {
            item->updateInformation();

            EmailItem etem;
            etem.orgUrl   = item->url();
            etem.tags     = item->tags();
            etem.comments = item->comments();
            etem.rating   = item->rating();
            list.append(etem);
        }

        ++it;
    }

    return list;
}

}  // namespace KIPISendimagesPlugin
