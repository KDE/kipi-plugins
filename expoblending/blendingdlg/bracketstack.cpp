/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "bracketstack.moc"

// Qt includes

#include <QHeaderView>
#include <QPainter>

// KDE includes

#include <kdeversion.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kio/previewjob.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kprawthumbthread.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

BracketStackItem::BracketStackItem(QTreeWidget* const parent)
                : QTreeWidgetItem(parent)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    setCheckState(0, Qt::Unchecked);
    setThumbnail(SmallIcon("image-x-generic", treeWidget()->iconSize().width(), KIconLoader::DisabledState));
}

BracketStackItem::~BracketStackItem()
{
}

void BracketStackItem::setUrl(const KUrl& url)
{
    m_url = url;
    setText(1, m_url.fileName());
}

KUrl BracketStackItem::url() const
{
    return m_url;
}

void BracketStackItem::setThumbnail(const QPixmap& pix)
{
    int iconSize = qMax<int>(treeWidget()->iconSize().width(), treeWidget()->iconSize().height());
    QPixmap pixmap(iconSize+2, iconSize+2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    setIcon(0, QIcon(pixmap));
}

void BracketStackItem::setExposure(const QString& exp)
{
    setText(2, exp);
}

bool BracketStackItem::isOn() const
{
    return (checkState(0) == Qt::Checked ? true : false);
}

void BracketStackItem::setOn(bool b)
{
    setCheckState(0, b ? Qt::Checked : Qt::Unchecked);
}

bool BracketStackItem::operator< (const QTreeWidgetItem& other) const
{
    int column     = treeWidget()->sortColumn();
    double thisEv  = text(column).toDouble();
    double otherEv = other.text(column).toDouble();
    return thisEv < otherEv;
}

// -------------------------------------------------------------------------

class BracketStackList::BracketStackListPriv
{
public:

    BracketStackListPriv()
    {
        iface        = 0;
        loadRawThumb = 0;
    }

    Interface*        iface;
    KPRawThumbThread* loadRawThumb;
};

BracketStackList::BracketStackList(Interface* iface, QWidget* parent)
                : QTreeWidget(parent), d(new BracketStackListPriv)
{
    d->iface = iface;

    setIconSize(QSize(64, 64));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSortingEnabled(true);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(3);
    setHeaderHidden(false);
    setDragEnabled(false);
    header()->setResizeMode(QHeaderView::Stretch);

    QStringList labels;
    labels.append( i18n("To process") );
    labels.append( i18n("File Name") );
    labels.append( i18n("Exposure (EV)") );
    setHeaderLabels(labels);

    if (d->iface)
    {
        connect(d->iface, SIGNAL(gotThumbnail(KUrl,QPixmap)),
                this, SLOT(slotThumbnail(KUrl,QPixmap)));
    }

    d->loadRawThumb = new KPRawThumbThread(this);

    connect(d->loadRawThumb, SIGNAL(signalRawThumb(KUrl,QImage)),
            this, SLOT(slotRawThumb(KUrl,QImage)));

    sortItems(2, Qt::DescendingOrder);
}

BracketStackList::~BracketStackList()
{
    delete d;
}

KUrl::List BracketStackList::urls()
{
    KUrl::List list;

    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        BracketStackItem* item = dynamic_cast<BracketStackItem*>(*it);
        if (item && item->isOn())
            list.append(item->url());

        ++it;
    }

    return list;
}

BracketStackItem* BracketStackList::findItem(const KUrl& url)
{
    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        BracketStackItem* lvItem = dynamic_cast<BracketStackItem*>(*it);
        if (lvItem && lvItem->url() == url)
        {
            return lvItem;
        }
        ++it;
    }
    return 0;
}

void BracketStackList::addItems(const KUrl::List& list)
{
    if (list.count() == 0)
        return;

    KUrl::List urls;

    for ( KUrl::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it )
    {
        KUrl imageUrl = *it;

        // Check if the new item already exist in the list.
        bool found = false;

        QTreeWidgetItemIterator iter(this);
        while (*iter)
        {
            BracketStackItem* item = dynamic_cast<BracketStackItem*>(*iter);

            if (item->url() == imageUrl)
                found = true;

            ++iter;
        }

        if (!found)
        {
            BracketStackItem* item = new BracketStackItem(this);
            item->setUrl(imageUrl);
            item->setOn(true);
            urls.append(imageUrl);
        }
    }

    if (d->iface)
    {
        d->iface->thumbnails(urls, iconSize().width());
    }
    else
    {
#if KDE_IS_VERSION(4,7,0)
        KFileItemList items;
        foreach (const KUrl& url, urls)
        {
            if (url.isValid())
                items.append(KFileItem(KFileItem::Unknown, KFileItem::Unknown, url, true));
        }
        KIO::PreviewJob* job = KIO::filePreview(items, iconSize());
#else
        KIO::PreviewJob *job = KIO::filePreview(urls, iconSize().width());
#endif

        connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)),
                this, SLOT(slotKDEPreview(KFileItem,QPixmap)));

        connect(job, SIGNAL(failed(KFileItem)),
                this, SLOT(slotKDEPreviewFailed(KFileItem)));
    }

    emit signalAddItems(urls);
}

// Used only if Kipi interface is null.
void BracketStackList::slotKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    if (!pix.isNull())
        slotThumbnail(item.url(), pix);
}

void BracketStackList::slotKDEPreviewFailed(const KFileItem& item)
{
    d->loadRawThumb->getRawThumb(item.url());
}

void BracketStackList::slotRawThumb(const KUrl& url, const QImage& img)
{
    slotThumbnail(url, QPixmap::fromImage(img));
}

void BracketStackList::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        BracketStackItem* item = static_cast<BracketStackItem*>(*it);
        if (item->url() == url)
        {
            if (pix.isNull())
                item->setThumbnail(SmallIcon("image-x-generic", iconSize().width(), KIconLoader::DisabledState));
            else
                item->setThumbnail(pix.scaled(iconSize().width(), iconSize().height(), Qt::KeepAspectRatio));

            return;
        }
        ++it;
    }
}

}  // namespace KIPIExpoBlendingPlugin
