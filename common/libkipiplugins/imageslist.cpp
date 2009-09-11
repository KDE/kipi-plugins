/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : widget to display an imagelist
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "imageslist.h"
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

// KDE includes

#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "imagedialog.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPlugins
{

const int DEFAULTSIZE = KIconLoader::SizeLarge;

ImagesListViewItem::ImagesListViewItem(ImagesListView *view, const KUrl& url)
                  : QTreeWidgetItem(view)
{
    m_view       = view;
    int iconSize = m_view->iconSize().width();
    setThumb(SmallIcon("image-x-generic", iconSize, KIconLoader::DisabledState));
    setUrl(url);
    setRating(-1);
    setFlags(Qt::ItemIsEnabled|Qt::ItemIsDragEnabled|Qt::ItemIsSelectable);
}

ImagesListViewItem::~ImagesListViewItem()
{
}

void ImagesListViewItem::updateInformation()
{
    if (m_view->iface())
    {
        ImageInfo info = m_view->iface()->info(m_url);

        setComments(info.description());

        setTags(QStringList());
        if (m_view->iface()->hasFeature(HostSupportsTags))
        {
            QMap<QString, QVariant> attribs = info.attributes();
            setTags(attribs["tags"].toStringList());
        }

        if (m_view->iface()->hasFeature(HostSupportsRating))
        {
            QMap<QString, QVariant> attribs = info.attributes();
            setRating(attribs["rating"].toInt());
        }
    }
}

void ImagesListViewItem::setUrl(const KUrl& url)
{
    m_url = url;
    setText(ImagesListView::Filename, m_url.fileName());
}

KUrl ImagesListViewItem::url() const
{
    return m_url;
}

void ImagesListViewItem::setComments(const QString& comments)
{
    m_comments = comments;
}

QString ImagesListViewItem::comments()
{
    return m_comments;
}

void ImagesListViewItem::setTags(const QStringList& tags)
{
    m_tags = tags;
}

QStringList ImagesListViewItem::tags()
{
    return m_tags;
}

void ImagesListViewItem::setRating(int rating)
{
    m_rating = rating;
}

int ImagesListViewItem::rating()
{
    return m_rating;
}

void ImagesListViewItem::setThumb(const QPixmap& pix)
{
    int iconSize = qMax<int>(m_view->iconSize().width(), m_view->iconSize().height());
    QPixmap pixmap(iconSize+2, iconSize+2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    setIcon(ImagesListView::Thumbnail, QIcon(pixmap));
}

// ---------------------------------------------------------------------------

ImagesListView::ImagesListView(ImagesList *parent)
              : QTreeWidget(parent)
{
    setup(DEFAULTSIZE);
}

ImagesListView::ImagesListView(int iconSize, ImagesList *parent)
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

void ImagesListView::setColumnLabel(ColumnType column, const QString &label)
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

void ImagesListView::setColumn(ColumnType column, const QString &label, bool enable)
{
    setColumnLabel(column, label);
    setColumnEnabled(column, enable);
}

void ImagesListView::dragEnterEvent(QDragEnterEvent *e)
{
    QTreeWidget::dragEnterEvent(e);
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void ImagesListView::dragMoveEvent(QDragMoveEvent *e)
{
    QTreeWidget::dragMoveEvent(e);
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void ImagesListView::dropEvent(QDropEvent *e)
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
    ImagesList *p = dynamic_cast<ImagesList*>(parent());
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

class ImagesListPriv
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
    }

    bool            allowRAW;
    bool            controlButtonsEnabled;
    int             iconSize;

    CtrlButton*     addButton;
    CtrlButton*     removeButton;
    CtrlButton*     moveUpButton;
    CtrlButton*     moveDownButton;
    CtrlButton*     clearButton;
    CtrlButton*     loadButton;
    CtrlButton*     saveButton;

    ImagesListView* listView;
    Interface*      iface;
};

ImagesList::ImagesList(Interface *iface, QWidget* parent, int iconSize)
          : QWidget(parent), d(new ImagesListPriv)
{
    d->iface    = iface;

    if (iconSize != -1)  // default = ICONSIZE
        setIconSize(iconSize);

    // --------------------------------------------------------

    d->listView  = new ImagesListView(d->iconSize, this);
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

    // --------------------------------------------------------

    setControlButtons(Add|Remove|MoveUp|MoveDown|Clear);      // add all buttons       (default)
    setControlButtonsPlacement(ControlButtonsRight);          // buttons on the right  (default)
    enableDragAndDrop(true);                                  // enable drag and drop  (default)

    // --------------------------------------------------------

    connect(d->listView, SIGNAL(addedDropedItems(const KUrl::List&)),
            this, SLOT(slotAddImages(const KUrl::List&)));

    connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
            this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));

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

    QHBoxLayout *hBtnLayout = new QHBoxLayout;
    hBtnLayout->addStretch(10);
    hBtnLayout->addWidget(d->moveUpButton);
    hBtnLayout->addWidget(d->addButton);
    hBtnLayout->addWidget(d->removeButton);
    hBtnLayout->addWidget(d->moveDownButton);
    hBtnLayout->addWidget(d->loadButton);
    hBtnLayout->addWidget(d->saveButton);
    hBtnLayout->addWidget(d->clearButton);
    hBtnLayout->addStretch(10);

    // --------------------------------------------------------

    QVBoxLayout *vBtnLayout = new QVBoxLayout;
    vBtnLayout->addStretch(10);
    vBtnLayout->addWidget(d->moveUpButton);
    vBtnLayout->addWidget(d->addButton);
    vBtnLayout->addWidget(d->removeButton);
    vBtnLayout->addWidget(d->moveDownButton);
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

    d->iface->thumbnails(urls, DEFAULTSIZE);

    emit signalImageListChanged();
    emit signalFoundRAWImages(raw);
}

void ImagesList::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (item->url() == url)
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
    for (QList<QTreeWidgetItem*>::const_iterator it = selectedItemsList.constBegin(); it!=selectedItemsList.constEnd(); ++it)
    {
        d->listView->removeItemWidget( *it, 0);
        delete *it;
    }
    emit signalImageListChanged();
}

void ImagesList::slotMoveUpItems()
{
    QModelIndex curIndex = listView()->currentIndex();
    if (!curIndex.isValid())
        return;

    QModelIndex aboveIndex = listView()->indexAbove(curIndex);
    if (!aboveIndex.isValid())
        return;

    QTreeWidgetItem* temp = listView()->takeTopLevelItem(aboveIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);

    emit signalImageListChanged();
}

void ImagesList::slotMoveDownItems()
{
    QModelIndex curIndex = listView()->currentIndex();
    if (!curIndex.isValid())
        return;

    QModelIndex belowIndex = listView()->indexBelow(curIndex);
    if (!belowIndex.isValid())
        return;

    QTreeWidgetItem* temp = listView()->takeTopLevelItem(belowIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);

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

KUrl::List ImagesList::imageUrls() const
{
    KUrl::List list;
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        list.append(item->url());
        ++it;
    }
    return list;
}

ImagesListView* ImagesList::listView() const
{
    return d->listView;
}

KIPI::Interface* ImagesList::iface() const
{
    return d->iface;
}

bool ImagesList::isRAWFile(const QString & filePath)
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
    const bool haveImages               = !(imageUrls().isEmpty()) && d->controlButtonsEnabled;
    const bool haveSelectedImages       = !(selectedItemsList.isEmpty()) && d->controlButtonsEnabled;
    const bool haveOnlyOneSelectedImage = (selectedItemsList.count()==1) && d->controlButtonsEnabled;

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
