/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : file list view and items.
 *
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011      by Veaceslav Munteanu <slavuttici at gmail dot com>
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

// Qt includes

#include <QFileInfo>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>

// LibKDcraw includes

#include "kpmetadata.h"

namespace KIPIDNGConverterPlugin
{

MyImageList::MyImageList(QWidget* const parent)
    : KPImagesList(parent)
{
    setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    listView()->setColumnLabel(KPImagesListView::Filename, i18n("Raw File"));
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(TARGETFILENAME), i18n("Target File"), true);
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(IDENTIFICATION), i18n("Camera"),      true);
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(STATUS),         i18n("Status"),      true);
}

MyImageList::~MyImageList()
{
}

void MyImageList::slotAddImages(const KUrl::List& list)
{
    /* Replaces the KPImagesList::slotAddImages method, so that
     * MyImageListViewItems can be added instead of ImagesListViewItems */

    // Figure out which of the supplied URL's should actually be added and which
    // of them already exist.
    bool found = false;

    for (KUrl::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        KUrl imageUrl = *it;
        found         = false;

        for (int i = 0; i < listView()->topLevelItemCount(); ++i)
        {
            MyImageListViewItem* const currItem = dynamic_cast<MyImageListViewItem*>(listView()->topLevelItem(i));

            if (currItem && currItem->url() == imageUrl)
            {
                found = true;
                break;
            }
        }

        if (!found && KPMetadata::isRawFile(imageUrl))
        {
            new MyImageListViewItem(listView(), imageUrl);
        }
    }

    // Duplicate the signalImageListChanged of the ImageWindow, to enable the
    // upload button again.
    emit signalImageListChanged();
}
void MyImageList::slotRemoveItems()
{
    bool find = false;

    do
    {
        find = false;
        QTreeWidgetItemIterator it(listView());

        while (*it)
        {
            MyImageListViewItem* const item = dynamic_cast<MyImageListViewItem*>(*it);

            if (item && item->isSelected())
            {
                delete item;
                find = true;
                break;
            }

            ++it;
        }
    }
    while(find);
}

// ------------------------------------------------------------------------------------------------

MyImageListViewItem::MyImageListViewItem(KPImagesListView* const view, const KUrl& url)
    : KPImagesListViewItem(view, url)
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

void MyImageListViewItem::setStatus(const QString& str)
{
    m_status = str;
    setText(MyImageList::STATUS, m_status);
}

QString MyImageListViewItem::destPath() const
{
    QString path = url().directory() + QString("/") + destFileName();
    return path;
}

} // namespace KIPIDNGConverterPlugin
