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

#include "imgurimageslist.h"

// C++ includes

#include <memory>

// Qt includes

#include <QLabel>
#include <QPointer>
#include <QDesktopServices>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIImgurPlugin
{

ImgurImagesList::ImgurImagesList(QWidget* const parent)
    : KPImagesList(parent)
{
    setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    setAllowDuplicate(false);
    setAllowRAW(false);

    auto* list = listView();

    list->setColumnLabel(KPImagesListView::Thumbnail, i18n("Thumbnail"));

    list->setColumnLabel(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(ImgurImagesList::Title),
                         i18n("Submission title"));

    list->setColumnLabel(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(ImgurImagesList::Description),
                         i18n("Submission description"));

    list->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(ImgurImagesList::URL),
                    i18n("Imgur URL"), true);

    list->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(ImgurImagesList::DeleteURL),
                    i18n("Imgur Delete URL"), true);

    connect(list, &KPImagesListView::itemDoubleClicked,
            this, &ImgurImagesList::slotDoubleClick);
}

QList<const ImgurImageListViewItem*> ImgurImagesList::getPendingItems()
{
    QList<const ImgurImageListViewItem*> ret;

    for (unsigned int i = listView()->topLevelItemCount(); i--;)
    {
        const auto* item = dynamic_cast<const ImgurImageListViewItem*>(listView()->topLevelItem(i));

        if (item && item->ImgurUrl().isEmpty())
            ret << item;
    }

    return ret;
}

void ImgurImagesList::slotAddImages(const QList<QUrl>& list)
{
    /* Replaces the KPImagesList::slotAddImages method, so that
     * ImgurImageListViewItems can be added instead of ImagesListViewItems */

    std::unique_ptr<MetadataProcessor> meta;

    if (iface())
        meta = std::unique_ptr<MetadataProcessor>(iface()->createMetadataProcessor());

    for (QList<QUrl>::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        // Already in the list?
        if (listView()->findItem(*it) == nullptr)
        {
            auto* item = new ImgurImageListViewItem(listView(), *it);

            // Load URLs from meta data, if possible
            if (meta && meta->load(*it))
            {
                item->setImgurUrl(meta->getXmpTagString(QLatin1String("Xmp.kipi.ImgurId")));
                item->setImgurDeleteUrl(meta->getXmpTagString(QLatin1String("Xmp.kipi.ImgurDeleteHash")));
            }
        }
    }

    emit signalImageListChanged();
    emit signalAddItems(list);
}

void ImgurImagesList::slotSuccess(const ImgurAPI3Result& result)
{
    QUrl imgurl = QUrl::fromLocalFile(result.action->upload.imgpath);

    processed(imgurl, true);

    Interface* intf = iface();

    if (intf)
    {
        QPointer<MetadataProcessor> meta = intf->createMetadataProcessor();

        // Save URLs to meta data, if possible
        if (meta && meta->load(imgurl))
        {
            meta->setXmpTagString(QLatin1String("Xmp.kipi.ImgurId"),         result.image.url);
            meta->setXmpTagString(QLatin1String("Xmp.kipi.ImgurDeleteHash"), ImgurAPI3::urlForDeletehash(result.image.deletehash).toString());
            bool saved = meta->applyChanges();
            qCDebug(KIPIPLUGINS_LOG) << "Metadata" << (saved ? "Saved" : "Not Saved") << "to" << imgurl;
        }
    }

    ImgurImageListViewItem* const currItem = dynamic_cast<ImgurImageListViewItem*>(listView()->findItem(imgurl));

    if (!currItem)
        return;

    if (!result.image.url.isEmpty())
        currItem->setImgurUrl(result.image.url);

    if (!result.image.deletehash.isEmpty())
        currItem->setImgurDeleteUrl(ImgurAPI3::urlForDeletehash(result.image.deletehash).toString());
}

void ImgurImagesList::slotDoubleClick(QTreeWidgetItem* element, int i)
{
    if (i == URL || i == DeleteURL)
    {
        const QUrl url = QUrl(element->text(i));
        // The delete page asks for confirmation, so we don't need to do that here
        QDesktopServices::openUrl(url);
    }
}

// ------------------------------------------------------------------------------------------------

ImgurImageListViewItem::ImgurImageListViewItem(KPImagesListView* const view, const QUrl& url)
    : KPImagesListViewItem(view, url)
{
    const QColor blue(50, 50, 255);

    setTextColor(ImgurImagesList::URL, blue);
    setTextColor(ImgurImagesList::DeleteURL, blue);
}

void ImgurImageListViewItem::setTitle(const QString& str)
{
    setText(ImgurImagesList::Title, str);
}

QString ImgurImageListViewItem::Title() const
{
    return text(ImgurImagesList::Title);
}

void ImgurImageListViewItem::setDescription(const QString& str)
{
    setText(ImgurImagesList::Description, str);
}

QString ImgurImageListViewItem::Description() const
{
    return text(ImgurImagesList::Description);
}

void ImgurImageListViewItem::setImgurUrl(const QString& str)
{
    setText(ImgurImagesList::URL, str);
}

QString ImgurImageListViewItem::ImgurUrl() const
{
    return text(ImgurImagesList::URL);
}

void ImgurImageListViewItem::setImgurDeleteUrl(const QString& str)
{
    setText(ImgurImagesList::DeleteURL, str);
}

QString ImgurImageListViewItem::ImgurDeleteUrl() const
{
    return text(ImgurImagesList::DeleteURL);
}

} // namespace KIPIImgurPlugin
