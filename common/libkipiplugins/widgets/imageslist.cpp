/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : widget to display an imagelist
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2010 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "imageslist.moc"

// Qt includes

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QUrl>
#include <QTimer>

// KDE includes

#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <kio/previewjob.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "imagedialog.h"

using namespace KIPIPlugins;

namespace KIPIPlugins
{

const int DEFAULTSIZE = KIconLoader::SizeLarge;

class ImagesListViewItem::ImagesListViewItemPriv
{
public:

    ImagesListViewItemPriv()
    {
        rating = -1;
        view   = 0;
        state  = Waiting;
    }

    int             rating;         // Image Rating from Kipi host.
    QString         comments;       // Image comments from Kipi host.
    QStringList     tags;           // List of keywords from Kipi host.
    KUrl            url;            // Image url provided by Kipi host.
    QPixmap         thumb;          // Image thumbnail
    ImagesListView* view;
    State           state;
};

ImagesListViewItem::ImagesListViewItem(ImagesListView* view, const KUrl& url)
                  : QTreeWidgetItem(view), d(new ImagesListViewItemPriv)
{
    kDebug() << "Creating new ImageListViewItem with url " << url
             << " for list view " << view;
    d->view      = view;
    int iconSize = d->view->iconSize().width();
    setThumb(SmallIcon("image-x-generic", iconSize, KIconLoader::DisabledState));
    setUrl(url);
    setRating(-1);
    setFlags(Qt::ItemIsEnabled|Qt::ItemIsDragEnabled|Qt::ItemIsSelectable);
}

ImagesListViewItem::~ImagesListViewItem()
{
    delete d;
}

void ImagesListViewItem::updateInformation()
{
    if (d->view->iface())
    {
        ImageInfo info = d->view->iface()->info(d->url);

        setComments(info.description());

        setTags(QStringList());
        if (d->view->iface()->hasFeature(HostSupportsTags))
        {
            QMap<QString, QVariant> attribs = info.attributes();
            setTags(attribs["tags"].toStringList());
        }

        if (d->view->iface()->hasFeature(HostSupportsRating))
        {
            QMap<QString, QVariant> attribs = info.attributes();
            setRating(attribs["rating"].toInt());
        }
    }
}

void ImagesListViewItem::setUrl(const KUrl& url)
{
    d->url = url;
    setText(ImagesListView::Filename, d->url.fileName());
}

KUrl ImagesListViewItem::url() const
{
    return d->url;
}

void ImagesListViewItem::setComments(const QString& comments)
{
    d->comments = comments;
}

QString ImagesListViewItem::comments() const
{
    return d->comments;
}

void ImagesListViewItem::setTags(const QStringList& tags)
{
    d->tags = tags;
}

QStringList ImagesListViewItem::tags() const
{
    return d->tags;
}

void ImagesListViewItem::setRating(int rating)
{
    d->rating = rating;
}

int ImagesListViewItem::rating() const
{
    return d->rating;
}

void ImagesListViewItem::setPixmap(const QPixmap& pix)
{
    QIcon icon = QIcon(pix);
    //  We make sure the preview icon stays the same regardless of the role
    icon.addPixmap(pix, QIcon::Selected, QIcon::On);
    icon.addPixmap(pix, QIcon::Selected, QIcon::Off);
    icon.addPixmap(pix, QIcon::Active,   QIcon::On);
    icon.addPixmap(pix, QIcon::Active,   QIcon::Off);
    icon.addPixmap(pix, QIcon::Normal,   QIcon::On);
    icon.addPixmap(pix, QIcon::Normal,   QIcon::Off);
    setIcon(ImagesListView::Thumbnail, icon);
}

void ImagesListViewItem::setThumb(const QPixmap& pix)
{
    kDebug() << "Received new thumbnail for url " << d->url
             << ". My view is " << d->view;
    if (!d->view)
    {
        kError() << "This item doesn't have a tree view. "
                 << "This should never happen!";
        return;
    }

    int iconSize = qMax<int>(d->view->iconSize().width(), d->view->iconSize().height());
    QPixmap pixmap(iconSize+2, iconSize+2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    d->thumb = pixmap;
    setPixmap(d->thumb);
}

void ImagesListViewItem::setProgressAnimation(const QPixmap& pix)
{
    QPixmap overlay = d->thumb;
    QPixmap mask(overlay.size());
    mask.fill(QColor(128, 128, 128, 192));
    QPainter p(&overlay);
    p.drawPixmap(0, 0, mask);
    p.drawPixmap((overlay.width()/2) - (pix.width()/2), (overlay.height()/2) - (pix.height()/2), pix);
    setPixmap(overlay);
}

void ImagesListViewItem::setProcessedIcon(const QIcon& icon)
{
    setIcon(ImagesListView::Filename, icon);
    // reset thumbnail back to no animation pix
    setPixmap(d->thumb);
}

void ImagesListViewItem::setState(State state)
{
    d->state = state;
}

ImagesListViewItem::State ImagesListViewItem::state() const
{
    return d->state;
}

ImagesListView* ImagesListViewItem::view() const
{
    return d->view;
}

// ---------------------------------------------------------------------------

ImagesListView::ImagesListView(ImagesList* parent)
              : QTreeWidget(parent)
{
    setup(DEFAULTSIZE);
}

ImagesListView::ImagesListView(int iconSize, ImagesList* parent)
              : QTreeWidget(parent)
{
    setup(iconSize);
}

ImagesListView::~ImagesListView()
{
}

void ImagesListView::setup(int iconSize)
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

    header()->setResizeMode(User1, QHeaderView::ResizeToContents);
    header()->setResizeMode(User2, QHeaderView::Stretch);
    header()->setResizeMode(User3, QHeaderView::Stretch);
    header()->setResizeMode(User4, QHeaderView::Stretch);
    header()->setResizeMode(User5, QHeaderView::Stretch);
    header()->setResizeMode(User6, QHeaderView::Stretch);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotItemClicked(QTreeWidgetItem*, int)));
}

