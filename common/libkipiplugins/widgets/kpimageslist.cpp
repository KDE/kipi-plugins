/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2008-05-21
 * Description : widget to display an imagelist
 *
 * Copyright (C) 2006-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2010 by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2009-2010 by Luka Renko <lure at kubuntu dot org>
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

#include "kpimageslist.h"

// Qt includes

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QMimeData>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QUrl>
#include <QTimer>
#include <QFile>
#include <QPointer>
#include <QXmlStreamAttributes>
#include <QStringRef>
#include <QString>
#include <QStandardPaths>
#include <QFileDialog>
#include <QIcon>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/ImageCollection>
#include <KIPI/Interface>
#include <KIPI/PluginLoader>

// Local includes

#include "kpimageinfo.h"
#include "kpimagedialog.h"
#include "kipiplugins_debug.h"
#include "kputil.h"

using namespace KIPIPlugins;

namespace KIPIPlugins
{

const int DEFAULTSIZE = 48;

class KPImagesListViewItem::Private
{
public:

    Private()
    {
        rating   = -1;
        view     = nullptr;
        state    = Waiting;
        hasThumb = false;
    }

    bool              hasThumb;       // True if thumbnails is a real photo thumbs

    int               rating;         // Image Rating from Kipi host.
    QString           comments;       // Image comments from Kipi host.
    QStringList       tags;           // List of keywords from Kipi host.
    QUrl              url;            // Image url provided by Kipi host.
    QPixmap           thumb;          // Image thumbnail.
    KPImagesListView* view;
    State             state;
};

KPImagesListViewItem::KPImagesListViewItem(KPImagesListView* const view, const QUrl& url)
    : QTreeWidgetItem(view),
      d(new Private)
{
    setUrl(url);
    setRating(-1);
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);

    d->view      = view;
    int iconSize = d->view->iconSize().width();
    setThumb(QIcon::fromTheme(QString::fromLatin1("image-x-generic")).pixmap(iconSize, iconSize, QIcon::Disabled), false);

    qCDebug(KIPIPLUGINS_LOG) << "Creating new ImageListViewItem with url " << d->url
                             << " for list view " << d->view;
}

KPImagesListViewItem::~KPImagesListViewItem()
{
    delete d;
}

bool KPImagesListViewItem::hasValidThumbnail() const
{
    return d->hasThumb;
}

void KPImagesListViewItem::updateInformation()
{
    if (d->view->iface())
    {
        KPImageInfo info(d->url);

        setComments(info.description());

        setTags(QStringList());

        if (d->view->iface()->hasFeature(HostSupportsTags))
        {
            setTags(info.keywords());
        }

        if (d->view->iface()->hasFeature(HostSupportsRating))
        {
            setRating(info.rating());
        }
    }
}

void KPImagesListViewItem::setUrl(const QUrl& url)
{
    d->url = url;
    setText(KPImagesListView::Filename, d->url.fileName());
}

QUrl KPImagesListViewItem::url() const
{
    return d->url;
}

void KPImagesListViewItem::setComments(const QString& comments)
{
    d->comments = comments;
}

QString KPImagesListViewItem::comments() const
{
    return d->comments;
}

void KPImagesListViewItem::setTags(const QStringList& tags)
{
    d->tags = tags;
}

QStringList KPImagesListViewItem::tags() const
{
    return d->tags;
}

void KPImagesListViewItem::setRating(int rating)
{
    d->rating = rating;
}

int KPImagesListViewItem::rating() const
{
    return d->rating;
}

void KPImagesListViewItem::setPixmap(const QPixmap& pix)
{
    QIcon icon = QIcon(pix);
    //  We make sure the preview icon stays the same regardless of the role
    icon.addPixmap(pix, QIcon::Selected, QIcon::On);
    icon.addPixmap(pix, QIcon::Selected, QIcon::Off);
    icon.addPixmap(pix, QIcon::Active,   QIcon::On);
    icon.addPixmap(pix, QIcon::Active,   QIcon::Off);
    icon.addPixmap(pix, QIcon::Normal,   QIcon::On);
    icon.addPixmap(pix, QIcon::Normal,   QIcon::Off);
    setIcon(KPImagesListView::Thumbnail, icon);
}

