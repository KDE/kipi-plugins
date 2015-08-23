/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-05-21
 * Description : widget to display an imagelist
 *
 * Copyright (C) 2006-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpimageslist.moc"

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
#include <QFile>
#include <QPointer>
#include <QXmlStreamAttributes>
#include <QStringRef>

// KDE includes

#include <kdeversion.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <kio/previewjob.h>
#include <kpixmapsequence.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "kprawthumbthread.h"
#include "kpimageinfo.h"
#include "kpimagedialog.h"

using namespace KIPIPlugins;

namespace KIPIPlugins
{

const int DEFAULTSIZE = KIconLoader::SizeLarge;

class KPImagesListViewItem::Private
{
public:

    Private()
    {
        rating   = -1;
        view     = 0;
        state    = Waiting;
        hasThumb = false;
    }

    bool              hasThumb;

    int               rating;         // Image Rating from Kipi host.
    QString           comments;       // Image comments from Kipi host.
    QStringList       tags;           // List of keywords from Kipi host.
    KUrl              url;            // Image url provided by Kipi host.
    QPixmap           thumb;          // Image thumbnail.
    KPImagesListView* view;
    State             state;
};

KPImagesListViewItem::KPImagesListViewItem(KPImagesListView* const view, const KUrl& url)
    : QTreeWidgetItem(view), d(new Private)
{
    kDebug() << "Creating new ImageListViewItem with url " << url
             << " for list view " << view;
    d->view      = view;
    int iconSize = d->view->iconSize().width();
    setThumb(SmallIcon("image-x-generic", iconSize, KIconLoader::DisabledState), false);
    setUrl(url);
    setRating(-1);
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
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

void KPImagesListViewItem::setUrl(const KUrl& url)
{
    d->url = url;
    setText(KPImagesListView::Filename, d->url.fileName());
}

KUrl KPImagesListViewItem::url() const
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
    kDebug() << "Received new thumbnail for url " << d->url
             << ". My view is " << d->view;

    if (!d->view)
    {
        kError() << "This item doesn't have a tree view. "
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

    header()->setResizeMode(User1, QHeaderView::Interactive);
    header()->setResizeMode(User2, QHeaderView::Interactive);
    header()->setResizeMode(User3, QHeaderView::Interactive);
    header()->setResizeMode(User4, QHeaderView::Interactive);
    header()->setResizeMode(User5, QHeaderView::Interactive);
    header()->setResizeMode(User6, QHeaderView::Stretch);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotItemClicked(QTreeWidgetItem*,int)));
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
    KPImagesListViewItem* item = dynamic_cast<KPImagesListViewItem*>(itemFromIndex(index));

    if (item && !item->hasValidThumbnail())
    {
        KPImagesList* view = dynamic_cast<KPImagesList*>(parent());

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

KPImagesListViewItem* KPImagesListView::findItem(const KUrl& url)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        KPImagesListViewItem* lvItem = dynamic_cast<KPImagesListViewItem*>(*it);

        if (lvItem && lvItem->url() == url)
        {
            return lvItem;
        }

        ++it;
    }

    return 0;
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
    KUrl::List urls;

    foreach(const QUrl& url, list)
    {
        QFileInfo fi(url.path());

        if (fi.isFile() && fi.exists())
        {
            urls.append(KUrl(url));
        }
    }

    if (!urls.isEmpty())
    {
        emit signalAddedDropedItems(urls);
    }
}

Interface* KPImagesListView::iface() const
{
    KPImagesList* p = dynamic_cast<KPImagesList*>(parent());
    if (p)
    {
        return p->iface();
    }
    return 0;
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
        allowDuplicate        = false;
        progressCount         = 0;
        progressTimer         = 0;
        loadRawThumb          = 0;
        progressPix           = KPixmapSequence("process-working", KIconLoader::SizeSmallMedium);

        PluginLoader* pl = PluginLoader::instance();
        if (pl)
        {
            iface = pl->interface();
        }
    }

    bool              allowRAW;
    bool              allowDuplicate;
    bool              controlButtonsEnabled;
    int               iconSize;

    CtrlButton*       addButton;
    CtrlButton*       removeButton;
    CtrlButton*       moveUpButton;
    CtrlButton*       moveDownButton;
    CtrlButton*       clearButton;
    CtrlButton*       loadButton;
    CtrlButton*       saveButton;

    KUrl::List        processItems;
    KPixmapSequence   progressPix;
    int               progressCount;
    QTimer*           progressTimer;

    KPImagesListView* listView;
    Interface*        iface;
    KPRawThumbThread* loadRawThumb;
};

