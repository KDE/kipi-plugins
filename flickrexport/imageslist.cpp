/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : images list settings page.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT includes.

#include <QPushButton>
#include <QPainter>
#include <QDragEnterEvent>
#include <QUrl>
#include <QFileInfo>
#include <QGridLayout>
#include <QHeaderView>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>
#include <kiconloader.h>
#include <kdebug.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes.

#include "imagedialog.h"
#include "imageslist.h"
#include "imageslist.moc"

namespace KIPIFlickrExportPlugin
{

ImagesListViewItem::ImagesListViewItem(QTreeWidget *view, const KUrl& url)
                  : QTreeWidgetItem(view)
{
    setThumb(SmallIcon("image-x-generic", KIconLoader::SizeLarge, KIconLoader::DisabledState));
    setUrl(url);
}

ImagesListViewItem::~ImagesListViewItem()
{
}

void ImagesListViewItem::setUrl(const KUrl& url)
{
    m_url = url;
    setText(1, m_url.fileName());
}

KUrl ImagesListViewItem::url() const
{
    return m_url;
}

void ImagesListViewItem::setThumb(const QPixmap& pix) 
{
    QPixmap pixmap(ICONSIZE+2, ICONSIZE+2);
    pixmap.fill(Qt::color0);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    setIcon(0, QIcon(pixmap));
}

// ---------------------------------------------------------------------------

ImagesListView::ImagesListView(QWidget *parent)
              : QTreeWidget(parent)
{
    setIconSize(QSize(ICONSIZE, ICONSIZE));
    setSelectionMode(QAbstractItemView::MultiSelection);
    setWhatsThis(i18n("<p>This is the list of images to upload on your Flickr account."));
    setAcceptDrops(true);
    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(2);
    setHeaderLabels(QStringList() << i18n("Thumbnail") << i18n("File Name"));
    header()->setResizeMode(QHeaderView::Stretch);
}

ImagesListView::~ImagesListView()
{
}

void ImagesListView::dragEnterEvent(QDragEnterEvent *e)
{
     if (e->mimeData()->hasUrls())
         e->acceptProposedAction();
}

void ImagesListView::dropEvent(QDropEvent *e)
{
    QList<QUrl> list = e->mimeData()->urls();
    KUrl::List  urls;

    foreach (QUrl url, list)
    {
        QFileInfo fi(url.path());
        if (fi.isFile() && fi.exists())
            urls.append(KUrl(url));
    }

    e->acceptProposedAction();

    if (!urls.isEmpty())
       emit addedDropedItems(urls);
}

// ---------------------------------------------------------------------------

class ImagesListPriv
{
public:

    ImagesListPriv()
    {
        listView     = 0;
        iface        = 0;
        addButton    = 0;
        removeButton = 0;
    }

    QPushButton     *addButton;
    QPushButton     *removeButton;

    ImagesListView  *listView;

    KIPI::Interface *iface;
};

ImagesList::ImagesList(QWidget* parent, KIPI::Interface *iface)
          : QWidget(parent)
{
    d = new ImagesListPriv;
    d->iface = iface;

    // --------------------------------------------------------

    QGridLayout* grid = new QGridLayout(this);
    d->listView       = new ImagesListView(this);

    // --------------------------------------------------------

    d->addButton    = new QPushButton(this);
    d->removeButton = new QPushButton(this);
    d->addButton->setText( i18n( "&Add" ) );
    d->addButton->setIcon(SmallIcon("list-add"));
    d->removeButton->setText( i18n( "&Remove" ) );
    d->removeButton->setIcon(SmallIcon("list-remove"));

    // --------------------------------------------------------

    grid->addWidget(d->listView,     0, 0, 3, 1);
    grid->addWidget(d->addButton,    0, 1, 1, 1);
    grid->addWidget(d->removeButton, 1, 1, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(2, 10);
    grid->setMargin(KDialog::spacingHint());
    grid->setSpacing(KDialog::spacingHint());

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

    KIPI::ImageCollection images = d->iface->currentSelection();

    if (images.isValid())
        slotAddImages(images.images());
}

ImagesList::~ImagesList()
{
    delete d;
}

void ImagesList::slotAddImages(const KUrl::List& list)
{
    if ( list.count() == 0 ) return;

    KUrl::List urls;

    for( KUrl::List::ConstIterator it = list.begin(); it != list.end(); ++it )
    {
        KUrl imageUrl = *it;

        // Check if the new item already exist in the list.

        bool find = false;

        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);

            if (item->url() == imageUrl)
                find = true;

            ++it;
        }

        if (!find)
        {
            new ImagesListViewItem(d->listView, imageUrl);
            urls.append(imageUrl);
        }
    }

    d->iface->thumbnails(urls, ICONSIZE);
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
    KIPIPlugins::ImageDialog dlg(this, d->iface, false);
    KUrl::List urls = dlg.urls();
    if (!urls.isEmpty())
        slotAddImages(urls);
}

void ImagesList::slotRemoveItems()
{
    bool find;
    do
    {
        find = false;
        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
            if (item->isSelected())
            {
                delete item;
                find = true;
                break;
            }
            ++it;
        }
    }
    while(find);
}

void ImagesList::removeItemByUrl(const KUrl& url)
{
    bool find;
    do
    {
        find = false;
        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
            if (item->url() == url)
            {
                delete item;
                find = true;
                break;
            }
            ++it;
        }
    }
    while(find);

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

}  // namespace KIPIFlickrExportPlugin