void KPImagesListViewItem::setThumb(const QPixmap& pix, bool hasThumb)
{
    if (hasThumb)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Received new thumbnail for url "
                                 << d->url << " for view " << d->view;
    }

    if (!d->view)
    {
        qCCritical(KIPIPLUGINS_LOG) << "This item doesn't have a tree view. "
                                    << "This should never happen!";
        return;
    }

    int iconSize = qMax<int>(d->view->iconSize().width(), d->view->iconSize().height());
    QPixmap pixmap(iconSize + 2, iconSize + 2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width() / 2) - (pix.width() / 2), (pixmap.height() / 2) - (pix.height() / 2), pix);
    d->thumb     = pixmap;
    setPixmap(d->thumb);

    d->hasThumb  = hasThumb;
}

void KPImagesListViewItem::setProgressAnimation(const QPixmap& pix)
{
    QPixmap overlay = d->thumb;
    QPixmap mask(overlay.size());
    mask.fill(QColor(128, 128, 128, 192));
    QPainter p(&overlay);
    p.drawPixmap(0, 0, mask);
    p.drawPixmap((overlay.width() / 2) - (pix.width() / 2), (overlay.height() / 2) - (pix.height() / 2), pix);
    setPixmap(overlay);
}

void KPImagesListViewItem::setProcessedIcon(const QIcon& icon)
{
    setIcon(KPImagesListView::Filename, icon);
    // reset thumbnail back to no animation pix
    setPixmap(d->thumb);
}

void KPImagesListViewItem::setState(State state)
{
    d->state = state;
}

KPImagesListViewItem::State KPImagesListViewItem::state() const
{
    return d->state;
}

KPImagesListView* KPImagesListViewItem::view() const
{
    return d->view;
}

// ---------------------------------------------------------------------------

KPImagesListView::KPImagesListView(KPImagesList* const parent)
    : QTreeWidget(parent)
{
    setup(DEFAULTSIZE);
}

KPImagesListView::KPImagesListView(int iconSize, KPImagesList* const parent)
    : QTreeWidget(parent)
{
    setup(iconSize);
}

KPImagesListView::~KPImagesListView()
{
}

void KPImagesListView::setup(int iconSize)
{
    m_iconSize = iconSize;
    setIconSize(QSize(m_iconSize, m_iconSize));
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    enableDragAndDrop(true);

    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setColumnCount(8);
    setHeaderLabels(QStringList() << i18n("Thumbnail")
                                  << i18n("File Name")
                                  << i18n("User1")
                                  << i18n("User2")
                                  << i18n("User3")
                                  << i18n("User4")
                                  << i18n("User5")
                                  << i18n("User6"));
    hideColumn(User1);
    hideColumn(User2);
    hideColumn(User3);
    hideColumn(User4);
    hideColumn(User5);
    hideColumn(User6);

    header()->setSectionResizeMode(User1, QHeaderView::Interactive);
    header()->setSectionResizeMode(User2, QHeaderView::Interactive);
    header()->setSectionResizeMode(User3, QHeaderView::Interactive);
    header()->setSectionResizeMode(User4, QHeaderView::Interactive);
    header()->setSectionResizeMode(User5, QHeaderView::Interactive);
    header()->setSectionResizeMode(User6, QHeaderView::Stretch);

    connect(this, &KPImagesListView::itemClicked,
            this, &KPImagesListView::slotItemClicked);
}

void KPImagesListView::enableDragAndDrop(const bool enable)
{
    setDragEnabled(enable);
    viewport()->setAcceptDrops(enable);
    setDragDropMode(enable ? QAbstractItemView::InternalMove : QAbstractItemView::NoDragDrop);
    setDragDropOverwriteMode(enable);
    setDropIndicatorShown(enable);
}

void KPImagesListView::drawRow(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const
{
    KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(itemFromIndex(index));

    if (item && !item->hasValidThumbnail())
    {
        KPImagesList* const view = dynamic_cast<KPImagesList*>(parent());

        if (view)
        {
            view->updateThumbnail(item->url());
        }
    }

    QTreeWidget::drawRow(p, opt, index);
}

void KPImagesListView::slotItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)

    if (!item)
    {
        return;
    }

    emit signalItemClicked(item);
}