void ImagesListView::enableDragAndDrop(const bool enable)
{
    setDragEnabled(enable);
    viewport()->setAcceptDrops(enable);
    setDragDropMode(enable ? QAbstractItemView::InternalMove : QAbstractItemView::NoDragDrop);
    setDragDropOverwriteMode(enable);
    setDropIndicatorShown(enable);
}

void ImagesListView::slotItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)

    if (!item)
        return;
    emit signalItemClicked(item);
}

void ImagesListView::setColumnLabel(ColumnType column, const QString& label)
{
    headerItem()->setText(column, label);
}

void ImagesListView::setColumnEnabled(ColumnType column, bool enable)
{
    if (enable)
        showColumn(column);
    else
        hideColumn(column);
}

void ImagesListView::setColumn(ColumnType column, const QString& label, bool enable)
{
    setColumnLabel(column, label);
    setColumnEnabled(column, enable);
}

ImagesListViewItem* ImagesListView::findItem(const KUrl& url)
{
    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        ImagesListViewItem* lvItem = dynamic_cast<ImagesListViewItem*>(*it);
        if (lvItem && lvItem->url() == url)
        {
            return lvItem;
        }
        ++it;
    }
    return 0;
}

void ImagesListView::dragEnterEvent(QDragEnterEvent* e)
{
    QTreeWidget::dragEnterEvent(e);
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void ImagesListView::dragMoveEvent(QDragMoveEvent* e)
{
    QTreeWidget::dragMoveEvent(e);
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void ImagesListView::dropEvent(QDropEvent* e)
{
    QTreeWidget::dropEvent(e);
    QList<QUrl> list = e->mimeData()->urls();
    KUrl::List urls;

    foreach (const QUrl &url, list)
    {
        QFileInfo fi(url.path());
        if (fi.isFile() && fi.exists())
            urls.append(KUrl(url));
    }

    if (!urls.isEmpty())
        emit addedDropedItems(urls);
}

KIPI::Interface* ImagesListView::iface() const
{
    ImagesList* p = dynamic_cast<ImagesList*>(parent());
    if (p)
        return p->iface();

    return 0;
}

// ---------------------------------------------------------------------------

CtrlButton::CtrlButton(const QIcon& icon, QWidget* parent)
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

class ImagesList::ImagesListPriv
{
public:

    ImagesListPriv()
    {
        listView              = 0;
        iface                 = 0;
        addButton             = 0;
        removeButton          = 0;
        moveUpButton          = 0;
        moveDownButton        = 0;
        clearButton           = 0;
        loadButton            = 0;
        saveButton            = 0;
        iconSize              = DEFAULTSIZE;
        allowRAW              = true;
        controlButtonsEnabled = true;
        processItem           = 0;
        progressPix           = SmallIcon("process-working", 22);
        progressCount         = 0;
        progressTimer         = 0;
        loadRawThumb          = 0;
    }

    bool                allowRAW;
    bool                controlButtonsEnabled;
    int                 iconSize;

    CtrlButton*         addButton;
    CtrlButton*         removeButton;
    CtrlButton*         moveUpButton;
    CtrlButton*         moveDownButton;
    CtrlButton*         clearButton;
    CtrlButton*         loadButton;
    CtrlButton*         saveButton;

    ImagesListViewItem* processItem;
    QPixmap             progressPix;
    int                 progressCount;
    QTimer*             progressTimer;

    ImagesListView*     listView;
    Interface*          iface;
    LoadRawThumbThread* loadRawThumb;
};

ImagesList::ImagesList(Interface* iface, QWidget* parent, int iconSize)
          : QWidget(parent), d(new ImagesListPriv)
{
    d->iface = iface;

    if (iconSize != -1)  // default = ICONSIZE
        setIconSize(iconSize);

    // --------------------------------------------------------

    d->listView = new ImagesListView(d->iconSize, this);
    d->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // --------------------------------------------------------

    d->addButton      = new CtrlButton(SmallIcon("list-add"), this);
    d->removeButton   = new CtrlButton(SmallIcon("list-remove"), this);
    d->moveUpButton   = new CtrlButton(SmallIcon("arrow-up"), this);
    d->moveDownButton = new CtrlButton(SmallIcon("arrow-down"), this);
    d->clearButton    = new CtrlButton(SmallIcon("edit-clear-list"), this);
    d->loadButton     = new CtrlButton(SmallIcon("document-open"), this);
    d->saveButton     = new CtrlButton(SmallIcon("document-save"), this);

    d->addButton->setToolTip(i18n("Add new images to the list"));
    d->removeButton->setToolTip(i18n("Remove selected images from the list"));
    d->moveUpButton->setToolTip(i18n("Move current selected image up in the list"));
    d->moveDownButton->setToolTip(i18n("Move current selected image down in the list"));
    d->clearButton->setToolTip(i18n("Clear the list."));
    d->loadButton->setToolTip(i18n("Load a saved list."));
    d->saveButton->setToolTip(i18n("Save the list."));

    d->progressTimer = new QTimer(this);

    // --------------------------------------------------------

    setControlButtons(Add|Remove|MoveUp|MoveDown|Clear);      // add all buttons       (default)
    setControlButtonsPlacement(ControlButtonsRight);          // buttons on the right  (default)
    enableDragAndDrop(true);                                  // enable drag and drop  (default)

    // --------------------------------------------------------

    connect(d->listView, SIGNAL(addedDropedItems(const KUrl::List&)),
            this, SLOT(slotAddImages(const KUrl::List&)));

    if (d->iface)
    {
        connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
                this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));
    }

    d->loadRawThumb = new LoadRawThumbThread(this);

    connect(d->loadRawThumb, SIGNAL(signalRawThumb(const KUrl&, const QImage&)),
            this, SLOT(slotRawThumb(const KUrl&, const QImage&)));

    connect(d->listView, SIGNAL(signalItemClicked(QTreeWidgetItem*)),
            this, SIGNAL(signalItemClicked(QTreeWidgetItem*)));

    // queue this connection because itemSelectionChanged is emitted
    // while items are deleted, and accessing selectedItems at that
    // time causes a crash ...
    connect(d->listView, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotImageListChanged()), Qt::QueuedConnection);

    connect(this, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    // --------------------------------------------------------

    connect(d->addButton, SIGNAL(clicked()),
            this, SLOT(slotAddItems()));

    connect(d->removeButton, SIGNAL(clicked()),
            this, SLOT(slotRemoveItems()));

    connect(d->moveUpButton, SIGNAL(clicked()),
            this, SLOT(slotMoveUpItems()));

    connect(d->moveDownButton, SIGNAL(clicked()),
            this, SLOT(slotMoveDownItems()));

    connect(d->clearButton, SIGNAL(clicked()),
            this, SLOT(slotClearItems()));

    connect(d->loadButton, SIGNAL(clicked()),
            this, SLOT(slotLoadItems()));

    connect(d->saveButton, SIGNAL(clicked()),
            this, SLOT(slotSaveItems()));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    // --------------------------------------------------------

    emit signalImageListChanged();
}

