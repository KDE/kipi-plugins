/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-24
 * Description : file list view and items.
 *
 * Copyright (C) 2008-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "myimagelist.moc"

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>

// KIPI includes

#include <libkipi/interface.h>

namespace KIPIDNGConverterPlugin
{

MyImageList::MyImageList(KIPI::Interface* iface, QWidget* parent)
    : ImagesList(iface, parent)
{
    setControlButtonsPlacement(ImagesList::NoControlButtons);
    listView()->setColumnLabel(ImagesListView::Filename, i18n("Raw File"));
    listView()->setColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(MyImageList::TARGETFILENAME),
                          i18n("Target File"), true);
    listView()->setColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(MyImageList::IDENTIFICATION),
                          i18n("Camera"), true);
}

MyImageList::~MyImageList()
{
}

void MyImageList::slotAddImages(const KUrl::List& list)
{
    /* Replaces the ImagesList::slotAddImages method, so that
     * MyImageListViewItems can be added instead of ImagesListViewItems */

    // Figure out which of the supplied URL's should actually be added and which
    // of them already exist.
    bool found;

    for (KUrl::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        KUrl imageUrl = *it;
        found         = false;
        for (int i = 0; i < listView()->topLevelItemCount(); ++i)
        {
            MyImageListViewItem* currItem = dynamic_cast<MyImageListViewItem*>(listView()->topLevelItem(i));
            if (currItem && currItem->url() == imageUrl)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            new MyImageListViewItem(listView(), imageUrl);
        }
    }

    // Duplicate the signalImageListChanged of the ImageWindow, to enable the
    // upload button again.
    emit signalImageListChanged();
}

// ------------------------------------------------------------------------------------------------

MyImageListViewItem::MyImageListViewItem(ImagesListView* view, const KUrl& url)
    : ImagesListViewItem(view, url)
{
}

MyImageListViewItem::~MyImageListViewItem()
{
}

void MyImageListViewItem::setDestFileName(const QString& str)
{
    m_destFileName = str;
    setText(MyImageList::TARGETFILENAME, m_destFileName);
}

QString MyImageListViewItem::destFileName() const
{
    return m_destFileName;
}

void MyImageListViewItem::setIdentity(const QString& str)
{
    m_identity = str;
    setText(MyImageList::IDENTIFICATION, m_identity);
}

QString MyImageListViewItem::identity() const
{
    return m_identity;
}

QString MyImageListViewItem::destPath() const
{
    QString path = url().directory() + "/" + destFileName();
    return path;
}

} // namespace KIPIDNGConverterPlugin