void KPImagesListView::setColumnLabel(ColumnType column, const QString& label)
{
    headerItem()->setText(column, label);
}

void KPImagesListView::setColumnEnabled(ColumnType column, bool enable)
{
    if (enable)
    {
        showColumn(column);
    }
    else
    {
        hideColumn(column);
    }
}

void KPImagesListView::setColumn(ColumnType column, const QString& label, bool enable)
{
    setColumnLabel(column, label);
    setColumnEnabled(column, enable);
}

KPImagesListViewItem* KPImagesListView::findItem(const QUrl& url)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        KPImagesListViewItem* const lvItem = dynamic_cast<KPImagesListViewItem*>(*it);

        if (lvItem && lvItem->url() == url)
        {
            return lvItem;
        }

        ++it;
    }

    return nullptr;
}

QModelIndex KPImagesListView::indexFromItem(KPImagesListViewItem* item, int column) const
{
  return QTreeWidget::indexFromItem(item, column);
}

void KPImagesListView::contextMenuEvent(QContextMenuEvent* e)
{
    QTreeWidget::contextMenuEvent(e);
    emit signalContextMenuRequested();
}

void KPImagesListView::dragEnterEvent(QDragEnterEvent* e)
{
    QTreeWidget::dragEnterEvent(e);

    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void KPImagesListView::dragMoveEvent(QDragMoveEvent* e)
{
    QTreeWidget::dragMoveEvent(e);

    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void KPImagesListView::dropEvent(QDropEvent* e)
{
    QTreeWidget::dropEvent(e);
    QList<QUrl> list = e->mimeData()->urls();
    QList<QUrl> urls;

    foreach(const QUrl& url, list)
    {
        QFileInfo fi(url.toLocalFile());

        if (fi.isFile() && fi.exists())
        {
            urls.append(url);
        }
    }

    if (!urls.isEmpty())
    {
        emit signalAddedDropedItems(urls);
    }
}

Interface* KPImagesListView::iface() const
{
    KPImagesList* const p = dynamic_cast<KPImagesList*>(parent());

    if (p)
    {
        return p->iface();
    }

    return nullptr;
}

// ---------------------------------------------------------------------------

CtrlButton::CtrlButton(const QIcon& icon, QWidget* const parent)
    : QPushButton(parent)
{
    const int btnSize = 32;

    setMinimumSize(btnSize, btnSize);
    setMaximumSize(btnSize, btnSize);
    setIcon(icon);
}

CtrlButton::~CtrlButton()
{
}

// ---------------------------------------------------------------------------

class KPImagesList::Private
{
public:

    Private()
    {
        listView               = nullptr;
        iface                  = nullptr;
        addButton              = nullptr;
        removeButton           = nullptr;
        moveUpButton           = nullptr;
        moveDownButton         = nullptr;
        clearButton            = nullptr;
        loadButton             = nullptr;
        saveButton             = nullptr;
        iconSize               = DEFAULTSIZE;
        allowRAW               = true;
        controlButtonsEnabled  = true;
        allowDuplicate         = false;
        progressCount          = 0;
        progressTimer          = nullptr;
        progressPix            = KPWorkingPixmap();
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

    bool                       allowRAW;
    bool                       allowDuplicate;
    bool                       controlButtonsEnabled;
    int                        iconSize;

    CtrlButton*                addButton;
    CtrlButton*                removeButton;
    CtrlButton*                moveUpButton;
    CtrlButton*                moveDownButton;
    CtrlButton*                clearButton;
    CtrlButton*                loadButton;
    CtrlButton*                saveButton;

    QList<QUrl>                processItems;
    KPWorkingPixmap            progressPix;
    int                        progressCount;
    QTimer*                    progressTimer;

    KPImagesListView*          listView;
    Interface*                 iface;
};

KPImagesList::KPImagesList(QWidget* const parent, int iconSize)
    : QWidget(parent),
      d(new Private)
{
    if (iconSize != -1)  // default = ICONSIZE
    {
        setIconSize(iconSize);
    }

    // --------------------------------------------------------

    d->listView = new KPImagesListView(d->iconSize, this);
    d->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // --------------------------------------------------------

    d->addButton      = new CtrlButton(QIcon::fromTheme(QString::fromLatin1("list-add")).pixmap(16, 16),      this);
    d->removeButton   = new CtrlButton(QIcon::fromTheme(QString::fromLatin1("list-remove")).pixmap(16, 16),   this);
    d->moveUpButton   = new CtrlButton(QIcon::fromTheme(QString::fromLatin1("go-up")).pixmap(16, 16),         this);
    d->moveDownButton = new CtrlButton(QIcon::fromTheme(QString::fromLatin1("go-down")).pixmap(16, 16),       this);
    d->clearButton    = new CtrlButton(QIcon::fromTheme(QString::fromLatin1("edit-clear")).pixmap(16, 16),    this);
    d->loadButton     = new CtrlButton(QIcon::fromTheme(QString::fromLatin1("document-open")).pixmap(16, 16), this);
    d->saveButton     = new CtrlButton(QIcon::fromTheme(QString::fromLatin1("document-save")).pixmap(16, 16), this);

    d->addButton->setToolTip(i18n("Add new images to the list"));
    d->removeButton->setToolTip(i18n("Remove selected images from the list"));
    d->moveUpButton->setToolTip(i18n("Move current selected image up in the list"));
    d->moveDownButton->setToolTip(i18n("Move current selected image down in the list"));
    d->clearButton->setToolTip(i18n("Clear the list."));
    d->loadButton->setToolTip(i18n("Load a saved list."));
    d->saveButton->setToolTip(i18n("Save the list."));

    d->progressTimer = new QTimer(this);

    // --------------------------------------------------------

    setControlButtons(Add | Remove | MoveUp | MoveDown | Clear | Save | Load ); // add all buttons      (default)
    setControlButtonsPlacement(ControlButtonsRight);                            // buttons on the right (default)
    enableDragAndDrop(true);                                                    // enable drag and drop (default)

    // --------------------------------------------------------

    connect(d->listView, &KPImagesListView::signalAddedDropedItems,
            this, &KPImagesList::slotAddImages);

    if (d->iface)
    {
        connect(d->iface, &Interface::gotThumbnail,
                this, &KPImagesList::slotThumbnail);
    }

    connect(d->listView, &KPImagesListView::signalItemClicked,
            this, &KPImagesList::signalItemClicked);

    connect(d->listView, &KPImagesListView::signalContextMenuRequested,
            this, &KPImagesList::signalContextMenuRequested);

    // queue this connection because itemSelectionChanged is emitted
    // while items are deleted, and accessing selectedItems at that
    // time causes a crash ...
    connect(d->listView, &KPImagesListView::itemSelectionChanged,
            this, &KPImagesList::slotImageListChanged, Qt::QueuedConnection);

    connect(this, &KPImagesList::signalImageListChanged,
            this, &KPImagesList::slotImageListChanged);

    // --------------------------------------------------------

    connect(d->addButton, &CtrlButton::clicked,
            this, &KPImagesList::slotAddItems);

    connect(d->removeButton, &CtrlButton::clicked,
            this, &KPImagesList::slotRemoveItems);

    connect(d->moveUpButton, &CtrlButton::clicked,
            this, &KPImagesList::slotMoveUpItems);

    connect(d->moveDownButton, &CtrlButton::clicked,
            this, &KPImagesList::slotMoveDownItems);

    connect(d->clearButton, &CtrlButton::clicked,
            this, &KPImagesList::slotClearItems);

    connect(d->loadButton, &CtrlButton::clicked,
            this, &KPImagesList::slotLoadItems);

    connect(d->saveButton, &CtrlButton::clicked,
            this, &KPImagesList::slotSaveItems);

    connect(d->progressTimer, &QTimer::timeout,
            this, &KPImagesList::slotProgressTimerDone);

    // --------------------------------------------------------

    emit signalImageListChanged();
}

void KPImagesList::enableControlButtons(bool enable)
{
    d->controlButtonsEnabled = enable;
    slotImageListChanged();
}

void KPImagesList::enableDragAndDrop(const bool enable)
{
    d->listView->enableDragAndDrop(enable);
}

void KPImagesList::setControlButtonsPlacement(ControlButtonPlacement placement)
{
    delete layout();

    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QGridLayout* const mainLayout = new QGridLayout;
    mainLayout->addWidget(d->listView, 1, 1, 1, 1);
    mainLayout->setRowStretch(1, 10);
    mainLayout->setColumnStretch(1, 10);
    mainLayout->setContentsMargins(spacing, spacing, spacing, spacing);
    mainLayout->setSpacing(spacing);

    // --------------------------------------------------------

    QHBoxLayout* const hBtnLayout = new QHBoxLayout;
    hBtnLayout->addStretch(10);
    hBtnLayout->addWidget(d->moveUpButton);
    hBtnLayout->addWidget(d->moveDownButton);
    hBtnLayout->addWidget(d->addButton);
    hBtnLayout->addWidget(d->removeButton);
    hBtnLayout->addWidget(d->loadButton);
    hBtnLayout->addWidget(d->saveButton);
    hBtnLayout->addWidget(d->clearButton);
    hBtnLayout->addStretch(10);

    // --------------------------------------------------------

    QVBoxLayout* const vBtnLayout = new QVBoxLayout;
    vBtnLayout->addStretch(10);
    vBtnLayout->addWidget(d->moveUpButton);
    vBtnLayout->addWidget(d->moveDownButton);
    vBtnLayout->addWidget(d->addButton);
    vBtnLayout->addWidget(d->removeButton);
    vBtnLayout->addWidget(d->loadButton);
    vBtnLayout->addWidget(d->saveButton);
    vBtnLayout->addWidget(d->clearButton);
    vBtnLayout->addStretch(10);

    // --------------------------------------------------------

    switch (placement)
    {
        case ControlButtonsAbove:
            mainLayout->addLayout(hBtnLayout, 0, 1, 1, 1);
            delete vBtnLayout;
            break;

        case ControlButtonsBelow:
            mainLayout->addLayout(hBtnLayout, 2, 1, 1, 1);
            delete vBtnLayout;
            break;

        case ControlButtonsLeft:
            mainLayout->addLayout(vBtnLayout, 1, 0, 1, 1);
            delete hBtnLayout;
            break;

        case ControlButtonsRight:
            mainLayout->addLayout(vBtnLayout, 1, 2, 1, 1);
            delete hBtnLayout;
            break;

        case NoControlButtons:
        default:
        {
            delete vBtnLayout;
            delete hBtnLayout;
            // set all buttons invisible
            setControlButtons(nullptr);
            break;
        }
    }

    setLayout(mainLayout);
}

void KPImagesList::setControlButtons(ControlButtons buttonMask)
{
    d->addButton->setVisible(buttonMask & Add);
    d->removeButton->setVisible(buttonMask & Remove);
    d->moveUpButton->setVisible(buttonMask & MoveUp);
    d->moveDownButton->setVisible(buttonMask & MoveDown);
    d->clearButton->setVisible(buttonMask & Clear);
    d->loadButton->setVisible(buttonMask & Load);
    d->saveButton->setVisible(buttonMask & Save);
}

KPImagesList::~KPImagesList()
{
    delete d;
}

void KPImagesList::setAllowDuplicate(bool allow)
{
  d->allowDuplicate = allow;
}

void KPImagesList::setAllowRAW(bool allow)
{
    d->allowRAW = allow;
}

void KPImagesList::setIconSize(int size)
{
    if (size < 16)
    {
        d->iconSize = 16;
    }
    else if (size > 128)
    {
        d->iconSize = 128;
    }
    else
    {
        d->iconSize = size;
    }
}

int KPImagesList::iconSize() const
{
    return d->iconSize;
}

void KPImagesList::loadImagesFromCurrentSelection()
{
    if (!d->iface)
    {
        return;
    }

    ImageCollection selection = d->iface->currentSelection();

    if (!selection.images().isEmpty())
    {
        if (selection.isValid())
        {
            slotAddImages(selection.images());
        }
    }
    else
    {
        loadImagesFromCurrentAlbum();
    }
}

void KPImagesList::loadImagesFromCurrentAlbum()
{
    if (!d->iface)
    {
        return;
    }

    ImageCollection images = d->iface->currentAlbum();

    if (images.isValid())
    {
        slotAddImages(images.images());
    }
}

bool KPImagesList::isRawFile(const QUrl& url) const
{
    QString   rawFilesExt = d->iface->rawFiles();
    QFileInfo fileInfo(url.toLocalFile());

    return (rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper()));
}

void KPImagesList::slotAddImages(const QList<QUrl>& list)
{
    if (list.count() == 0)
    {
        return;
    }

    QList<QUrl> urls;
    bool raw = false;

    for (QList<QUrl>::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        QUrl imageUrl = *it;

        // Check if the new item already exist in the list.
        bool found = false;

        QTreeWidgetItemIterator iter(d->listView);

        while (*iter)
        {
            KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*iter);

            if (item && item->url() == imageUrl)
            {
                found = true;
            }

            ++iter;
        }

        if (d->allowDuplicate || !found)
        {
            // if RAW files are not allowed, skip the image
            if (!d->allowRAW && isRawFile(imageUrl))
            {
                raw = true;
                continue;
            }

            new KPImagesListViewItem(listView(), imageUrl);
            urls.append(imageUrl);
        }
    }

    emit signalAddItems(urls);
    emit signalImageListChanged();
    emit signalFoundRAWImages(raw);
}

void KPImagesList::slotAddItems()
{
    KPImageDialog dlg(this, false);
    QList<QUrl> urls = dlg.urls();

    if (!urls.isEmpty())
    {
        slotAddImages(urls);
    }

//     emit signalImageListChanged();
}

void KPImagesList::slotRemoveItems()
{
    QList<QTreeWidgetItem*> selectedItemsList = d->listView->selectedItems();
    QList<QUrl> urls;

    for (QList<QTreeWidgetItem*>::const_iterator it = selectedItemsList.constBegin();
         it != selectedItemsList.constEnd(); ++it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item)
        {
            emit signalRemovingItem(item);
            urls.append(item->url());

            if (d->processItems.contains(item->url()))
            {
                d->processItems.removeAll(item->url());
            }

            d->listView->removeItemWidget(*it, 0);
            delete *it;
        }
    }

    emit signalRemovedItems(urls);
    emit signalImageListChanged();
}