KPImagesList::KPImagesList(QWidget* const parent, int iconSize)
    : QWidget(parent), d(new Private)
{
    if (iconSize != -1)  // default = ICONSIZE
    {
        setIconSize(iconSize);
    }

    // --------------------------------------------------------

    d->listView = new KPImagesListView(d->iconSize, this);
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

    setControlButtons(Add | Remove | MoveUp | MoveDown | Clear | Save | Load ); // add all buttons       (default)
    setControlButtonsPlacement(ControlButtonsRight);             // buttons on the right  (default)
    enableDragAndDrop(true);                                     // enable drag and drop  (default)

    // --------------------------------------------------------

    connect(d->listView, SIGNAL(signalAddedDropedItems(KUrl::List)),
            this, SLOT(slotAddImages(KUrl::List)));

    if (d->iface)
    {
        connect(d->iface, SIGNAL(gotThumbnail(KUrl,QPixmap)),
                this, SLOT(slotThumbnail(KUrl,QPixmap)));
    }

    d->loadRawThumb = new KPRawThumbThread(this);

    connect(d->loadRawThumb, SIGNAL(signalRawThumb(KUrl,QImage)),
            this, SLOT(slotRawThumb(KUrl,QImage)));

    connect(d->listView, SIGNAL(signalItemClicked(QTreeWidgetItem*)),
            this, SIGNAL(signalItemClicked(QTreeWidgetItem*)));

    connect(d->listView, SIGNAL(signalContextMenuRequested()),
            this, SIGNAL(signalContextMenuRequested()));

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
    if (size < KIconLoader::SizeSmall)
    {
        d->iconSize = KIconLoader::SizeSmall;
    }
    else if (size > KIconLoader::SizeEnormous)
    {
        d->iconSize = KIconLoader::SizeEnormous;
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
    bool selection = checkSelection();
    if(selection == true)
    {
        if (!d->iface)
        {
            return;
        }

        ImageCollection images = d->iface->currentSelection();

        if (images.isValid())
        {
            slotAddImages(images.images());
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

bool KPImagesList::checkSelection()
{
    if (!d->iface)
    {
        return false;
    }

    ImageCollection images = d->iface->currentSelection();
    bool check_empty = images.images().empty();
    if(check_empty == true)
    {
        return false;
    }
    else
    {
        return true;
    }
}  

void KPImagesList::slotAddImages(const KUrl::List& list)
{
    if (list.count() == 0)
    {
        return;
    }

    KUrl::List urls;
    bool raw = false;

    for (KUrl::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        KUrl imageUrl = *it;

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
            if (!d->allowRAW && KPMetadata::isRawFile(imageUrl))
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
    KUrl::List urls = dlg.urls();

    if (!urls.isEmpty())
    {
        slotAddImages(urls);
    }

//     emit signalImageListChanged();
}

void KPImagesList::slotRemoveItems()
{
    QList<QTreeWidgetItem*> selectedItemsList = d->listView->selectedItems();
    KUrl::List urls;

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

    QTreeWidgetItem* temp = listView()->takeTopLevelItem(aboveIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);
    // this is a quick fix. We loose the extra tags in flickr upload, but at list we don't get a crash
    dynamic_cast<KIPIPlugins::KPImagesListViewItem*>(temp)->updateItemWidgets();

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
    KIPIPlugins::KPImagesListViewItem* const uw = dynamic_cast<KIPIPlugins::KPImagesListViewItem*>(temp);

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
    KUrl loadLevelsFile;

    loadLevelsFile = KFileDialog::getOpenUrl(KGlobalSettings::documentPath(),
                                            QString( "*" ), this,
                                            QString( i18n("Select the image file list to load")) );

    if ( loadLevelsFile.isEmpty() )
    {
        return;
    }
    QFile file(loadLevelsFile.path());

    kDebug() << "file path " <<loadLevelsFile.path();
    file.open(QIODevice::ReadOnly);
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);

    while (!xmlReader.atEnd())
    {
        if (xmlReader.isStartElement() && xmlReader.name() == "Image")
        {
          // get all attributes and its value of a tag in attrs variable.
          QXmlStreamAttributes attrs = xmlReader.attributes();
          // get value of each attribute from QXmlStreamAttributes
          QStringRef url = attrs.value("url");
          if (url.isEmpty())
          {
              xmlReader.readNext();
              continue;
          }
          KUrl::List urls;
          urls.append(url.toString());

          if (!urls.isEmpty())
          {
              //allow plugins to append a new file
              slotAddImages(urls);
              // read plugin Image custom attributes and children element  
              emit signalXMLLoadImageElement(xmlReader);
          }
        }
        else if (xmlReader.isStartElement() && xmlReader.name() != "Images")
        {
          // unmanaged start element (it should be plugins one)
          emit signalXMLCustomElements(xmlReader);
        }
        else if(xmlReader.isEndElement() && xmlReader.name() == "Images")
        {
          // if EndElement is Images return
          return;
        }
        xmlReader.readNext();
    }
}

void KPImagesList::slotSaveItems()
{
    KUrl saveLevelsFile;
    saveLevelsFile = KFileDialog::getSaveUrl(KGlobalSettings::documentPath(),
                                             QString( "*" ), this,
                                             QString( i18n("Select the image file list to save")) );
    kDebug() << "file url " <<saveLevelsFile.prettyUrl().toAscii();

    if ( saveLevelsFile.isEmpty() )
    {
       kDebug() << "empty url ";
        return;
    }

    QFile file(saveLevelsFile.path() /*.prettyUrl().toAscii()*/);
    file.open(QIODevice::WriteOnly);
//     file.open(stdout, QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter;
    xmlWriter.setDevice(&file);

    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("Images");

    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        KPImagesListViewItem* lvItem = dynamic_cast<KPImagesListViewItem*>(*it);

        if (lvItem)
        {
            xmlWriter.writeStartElement("Image");

            xmlWriter.writeAttribute("url", lvItem->url().prettyUrl().toAscii());

            //emit xmlWriter, item?
            emit signalXMLSaveItem(xmlWriter, lvItem);

            xmlWriter.writeEndElement(); //Image
        }
        ++it;
    }

    emit signalXMLCustomElements(xmlWriter);

    xmlWriter.writeEndElement(); // Images

    xmlWriter.writeEndDocument(); //end document
}

void KPImagesList::removeItemByUrl(const KUrl& url)
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

KUrl::List KPImagesList::imageUrls(bool onlyUnprocessed) const
{
    KUrl::List list;
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
        foreach(const KUrl& url, d->processItems)
        {
            KPImagesListViewItem* item = listView()->findItem(url);
            if (item) item->setProgressAnimation(d->progressPix.frameAt(d->progressCount));
        }

        d->progressCount++;

        if (d->progressCount == 8)
        {
            d->progressCount = 0;
        }

        d->progressTimer->start(300);
    }
}

void KPImagesList::processing(const KUrl& url)
{
    KPImagesListViewItem* item = listView()->findItem(url);

    if (item)
    {
        d->processItems.append(url);
        d->listView->setCurrentItem(item, true);
        d->listView->scrollToItem(item);
        d->progressTimer->start(300);
    }
}

void KPImagesList::processed(const KUrl& url, bool success)
{
    KPImagesListViewItem* item = listView()->findItem(url);

    if (item)
    {
        d->processItems.removeAll(url);
        item->setProcessedIcon(SmallIcon(success ?  "dialog-ok" : "dialog-cancel"));
        item->setState(success ? KPImagesListViewItem::Success : KPImagesListViewItem::Failed);

        if(d->processItems.isEmpty())
            d->progressTimer->stop();
    }
}

void KPImagesList::cancelProcess()
{
    foreach(const KUrl& url, d->processItems)
    {
        processed(url, false);
    }
}

void KPImagesList::clearProcessedStatus()
{
    QTreeWidgetItemIterator it(d->listView);

    while (*it)
    {
        KPImagesListViewItem* lvItem = dynamic_cast<KPImagesListViewItem*>(*it);

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

void KPImagesList::updateThumbnail(const KUrl& url)
{
    if (d->iface)
    {
        d->iface->thumbnails(KUrl::List() << url.toLocalFile(), DEFAULTSIZE);
    }
    else
    {
        if (!url.isValid())
        {
            return;
        }

#if KDE_IS_VERSION(4,7,0)
        KFileItemList items;
        items.append(KFileItem(KFileItem::Unknown, KFileItem::Unknown, url.toLocalFile(), true));
        KIO::PreviewJob* job = KIO::filePreview(items, QSize(DEFAULTSIZE, DEFAULTSIZE));
#else
        KIO::PreviewJob* job = KIO::filePreview(KUrl::List() << url.toLocalFile(), DEFAULTSIZE);
#endif

        connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)),
                this, SLOT(slotKDEPreview(KFileItem,QPixmap)));

        connect(job, SIGNAL(failed(KFileItem)),
                this, SLOT(slotKDEPreviewFailed(KFileItem)));
    }
}

// Used only if Kipi interface is null.
void KPImagesList::slotKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    if (!pix.isNull())
    {
        slotThumbnail(item.url(), pix);
    }
}

