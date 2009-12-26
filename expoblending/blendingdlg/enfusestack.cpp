/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "enfusestack.moc"

// Qt includes

#include <QHeaderView>
#include <QPainter>
#include <QFileInfo>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kio/previewjob.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local include

#include "manager.h"

namespace KIPIExpoBlendingPlugin
{

EnfuseStackItem::EnfuseStackItem(QTreeWidget* parent)
               : QTreeWidgetItem(parent)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    setCheckState(0, Qt::Unchecked);
    setThumbnail(SmallIcon("image-x-generic", treeWidget()->iconSize().width(), KIconLoader::DisabledState));
}

EnfuseStackItem::~EnfuseStackItem()
{
}

void EnfuseStackItem::setUrl(const KUrl& url)
{
    m_url = url;
}

KUrl EnfuseStackItem::url() const
{
    return m_url;
}

void EnfuseStackItem::setTargetFileName(const QString& fn)
{
    m_fileName = fn;
}

QString EnfuseStackItem::targetFileName() const
{
    return m_fileName;
}

void EnfuseStackItem::setThumbnail(const QPixmap& pix)
{
    int iconSize = qMax<int>(treeWidget()->iconSize().width(), treeWidget()->iconSize().height());
    QPixmap pixmap(iconSize+2, iconSize+2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    setIcon(0, QIcon(pixmap));
}

bool EnfuseStackItem::isOn() const
{
    return (checkState(0) == Qt::Checked ? true : false);
}

void EnfuseStackItem::setOn(bool b)
{
    setCheckState(0, b ? Qt::Checked : Qt::Unchecked);
}

// -------------------------------------------------------------------------

class EnfuseStackListPriv
{
public:

    EnfuseStackListPriv()
    {
        mngr = 0;
    }

    Manager* mngr;
};

EnfuseStackList::EnfuseStackList(Manager* mngr, QWidget* parent)
               : QTreeWidget(parent), d(new EnfuseStackListPriv)
{
    d->mngr = mngr;

    setIconSize(QSize(64, 64));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(2);
    setHeaderHidden(false);
    setDragEnabled(false);
    header()->setResizeMode(QHeaderView::Stretch);

    QStringList labels;
    labels.append( i18n("Thumbnail") );
    labels.append( i18n("Format") );
    setHeaderLabels(labels);

    if (d->mngr->iface())
    {
        connect(d->mngr->iface(), SIGNAL(gotThumbnail(const KUrl&, const QPixmap&)),
                this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));
    }
}

EnfuseStackList::~EnfuseStackList()
{
    delete d;
}

KUrl::List EnfuseStackList::urls()
{
    KUrl::List list;

    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(*it);
        if (item && item->isOn())
            list.append(item->url());

        ++it;
    }

    return list;
}

void EnfuseStackList::addItem(const KUrl& url)
{
    if (!url.isValid())
        return;

    // Check if the new item already exist in the list.
    bool found = false;

    QTreeWidgetItemIterator iter(this);
    while (*iter)
    {
        EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(*iter);
        if (item->url() == url)
            found = true;

        ++iter;
    }

    if (!found)
    {
        EnfuseStackItem* item = new EnfuseStackItem(this);
        item->setUrl(url);
        item->setOn(true);
    }

    if (d->mngr->iface())
    {
        d->mngr->iface()->thumbnails(url, iconSize().width());
    }
    else
    {
        KIO::PreviewJob *job = KIO::filePreview(url, iconSize().width());

        connect(job, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)),
                this, SLOT(slotKDEPreview(const KFileItem&, const QPixmap&)));
    }
}

// Used only if Kipi interface is null.
void EnfuseStackList::slotKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    if (!pix.isNull())
        slotThumbnail(item.url(), pix);
}

void EnfuseStackList::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(*it);
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