void KPImagesList::slotMoveUpItems()
{
    // move above item down, then we don't have to fix the focus
    QModelIndex curIndex = listView()->currentIndex();

    if (!curIndex.isValid())
    {
        return;
    }

    QModelIndex aboveIndex = listView()->indexAbove(curIndex);

    if (!aboveIndex.isValid())
    {
        return;
    }

    QTreeWidgetItem* const temp = listView()->takeTopLevelItem(aboveIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);
    // this is a quick fix. We loose the extra tags in flickr upload, but at list we don't get a crash
    KPImagesListViewItem* const uw = dynamic_cast<KPImagesListViewItem*>(temp);

    if (uw)
        uw->updateItemWidgets();

    emit signalImageListChanged();
    emit signalMoveUpItem();
}

void KPImagesList::slotMoveDownItems()
{
    // move below item up, then we don't have to fix the focus
    QModelIndex curIndex = listView()->currentIndex();

    if (!curIndex.isValid())
    {
        return;
    }

    QModelIndex belowIndex = listView()->indexBelow(curIndex);

    if (!belowIndex.isValid())
    {
        return;
    }

    QTreeWidgetItem* const temp = listView()->takeTopLevelItem(belowIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);

    // This is a quick fix. We can loose extra tags in uploader, but at least we don't get a crash
    KPImagesListViewItem* const uw = dynamic_cast<KPImagesListViewItem*>(temp);

    if (uw)
        uw->updateItemWidgets();

    emit signalImageListChanged();
    emit signalMoveDownItem();
}