void ImagesList::enableControlButtons(bool enable)
{
    d->controlButtonsEnabled = enable;
    slotImageListChanged();
}

void ImagesList::enableDragAndDrop(const bool enable)
{
    d->listView->enableDragAndDrop(enable);
}

void ImagesList::setControlButtonsPlacement(ControlButtonPlacement placement)
{
    delete layout();

    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addWidget(d->listView, 1, 1, 1, 1);
    mainLayout->setRowStretch(1, 10);
    mainLayout->setColumnStretch(1, 10);
    mainLayout->setMargin(KDialog::spacingHint());
    mainLayout->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    QHBoxLayout* hBtnLayout = new QHBoxLayout;
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

    QVBoxLayout* vBtnLayout = new QVBoxLayout;
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
            setControlButtons(0x0);
            break;
        }
    }
    setLayout(mainLayout);
}

void ImagesList::setControlButtons(ControlButtons buttonMask)
{
    d->addButton->setVisible(buttonMask & Add);
    d->removeButton->setVisible(buttonMask & Remove);
    d->moveUpButton->setVisible(buttonMask & MoveUp);
    d->moveDownButton->setVisible(buttonMask & MoveDown);
    d->clearButton->setVisible(buttonMask & Clear);
    d->loadButton->setVisible(buttonMask & Load);
    d->saveButton->setVisible(buttonMask & Save);
}

