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
#include <QList>

// KDE includes

#include <kmenu.h>
#include <kaction.h>
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
    m_asThumbnail = false;
}

EnfuseStackItem::~EnfuseStackItem()
{
}

void EnfuseStackItem::setEnfuseSettings(const EnfuseSettings& settings)
{
    m_settings = settings;
    setText(2, m_settings.inputImagesList());
    setToolTip(1, m_settings.asCommentString());
    setToolTip(2, m_settings.asCommentString());
}

EnfuseSettings EnfuseStackItem::enfuseSettings() const
{
    return m_settings;
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
    setText(1, fn);
}

QString EnfuseStackItem::targetFileName() const
{
    return text(1);
}

void EnfuseStackItem::setThumbnail(const QPixmap& pix)
{
    int iconSize = qMax<int>(treeWidget()->iconSize().width(), treeWidget()->iconSize().height());
    QPixmap pixmap(iconSize+2, iconSize+2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    setIcon(0, QIcon(pixmap));
    m_asThumbnail = true;
}

bool EnfuseStackItem::asThumbnail()
{
    return m_asThumbnail;
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

    setContextMenuPolicy(Qt::CustomContextMenu);
    setIconSize(QSize(64, 64));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(3);
    setHeaderHidden(false);
    setDragEnabled(false);
    header()->setResizeMode(QHeaderView::Stretch);

    QStringList labels;
    labels.append( i18n("Thumbnail") );
    labels.append( i18n("Target") );
    labels.append( i18n("Inputs") );
    setHeaderLabels(labels);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotItemClicked(QTreeWidgetItem*)));

    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(slotContextMenu(const QPoint&)));
}

EnfuseStackList::~EnfuseStackList()
{
    delete d;
}

void EnfuseStackList::slotContextMenu(const QPoint& p)
{
    KMenu popmenu(this);

    EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(itemAt(p));
    if (item)
    {
        KAction* rmItem = new KAction(KIcon("dialog-close"), i18n("Remove item"), this);
        connect(rmItem, SIGNAL(triggered(bool) ),
                this, SLOT(slotRemoveItem()));
        popmenu.addAction(rmItem);
        popmenu.addSeparator();
    }

    KAction* rmAll = new KAction(KIcon("edit-delete-shred"), i18n("Clear All"), this);
    connect(rmAll, SIGNAL(triggered(bool) ),
            this, SLOT(clear()));

    popmenu.addAction(rmAll);
    popmenu.exec(QCursor::pos());
}

void EnfuseStackList::slotRemoveItem()
{
    EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(currentItem());
    if (item)
        delete item;
}

QMap<KUrl, QString> EnfuseStackList::urlsMap()
{
    QMap<KUrl, QString> map;

    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(*it);
        if (item && item->isOn())
            map.insert(item->url(), item->targetFileName());

        ++it;
    }

    return map;
}

void EnfuseStackList::clearSelected()
{
    QList<QTreeWidgetItem*> list;
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(*it);
        if (item && item->isOn())
        {
            list.append(item);
        }
        ++it;
    }

    foreach(QTreeWidgetItem* item, list)
        delete item;
}

void EnfuseStackList::setOnItem(const KUrl& url, bool on)
{
    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        EnfuseStackItem* lvItem = dynamic_cast<EnfuseStackItem*>(*it);
        if (lvItem && lvItem->url() == url)
        {
            lvItem->setOn(on);
            break;
        }
        ++it;
    }
}

void EnfuseStackList::addItem(const KUrl& url, const EnfuseSettings& settings)
{
    if (!url.isValid())
        return;

    // Check if the new item already exist in the list.
    bool found = false;
    int count  = 0;

    QTreeWidgetItemIterator iter(this);
    while (*iter)
    {
        EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(*iter);
        if (item && item->url() == url)
            found = true;

        ++iter;
        count++;
    }

    if (!found)
    {
        EnfuseStackItem* item = new EnfuseStackItem(this);
        item->setUrl(url);
        item->setOn(true);
        item->setEnfuseSettings(settings);
        setCurrentItem(item);

        QFileInfo fi(url.toLocalFile());
        QString   temp;
        item->setTargetFileName(temp.sprintf("enfused-%02i.", count+1).append(fi.suffix()));

        emit signalItemClicked(url);
    }
}

void EnfuseStackList::setThumbnail(const KUrl& url, const QImage& img)
{
    if (img.isNull()) return;

    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(*it);
        if (item && (item->url() == url) && (!item->asThumbnail()))
        {
            item->setThumbnail(QPixmap::fromImage(img.scaled(iconSize().width(), iconSize().height(), Qt::KeepAspectRatio)));
            return;
        }
        ++it;
    }
}

void EnfuseStackList::slotItemClicked(QTreeWidgetItem* item)
{
    EnfuseStackItem* eItem = dynamic_cast<EnfuseStackItem*>(item);
    if (eItem)
        emit signalItemClicked(eItem->url());
}

}  // namespace KIPIExpoBlendingPlugin