void KPImagesList::slotClearItems()
{
    listView()->selectAll();
    slotRemoveItems();
    listView()->clear();
}

void KPImagesList::slotLoadItems()
{
    QUrl loadLevelsFile;

    loadLevelsFile = QFileDialog::getOpenFileUrl(this, i18n("Select the image file list to load"),
                                                 QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),
                                                 i18n("All Files (*)"));

    if ( loadLevelsFile.isEmpty() )
    {
        qCDebug(KIPIPLUGINS_LOG) << "empty url";
        return;
    }

    QFile file(loadLevelsFile.toLocalFile());

    qCDebug(KIPIPLUGINS_LOG) << "file path " << loadLevelsFile.toLocalFile();

    if (!file.open(QIODevice::ReadOnly))
    {
        qCDebug(KIPIPLUGINS_LOG) << "Cannot open file";
        return;
    }

    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);

    while (!xmlReader.atEnd())
    {
        if (xmlReader.isStartElement() && xmlReader.name() == QString::fromLatin1("Image"))
        {
            // get all attributes and its value of a tag in attrs variable.
            QXmlStreamAttributes attrs = xmlReader.attributes();
            // get value of each attribute from QXmlStreamAttributes
            QStringRef url = attrs.value(QString::fromLatin1("url"));

            if (url.isEmpty())
            {
                xmlReader.readNext();
                continue;
            }

            QList<QUrl> urls;
            urls.append(QUrl(url.toString()));

            if (!urls.isEmpty())
            {
                //allow plugins to append a new file
                slotAddImages(urls);
                // read plugin Image custom attributes and children element
                emit signalXMLLoadImageElement(xmlReader);
            }
        }
        else if (xmlReader.isStartElement() && xmlReader.name() != QString::fromLatin1("Images"))
        {
            // unmanaged start element (it should be plugins one)
            emit signalXMLCustomElements(xmlReader);
        }
        else if (xmlReader.isEndElement() && xmlReader.name() == QString::fromLatin1("Images"))
        {
            // if EndElement is Images return
            return;
        }

        xmlReader.readNext();
    }
}