ImagesList::~ImagesList()
{
    delete d;
}

void ImagesList::setAllowRAW(bool allow)
{
    d->allowRAW = allow;
}

void ImagesList::setIconSize(int size)
{
    if (size < KIconLoader::SizeSmall)
        d->iconSize = KIconLoader::SizeSmall;
    else if (size > KIconLoader::SizeEnormous)
        d->iconSize = KIconLoader::SizeEnormous;
    else
        d->iconSize = size;
}

int ImagesList::iconSize() const
{
    return d->iconSize;
}

void ImagesList::loadImagesFromCurrentSelection()
{
    if (!d->iface) return;

    ImageCollection images = d->iface->currentSelection();

    if (images.isValid())
        slotAddImages(images.images());
}

void ImagesList::slotAddImages(const KUrl::List& list)
{
    if (list.count() == 0)
        return;

    KUrl::List urls;
    bool raw = false;

    for ( KUrl::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it )
    {
        KUrl imageUrl = *it;

        // Check if the new item already exist in the list.
        bool found = false;

        QTreeWidgetItemIterator iter(d->listView);
        while (*iter)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*iter);

            if (item->url() == imageUrl)
                found = true;

            ++iter;
        }

        if (!found)
        {
            // if RAW files are not allowed, skip the image
            if (!d->allowRAW && isRAWFile(imageUrl.path()))
            {
                raw = true;
                continue;
            }
            new ImagesListViewItem(listView(), imageUrl);
            urls.append(imageUrl);
        }
    }

    if (d->iface)
    {
        d->iface->thumbnails(urls, DEFAULTSIZE);
    }
    else
    {
        KIO::PreviewJob* job = KIO::filePreview(urls, DEFAULTSIZE);

        connect(job, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)),
                this, SLOT(slotKDEPreview(const KFileItem&, const QPixmap&)));

        connect(job, SIGNAL(failed(const KFileItem&)),
                this, SLOT(slotKDEPreviewFailed(const KFileItem&)));
    }

    emit signalAddItems(urls);
    emit signalImageListChanged();
    emit signalFoundRAWImages(raw);
}

// Used only if Kipi interface is null.
void ImagesList::slotKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    if (!pix.isNull())
        slotThumbnail(item.url(), pix);
}

void ImagesList::slotKDEPreviewFailed(const KFileItem& item)
{
    d->loadRawThumb->getRawThumb(item.url());
}

void ImagesList::slotRawThumb(const KUrl& url, const QImage& img)
{
    slotThumbnail(url, QPixmap::fromImage(img));
}

void ImagesList::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (item && item->url() == url)
        {
            if (pix.isNull())
                item->setThumb(SmallIcon("image-x-generic", d->iconSize, KIconLoader::DisabledState));
            else
                item->setThumb(pix.scaled(d->iconSize, d->iconSize, Qt::KeepAspectRatio));

            return;
        }
        ++it;
    }
}

void ImagesList::slotAddItems()
{
    ImageDialog dlg(this, d->iface, false);
    KUrl::List urls = dlg.urls();
    if (!urls.isEmpty())
        slotAddImages(urls);

    emit signalImageListChanged();
}