void KPImagesList::slotKDEPreviewFailed(const KFileItem& item)
{
    d->loadRawThumb->getRawThumb(item.url());
}

void KPImagesList::slotRawThumb(const KUrl& url, const QImage& img)
{
    slotThumbnail(url, QPixmap::fromImage(img));
}

void KPImagesList::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    QTreeWidgetItemIterator it(d->listView);

    while (*it)
    {
        KPImagesListViewItem* item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item && item->url() == url)
        {
            if (!pix.isNull())
            {
                item->setThumb(pix.scaled(d->iconSize, d->iconSize, Qt::KeepAspectRatio));
            }

            if (!d->allowDuplicate)
              return;
        }

        ++it;
    }
}

KIPIPlugins::KPImagesListViewItem* KIPIPlugins::KPImagesListView::getCurrentItem() const
{
    QTreeWidgetItem* const currentTreeItem = currentItem();
    if (!currentTreeItem)
    {
        return 0;
    }

    return dynamic_cast<KPImagesListViewItem*>(currentTreeItem);
}

KUrl KIPIPlugins::KPImagesList::getCurrentUrl() const
{
    KPImagesListViewItem* const currentItem = d->listView->getCurrentItem();
    if (!currentItem)
    {
        return KUrl();
    }

    return currentItem->url();
}

}  // namespace KIPIPlugins