void KPImagesList::slotSaveItems()
{
    QUrl saveLevelsFile;
    saveLevelsFile = QFileDialog::getSaveFileUrl(this, i18n("Select the image file list to save"),
                                                 QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),
                                                 i18n("All Files (*)"));

    qCDebug(KIPIPLUGINS_LOG) << "file url " << saveLevelsFile.toDisplayString();

    if (saveLevelsFile.isEmpty())
    {
        qCDebug(KIPIPLUGINS_LOG) << "empty url";
        return;
    }

    QFile file(saveLevelsFile.toLocalFile());

    if (!file.open(QIODevice::WriteOnly))
    {
        qCDebug(KIPIPLUGINS_LOG) << "Cannot open target file";
        return;
    }

    QXmlStreamWriter xmlWriter;
    xmlWriter.setDevice(&file);

    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement(QString::fromLatin1("Images"));

    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        KPImagesListViewItem* const lvItem = dynamic_cast<KPImagesListViewItem*>(*it);

        if (lvItem)
        {
            xmlWriter.writeStartElement(QString::fromLatin1("Image"));

            xmlWriter.writeAttribute(QString::fromLatin1("url"), lvItem->url().toDisplayString());

            // emit xmlWriter, item?
            emit signalXMLSaveItem(xmlWriter, lvItem);

            xmlWriter.writeEndElement(); // Image
        }

        ++it;
    }

    emit signalXMLCustomElements(xmlWriter);

    xmlWriter.writeEndElement();  // Images

    xmlWriter.writeEndDocument(); // end document
}

