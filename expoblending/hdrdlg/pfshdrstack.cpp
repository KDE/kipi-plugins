/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-09-04
 * Description : a tool to blend bracketed images/create HDR images.
 *
 * Copyright (C) 2013 by Soumajyoti Sarkar <ergy dot ergy at gmail dot com>
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

#include "pfshdrstack.moc"

// Qt includes

#include <QHeaderView>
#include <QPainter>
#include <QFileInfo>
#include <QList>
#include <QTimer>

// KDE includes

#include <kmenu.h>
#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kio/previewjob.h>
#include <kpixmapsequence.h>

// LibKIPI includes

#include <libkipi/interface.h>

namespace KIPIExpoBlendingPlugin
{

class PfsHdrStackItem::PfsHdrStackItemPriv
{
public:

    PfsHdrStackItemPriv()
    {
        asValidThumb = false;
    }

    bool           asValidThumb;
    QPixmap        thumb;
    PfsHdrSettings settings;
};

PfsHdrStackItem::PfsHdrStackItem(QTreeWidget* const parent)
    : QTreeWidgetItem(parent), d(new PfsHdrStackItemPriv)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    setCheckState(0, Qt::Unchecked);
    setThumbnail(SmallIcon("image-x-generic", treeWidget()->iconSize().width(), KIconLoader::DisabledState));
    d->asValidThumb = false;
}

PfsHdrStackItem::~PfsHdrStackItem()
{
    delete d;
}

void PfsHdrStackItem::setPfsHdrSettings(const PfsHdrSettings& settings)
{
    d->settings = settings;
    setText(1, d->settings.targetFileName);
    setText(2, d->settings.inputImagesList());
    setToolTip(2, d->settings.inputImagesList().replace(" ; ", "\n"));
}

PfsHdrSettings PfsHdrStackItem::pfshdrSettings() const
{
    return d->settings;
}

KUrl PfsHdrStackItem::url() const
{
    return d->settings.previewUrl;
}

void PfsHdrStackItem::setProgressAnimation(const QPixmap& pix)
{
    QPixmap overlay = d->thumb;
    QPixmap mask(overlay.size());
    mask.fill(QColor(128, 128, 128, 192));
    QPainter p(&overlay);
    p.drawPixmap(0, 0, mask);
    p.drawPixmap((overlay.width()/2) - (pix.width()/2), (overlay.height()/2) - (pix.height()/2), pix);
    setIcon(0, QIcon(overlay));
}

void PfsHdrStackItem::setThumbnail(const QPixmap& pix)
{
    int iconSize = qMax<int>(treeWidget()->iconSize().width(), treeWidget()->iconSize().height());
    QPixmap pixmap(iconSize+2, iconSize+2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    d->thumb = pixmap;
    setIcon(0, QIcon(pixmap));
    d->asValidThumb = true;
}

void PfsHdrStackItem::setProcessedIcon(const QIcon& icon)
{
    setIcon(1, icon);
    setIcon(0, QIcon(d->thumb));
}

bool PfsHdrStackItem::asValidThumb() const
{
    return d->asValidThumb;
}

bool PfsHdrStackItem::isOn() const
{
    return (checkState(0) == Qt::Checked ? true : false);
}

void PfsHdrStackItem::setOn(bool b)
{
    setCheckState(0, b ? Qt::Checked : Qt::Unchecked);
}

// -------------------------------------------------------------------------

class PfsHdrStackList::PfsHdrStackListPriv
{
public:

    PfsHdrStackListPriv()
    {
        outputFormat = KPSaveSettingsWidget::OUTPUT_PNG;
        progressPix   = KPixmapSequence("process-working", KIconLoader::SizeSmallMedium);
        progressCount = 0;
        progressTimer = 0;
        processItem   = 0;
    }

    KPSaveSettingsWidget::OutputFormat outputFormat;

    QString                          templateFileName;

    int                              progressCount;
    QTimer*                          progressTimer;
    KPixmapSequence                  progressPix;
    PfsHdrStackItem*                 processItem;
};

PfsHdrStackList::PfsHdrStackList(QWidget* const parent)
    : QTreeWidget(parent), d(new PfsHdrStackListPriv)
{
    d->progressTimer = new QTimer(this);

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
    labels.append( i18n("To Save") );
    labels.append( i18n("Target") );
    labels.append( i18n("Inputs") );
    setHeaderLabels(labels);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotItemClicked(QTreeWidgetItem*)));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotContextMenu(QPoint)));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));
}

PfsHdrStackList::~PfsHdrStackList()
{
    delete d;
}

void PfsHdrStackList::slotContextMenu(const QPoint& p)
{
    KMenu popmenu(this);

    PfsHdrStackItem* item = dynamic_cast<PfsHdrStackItem*>(itemAt(p));
    if (item)
    {
        KAction* rmItem = new KAction(KIcon("dialog-close"), i18n("Remove item"), this);
        connect(rmItem, SIGNAL(triggered(bool)),
                this, SLOT(slotRemoveItem()));
        popmenu.addAction(rmItem);
        popmenu.addSeparator();
    }

    KAction* rmAll = new KAction(KIcon("edit-delete-shred"), i18n("Clear all"), this);
    connect(rmAll, SIGNAL(triggered(bool)),
            this, SLOT(clear()));

    popmenu.addAction(rmAll);
    popmenu.exec(QCursor::pos());
}

