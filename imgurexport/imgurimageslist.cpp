/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#include "imgurimageslist.moc"

// KDE includes

#include <kdebug.h>
#include <klocale.h>

// Local includes

#include "kpmetadata.h"
#include "imgurtalker.h"

namespace KIPIImgurExportPlugin
{

ImgurImagesList::ImgurImagesList(Interface* const interface, QWidget* const parent)
    : KPImagesList(interface, parent)
{
    //setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    listView()->setColumnLabel(KPImagesListView::Filename, i18n("Raw File"));
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(ImgurImagesList::URL),
                          i18n("Imgur URL"), true);
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(ImgurImagesList::DELETEURL),
                          i18n("Imgur Delete URL"), true);
}

ImgurImagesList::~ImgurImagesList()
{
}

void ImgurImagesList::updateItemWidgets()
{
    kDebug() << "update";
}

void ImgurImagesList::slotAddImages(const KUrl::List& list)
{
    /* Replaces the KPImagesList::slotAddImages method, so that
     * ImgurImageListViewItems can be added instead of ImagesListViewItems */

    // Figure out which of the supplied URL's should actually be added and which
    // of them already exist.
    bool found;

    for (KUrl::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        KUrl imageUrl = *it;
        found         = false;

        KPMetadata meta(imageUrl.toLocalFile());

        const QString sUrl = meta.getXmpTagString("Xmp.kipi.ImgurURL");
        const QString sDeleteUrl = meta.getXmpTagString("Xmp.kipi.ImgurDeleteURL");

        for (int i = 0; i < listView()->topLevelItemCount(); ++i)
        {
            ImgurImageListViewItem* currItem = dynamic_cast<ImgurImageListViewItem*>(listView()->topLevelItem(i));
            if (currItem && currItem->url() == imageUrl)
            {
                found = true;
                if (!sUrl.isEmpty()) {
                    currItem->setUrl(sUrl);
                }

                if (!sDeleteUrl.isEmpty()) {
                    currItem->setDeleteUrl(sDeleteUrl);
                }
                break;
            }
        }

        if (!found) {
            new ImgurImageListViewItem(listView(), imageUrl);
        }
    }

    // Duplicate the signalImageListChanged of the ImageWindow, to enable the
    // upload button again.
    emit signalImageListChanged();
}

void ImgurImagesList::slotImageChanged (const KUrl& imageUrl, ImgurSuccess success) {
    KPMetadata meta(imageUrl.toLocalFile());

    if (!sUrl.isEmpty()) {
        currItem->setUrl(success.links.imgur_page);
    }

    if (!sDeleteUrl.isEmpty()) {
        currItem->setDeleteUrl(success.links.delete_page);
    }
}

// ------------------------------------------------------------------------------------------------

ImgurImageListViewItem::ImgurImageListViewItem(KPImagesListView* const view, const KUrl& url)
    : KPImagesListViewItem(view, url)
{
}

ImgurImageListViewItem::~ImgurImageListViewItem()
{
}

void ImgurImageListViewItem::setUrl(const QString& str)
{
    m_Url = str;
    setText(ImgurImagesList::URL, m_Url);
}

QString ImgurImageListViewItem::Url() const
{
    return m_Url;
}

void ImgurImageListViewItem::setDeleteUrl(const QString& str)
{
    m_deleteUrl = str;
    setText(ImgurImagesList::DELETEURL, m_deleteUrl);
}

QString ImgurImageListViewItem::deleteUrl() const
{
    return m_deleteUrl;
}

} // namespace KIPIImgurExportPlugin