void KPImagesList::removeItemByUrl(const QUrl& url)
{
    bool found;

    do
    {
        found = false;
        QTreeWidgetItemIterator it(d->listView);

        while (*it)
        {
            KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

            if (item && item->url() == url)
            {
                emit signalRemovingItem(item);

                if (d->processItems.contains(item->url()))
                {
                    d->processItems.removeAll(item->url());
                }

                delete item;
                found = true;
                break;
            }

            ++it;
        }
    }
    while (found);

    emit signalImageListChanged();
}

QList<QUrl> KPImagesList::imageUrls(bool onlyUnprocessed) const
{
    QList<QUrl> list;
    QTreeWidgetItemIterator it(d->listView);

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item)
        {
            if ((onlyUnprocessed == false) || (item->state() != KPImagesListViewItem::Success))
            {
                list.append(item->url());
            }
        }

        ++it;
    }

    return list;
}

void KPImagesList::slotProgressTimerDone()
{
    if (!d->processItems.isEmpty())
    {
        foreach(const QUrl& url, d->processItems)
        {
            KPImagesListViewItem* const item = listView()->findItem(url);

            if (item)
                item->setProgressAnimation(d->progressPix.frameAt(d->progressCount));
        }

        d->progressCount++;

        if (d->progressCount == 8)
        {
            d->progressCount = 0;
        }

        d->progressTimer->start(300);
    }
}