void PfsHdrStackList::slotRemoveItem()
{
    PfsHdrStackItem* item = dynamic_cast<PfsHdrStackItem*>(currentItem());
    delete item;
}

QList<PfsHdrSettings> PfsHdrStackList::settingsList()
{
    QList<PfsHdrSettings> list;

    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        PfsHdrStackItem* item = dynamic_cast<PfsHdrStackItem*>(*it);
        if (item && item->isOn())
            list.append(item->pfshdrSettings());

        ++it;
    }

    return list;
}

void PfsHdrStackList::clearSelected()
{
    QList<QTreeWidgetItem*> list;
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        PfsHdrStackItem* item = dynamic_cast<PfsHdrStackItem*>(*it);
        if (item && item->isOn())
        {
            list.append(item);
        }
        ++it;
    }

    foreach(QTreeWidgetItem* item, list)
        delete item;
}

void PfsHdrStackList::setOnItem(const KUrl& url, bool on)
{
    PfsHdrStackItem* item = findItemByUrl(url);
    if (item)
        item->setOn(on);
}

void PfsHdrStackList::removeItem(const KUrl& url)
{
    PfsHdrStackItem* item = findItemByUrl(url);
    delete item;
}

void PfsHdrStackList::addItem(const KUrl& url, const PfsHdrSettings& settings)
{
    if (!url.isValid())
        return;

    // Check if the new item already exist in the list.
    if (!findItemByUrl(url))
    {
        PfsHdrSettings pfshdrPrms = settings;
        QString ext               = KPSaveSettingsWidget::extensionForFormat(pfshdrPrms.outputFormat);
        pfshdrPrms.previewUrl     = url;

        PfsHdrStackItem* item = new PfsHdrStackItem(this);
        item->setPfsHdrSettings(pfshdrPrms);
        item->setOn(true);
        setCurrentItem(item);
        setTemplateFileName(d->outputFormat, d->templateFileName);

        emit signalItemClicked(url);
    }
}

void PfsHdrStackList::setThumbnail(const KUrl& url, const QImage& img)
{
    if (img.isNull()) return;

    PfsHdrStackItem* item = findItemByUrl(url);
    if (item && (!item->asValidThumb()))
        item->setThumbnail(QPixmap::fromImage(img.scaled(iconSize().width(), iconSize().height(), Qt::KeepAspectRatio)));
}

void PfsHdrStackList::slotItemClicked(QTreeWidgetItem* item)
{
    PfsHdrStackItem* eItem = dynamic_cast<PfsHdrStackItem*>(item);
    if (eItem)
        emit signalItemClicked(eItem->url());
}

void PfsHdrStackList::slotProgressTimerDone()
{
    d->processItem->setProgressAnimation(d->progressPix.frameAt(d->progressCount));
    d->progressCount++;
    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}

PfsHdrStackItem* PfsHdrStackList::findItemByUrl(const KUrl& url)
{
    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        PfsHdrStackItem* item = dynamic_cast<PfsHdrStackItem*>(*it);
        if (item && (item->url() == url))
            return item;

        ++it;
    }
    return 0;
}

void PfsHdrStackList::processingItem(const KUrl& url, bool run)
{
    d->processItem = findItemByUrl(url);
    if (d->processItem)
    {
        if (run)
        {
            setCurrentItem(d->processItem, true);
            scrollToItem(d->processItem);
            d->progressTimer->start(300);
        }
        else
        {
            d->progressTimer->stop();
            d->processItem = 0;
        }
    }
}

void PfsHdrStackList::processedItem(const KUrl& url, bool success)
{
    PfsHdrStackItem* item = findItemByUrl(url);
    if (item)
        item->setProcessedIcon(SmallIcon(success ? "dialog-ok" : "dialog-cancel"));
}

void PfsHdrStackList::setTemplateFileName(KPSaveSettingsWidget::OutputFormat frm, const QString& string)
{
    d->outputFormat     = frm;
    d->templateFileName = string;
    int count           = 0;

    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        PfsHdrStackItem* item = dynamic_cast<PfsHdrStackItem*>(*it);
        if (item)
        {
            QString temp;
            PfsHdrSettings settings = item->pfshdrSettings();
            QString ext             = KPSaveSettingsWidget::extensionForFormat(d->outputFormat);
            settings.outputFormat   = d->outputFormat;
            settings.targetFileName = d->templateFileName + temp.sprintf("-%02i", count+1).append(ext);
            item->setPfsHdrSettings(settings);
        }
        ++it;
        count++;
    }
}

}  // namespace KIPIExpoBlendingPlugin