void ImagesList::slotRemoveItems()
{
    QList<QTreeWidgetItem*> selectedItemsList = d->listView->selectedItems();
    for (QList<QTreeWidgetItem*>::const_iterator it = selectedItemsList.constBegin();
         it!=selectedItemsList.constEnd(); ++it)
    {
        if (*it == d->processItem)
        {
            d->progressTimer->stop();
            d->processItem = NULL;
        }
        d->listView->removeItemWidget(*it, 0);
        delete *it;
    }
    emit signalImageListChanged();
}

void ImagesList::slotMoveUpItems()
{
    // move above item down, then we don't have to fix the focus
    QModelIndex curIndex = listView()->currentIndex();
    if (!curIndex.isValid())
        return;

    QModelIndex aboveIndex = listView()->indexAbove(curIndex);
    if (!aboveIndex.isValid())
        return;

    QTreeWidgetItem* temp = listView()->takeTopLevelItem(aboveIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);
    // this is a quick fix. We loose the extra tags in flickr upload, but at list we don't get a crash
    dynamic_cast<KIPIPlugins::ImagesListViewItem*>(temp)->updateItemWidgets();

    emit signalImageListChanged();
}

void ImagesList::slotMoveDownItems()
{
    // move below item up, then we don't have to fix the focus
    QModelIndex curIndex = listView()->currentIndex();
    if (!curIndex.isValid())
        return;

    QModelIndex belowIndex = listView()->indexBelow(curIndex);
    if (!belowIndex.isValid())
        return;

    QTreeWidgetItem* temp = listView()->takeTopLevelItem(belowIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);
    // this is a quick fix. We loose the extra tags in flickr upload, but at list we don't get a crash
    dynamic_cast<KIPIPlugins::ImagesListViewItem*>(temp)->updateItemWidgets();

    emit signalImageListChanged();
}

void ImagesList::slotClearItems()
{
    listView()->clear();
    slotRemoveItems();
}

void ImagesList::removeItemByUrl(const KUrl& url)
{
    bool found;
    do
    {
        found = false;
        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
            if (item->url() == url)
            {
                if (item == d->processItem)
                {
                    d->progressTimer->stop();
                    d->processItem = NULL;
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

KUrl::List ImagesList::imageUrls(bool onlyUnprocessed) const
{
    KUrl::List list;
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if ((onlyUnprocessed == false) || (item->state() != ImagesListViewItem::Success))
            list.append(item->url());
        ++it;
    }
    return list;
}

void ImagesList::slotProgressTimerDone()
{
    if (d->processItem)
    {
        QPixmap pix(d->progressPix.copy(0, d->progressCount*22, 22, 22));
        d->processItem->setProgressAnimation(pix);

        d->progressCount++;
        if (d->progressCount == 8)
            d->progressCount = 0;

        d->progressTimer->start(300);
    }
}

void ImagesList::processing(const KUrl& url)
{
    d->processItem = d->listView->findItem(url);
    if (d->processItem)
    {
        d->listView->setCurrentItem(d->processItem, true);
        d->listView->scrollToItem(d->processItem);
        d->progressTimer->start(300);
    }
}

void ImagesList::processed(bool success)
{
    if (d->processItem)
    {
        d->progressTimer->stop();
        d->processItem->setProcessedIcon(
            SmallIcon(success ?  "dialog-ok" : "dialog-cancel"));
        d->processItem->setState(success ? ImagesListViewItem::Success : ImagesListViewItem::Failed);
        d->processItem = 0;
    }
}

void ImagesList::clearProcessedStatus()
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* lvItem = dynamic_cast<ImagesListViewItem*>(*it);
        if (lvItem)
        {
            lvItem->setProcessedIcon(QIcon());
        }
        ++it;
    }
}

ImagesListView* ImagesList::listView() const
{
    return d->listView;
}

KIPI::Interface* ImagesList::iface() const
{
    return d->iface;
}

bool ImagesList::isRAWFile(const QString& filePath)
{
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());

    QFileInfo fileInfo(filePath);
    if (rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper()))
        return true;

    return false;
}

void ImagesList::slotImageListChanged()
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

    // TODO: load and save are not yet implemented, when should they be enabled/disabled?
    d->loadButton->setEnabled(d->controlButtonsEnabled);
    d->saveButton->setEnabled(d->controlButtonsEnabled);
}

}  // namespace KIPIPlugins