void KPImagesList::processing(const QUrl& url)
{
    KPImagesListViewItem* const item = listView()->findItem(url);

    if (item)
    {
        d->processItems.append(url);
        d->listView->setCurrentItem(item, true);
        d->listView->scrollToItem(item);
        d->progressTimer->start(300);
    }
}

void KPImagesList::processed(const QUrl& url, bool success)
{
    KPImagesListViewItem* const item = listView()->findItem(url);

    if (item)
    {
        d->processItems.removeAll(url);
        item->setProcessedIcon(QIcon::fromTheme(success ? QString::fromLatin1("dialog-ok-apply")
                                                        : QString::fromLatin1("dialog-cancel")).pixmap(16, 16));
        item->setState(success ? KPImagesListViewItem::Success
                               : KPImagesListViewItem::Failed);

        if (d->processItems.isEmpty())
            d->progressTimer->stop();
    }
}

void KPImagesList::cancelProcess()
{
    foreach(const QUrl& url, d->processItems)
    {
        processed(url, false);
    }
}

void KPImagesList::clearProcessedStatus()
{
    QTreeWidgetItemIterator it(d->listView);

    while (*it)
    {
        KPImagesListViewItem* const lvItem = dynamic_cast<KPImagesListViewItem*>(*it);

        if (lvItem)
        {
            lvItem->setProcessedIcon(QIcon());
        }

        ++it;
    }
}

KPImagesListView* KPImagesList::listView() const
{
    return d->listView;
}

Interface* KPImagesList::iface() const
{
    return d->iface;
}

void KPImagesList::slotImageListChanged()
{
    const QList<QTreeWidgetItem*> selectedItemsList = d->listView->selectedItems();
    const bool haveImages                           = !(imageUrls().isEmpty())         && d->controlButtonsEnabled;
    const bool haveSelectedImages                   = !(selectedItemsList.isEmpty())   && d->controlButtonsEnabled;
    const bool haveOnlyOneSelectedImage             = (selectedItemsList.count() == 1) && d->controlButtonsEnabled;

    d->removeButton->setEnabled(haveSelectedImages);
    d->moveUpButton->setEnabled(haveOnlyOneSelectedImage);
    d->moveDownButton->setEnabled(haveOnlyOneSelectedImage);
    d->clearButton->setEnabled(haveImages);

    // All buttons are enabled / disabled now, but the "Add" button should always be
    // enabled, if the buttons are not explicitly disabled with enableControlButtons()
    d->addButton->setEnabled(d->controlButtonsEnabled);

    // TODO: should they be enabled by default now?
    d->loadButton->setEnabled(d->controlButtonsEnabled);
    d->saveButton->setEnabled(d->controlButtonsEnabled);
}

void KPImagesList::updateThumbnail(const QUrl& url)
{
    if (d->iface)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Request to update thumbnail for " << url;
        d->iface->thumbnails(QList<QUrl>() << url, DEFAULTSIZE);
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "No KIPI interface available : thumbnails will not generated.";
    }
}

void KPImagesList::slotThumbnail(const QUrl& url, const QPixmap& pix)
{
    qCDebug(KIPIPLUGINS_LOG) << "KIPI host send thumb (" << pix.size() << ") for " << url;

    QTreeWidgetItemIterator it(d->listView);

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item && item->url() == url)
        {
            if (!pix.isNull())
            {
                qCDebug(KIPIPLUGINS_LOG) << "Update thumb in list for " << url;
                item->setThumb(pix.scaled(d->iconSize, d->iconSize, Qt::KeepAspectRatio));
            }

            if (!d->allowDuplicate)
                return;
        }

        ++it;
    }
}

KPImagesListViewItem* KPImagesListView::getCurrentItem() const
{
    QTreeWidgetItem* const currentTreeItem = currentItem();

    if (!currentTreeItem)
    {
        return nullptr;
    }

    return dynamic_cast<KPImagesListViewItem*>(currentTreeItem);
}

QUrl KPImagesList::getCurrentUrl() const
{
    KPImagesListViewItem* const currentItem = d->listView->getCurrentItem();

    if (!currentItem)
    {
        return QUrl();
    }

    return currentItem->url();
}

}  // namespace KIPIPlugins
