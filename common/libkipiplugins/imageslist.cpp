/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : widget to display an imagelist
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#define ICONSIZE 64

#include "imageslist.h"
#include "imageslist.moc"

// Qt includes.

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QUrl>

// KDE includes.

#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// LibKDcraw includes.

#include <libkdcraw/kdcraw.h>

// Local includes.

#include "imagedialog.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPlugins
{

ImagesListViewItem::ImagesListViewItem(ImagesListView *view, const KUrl& url)
                  : QTreeWidgetItem(view)
{
    setThumb(SmallIcon("image-x-generic", KIconLoader::SizeLarge, KIconLoader::DisabledState));
    setUrl(url);
    setRating(-1);
    Interface *iface = view->iface();
    if (iface)
    {
        ImageInfo info = iface->info(url);

        setComments(info.description());

        setTags(QStringList());
        if (view->iface()->hasFeature(HostSupportsTags))
        {
            QMap<QString, QVariant> attribs = info.attributes();
            setTags(attribs["tags"].toStringList());
        }

        if (view->iface()->hasFeature(HostSupportsRating))
        {
            QMap<QString, QVariant> attribs = info.attributes();
            setRating(attribs["rating"].toInt());
        }
    }
}

ImagesListViewItem::~ImagesListViewItem()
{
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
    QPixmap pixmap(ICONSIZE+2, ICONSIZE+2);
    pixmap.fill(Qt::color0);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    setIcon(ImagesListView::Thumbnail, QIcon(pixmap));
}

// ---------------------------------------------------------------------------

ImagesListView::ImagesListView(ImagesList *parent)
              : QTreeWidget(parent)
{
    setIconSize(QSize(ICONSIZE, ICONSIZE));
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);

    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setColumnCount(5);
    setHeaderLabels(QStringList() << i18n("Thumbnail")
                                  << i18n("File Name")
                                  << i18n("User1")
                                  << i18n("User2")
                                  << i18n("User3"));
    hideColumn(User1);
    hideColumn(User2);
    hideColumn(User3);
    header()->setResizeMode(QHeaderView::ResizeToContents);
}

ImagesListView::~ImagesListView()
{
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
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void ImagesListView::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void ImagesListView::dropEvent(QDropEvent *e)
{
    QList<QUrl> list = e->mimeData()->urls();
    KUrl::List urls;

    foreach (const QUrl &url, list)
    {
        QFileInfo fi(url.path());
        if (fi.isFile() && fi.exists())
            urls.append(KUrl(url));
    }

    e->acceptProposedAction();

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

class ImagesListPriv
{
public:

    ImagesListPriv()
    {
        listView        = 0;
        iface           = 0;
        addButton       = 0;
        removeButton    = 0;
        plainPage       = 0;
    }

    bool                allowRAW;

    QPushButton*        addButton;
    QPushButton*        removeButton;

    QWidget*            plainPage;

    ImagesListView*     listView;

    Interface*          iface;
};

ImagesList::ImagesList(Interface *iface, bool allowRAW, bool autoLoad, QWidget* parent)
          : QWidget(parent),
            d(new ImagesListPriv)
{
    d->iface    = iface;
    d->allowRAW = allowRAW;

    // --------------------------------------------------------

    QGridLayout* mainLayout = new QGridLayout;
    d->listView             = new ImagesListView;
    d->plainPage            = new QWidget;

    // --------------------------------------------------------

    d->addButton    = new QPushButton;
    d->removeButton = new QPushButton;
    d->addButton->setText(i18n("&Add"));
    d->addButton->setIcon(SmallIcon("list-add"));
    d->removeButton->setText(i18n("&Remove"));
    d->removeButton->setIcon(SmallIcon("list-remove"));

    // --------------------------------------------------------

    mainLayout->addWidget(d->listView,      0, 0, 5, 1);
    mainLayout->addWidget(d->addButton,     0, 1, 1, 1);
    mainLayout->addWidget(d->removeButton,  1, 1, 1, 1);
    mainLayout->addWidget(d->plainPage,     2, 1, 1, 1);
    mainLayout->setRowStretch(3, 10);
    mainLayout->setMargin(KDialog::spacingHint());
    mainLayout->setSpacing(KDialog::spacingHint());
    setLayout(mainLayout);

    // --------------------------------------------------------

    connect(d->listView, SIGNAL(addedDropedItems(const KUrl::List&)),
            this, SLOT(slotAddImages(const KUrl::List&)));

    connect(d->addButton, SIGNAL(clicked()),
            this, SLOT(slotAddItems()));

    connect(d->removeButton, SIGNAL(clicked()),
            this, SLOT(slotRemoveItems()));

    connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
            this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));

    // --------------------------------------------------------

    if (autoLoad)
    {
        ImageCollection images = d->iface->currentSelection();

        if (images.isValid())
            slotAddImages(images.images());
    }
}

ImagesList::~ImagesList()
{
    delete d;
}

void ImagesList::slotAddImages(const KUrl::List& list)
{
    if (list.count() == 0) return;

    KUrl::List urls;

    bool raw = false;

    for ( KUrl::List::ConstIterator it = list.begin(); it != list.end(); ++it )
    {
        KUrl imageUrl = *it;

        // Check if the new item already exist in the list.

        bool found = false;

        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);

            if (item->url() == imageUrl)
                found = true;

            ++it;
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

    d->iface->thumbnails(urls, ICONSIZE);

    emit signalImageListChanged(imageUrls().isEmpty());
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
                item->setThumb(SmallIcon("image-x-generic", ICONSIZE, KIconLoader::DisabledState));
            else
                item->setThumb(pix.scaled(ICONSIZE, ICONSIZE, Qt::KeepAspectRatio));

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

    emit signalImageListChanged(imageUrls().isEmpty());
}

void ImagesList::slotRemoveItems()
{
    QTreeWidgetItemIterator it(d->listView, QTreeWidgetItemIterator::Selected);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        ++it;
        delete item;
    }
    emit signalImageListChanged(imageUrls().isEmpty());
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

    emit signalImageListChanged(imageUrls().isEmpty());
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

QWidget* ImagesList::plainPage() const
{
    return d->plainPage;
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

}  // namespace KIPIPlugins
