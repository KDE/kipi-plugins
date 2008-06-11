/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <qpushbutton.h>
#include <qpainter.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <q3whatsthis.h>
#include <q3dragobject.h>
#include <qimage.h>
//Added by qt3to4:
#include <QDropEvent>
#include <Q3GridLayout>
#include <QPixmap>
#include <Q3StrList>
#include <QDragEnterEvent>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kio/previewjob.h>

// Libkipi includes.

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

#include "imagedialog.h"
// Local includes.

#include "imageslist.h"
#include "imageslist.moc"

namespace KIPIFlickrExportPlugin
{

ImagesListViewItem::ImagesListViewItem(Q3ListView *view, const KUrl& url)
                  : Q3ListViewItem(view)
{
    setThumb(SmallIcon("file_broken", ICONSIZE, KIcon::Disabled));
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
    setPixmap(0, pixmap);
}

// ---------------------------------------------------------------------------

ImagesListView::ImagesListView(QWidget *parent)
              : Q3ListView(parent)
{
    setSelectionMode(Q3ListView::Multi);
    Q3WhatsThis::add(this, i18n("<p>This is the list of images to upload on your Flickr account."));
    setAcceptDrops(true);
}

ImagesListView::~ImagesListView()
{
}

void ImagesListView::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(Q3UriDrag::canDecode(e));
}

void ImagesListView::dropEvent(QDropEvent *e)
{
    Q3StrList   strList;
    KUrl::List urls;

    if (!Q3UriDrag::decode(e, strList))
        return;

    Q3StrList stringList;
    Q3StrListIterator it(strList);
    char *str;

    while ((str = it.current()) != 0)
    {
        QString filePath = Q3UriDrag::uriToLocalFile(str);
        QFileInfo fileInfo(filePath);

        if (fileInfo.isFile() && fileInfo.exists())
            urls.append(fileInfo.filePath());

        ++it;
    }

    if (!urls.isEmpty())
       emit signalDropedItems(urls);
}

// ---------------------------------------------------------------------------

class ImagesPagePriv
{
public:

    ImagesPagePriv()
    {
        listView     = 0;
        addButton    = 0;
        removeButton = 0;
        iface        = 0;
    }

    QPushButton     *addButton;
    QPushButton     *removeButton;

    ImagesListView  *listView;

    KIPI::Interface *iface;
};

ImagesList::ImagesList(QWidget* parent, KIPI::Interface *iface)
          : QWidget(parent)
{
    d = new ImagesPagePriv;
    d->iface = iface;

    // --------------------------------------------------------

    Q3GridLayout* grid = new Q3GridLayout(this, 2, 3);
    d->listView       = new ImagesListView(this);
    d->listView->addColumn(i18n("Thumbnail"));
    d->listView->addColumn(i18n("File Name"));
    d->listView->setResizeMode(Q3ListView::AllColumns);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setSorting(-1);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setSelectionMode(Q3ListView::Extended);

    // --------------------------------------------------------

    d->addButton    = new QPushButton(this);
    d->removeButton = new QPushButton(this);
    d->addButton->setText(i18n("&Add"));
    d->addButton->setIconSet(SmallIcon("add"));
    d->removeButton->setText(i18n("&Remove"));
    d->removeButton->setIconSet(SmallIcon("remove"));

    // --------------------------------------------------------

    grid->addMultiCellWidget(d->listView,     0, 2, 0, 2);
    grid->addMultiCellWidget(d->addButton,    0, 0, 3, 3);
    grid->addMultiCellWidget(d->removeButton, 1, 1, 3, 3);
    grid->setColStretch(0, 10);
    grid->setRowStretch(2, 10);
    grid->setMargin(KDialog::spacingHint());
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->addButton, SIGNAL(clicked()),
            this, SLOT(slotAddItems()));

    connect(d->removeButton, SIGNAL(clicked()),
            this, SLOT(slotRemoveItems()));

    connect(d->listView, SIGNAL(signalDropedItems(const KUrl::List&)),
            this, SLOT(slotAddImages(const KUrl::List&)));

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
    if (list.count() == 0) return;

    KUrl::List urls;

    for(KUrl::List::const_iterator it = list.begin(); it != list.end(); ++it)
    {
        KUrl imageUrl = *it;

        // Check if the new item already exist in the list.

        bool find = false;

        Q3ListViewItemIterator it(d->listView);
        while (it.current())
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

    emit signalImageListChanged(imageUrls().isEmpty());

    KIO::PreviewJob *thumbnailJob = KIO::filePreview(urls, ICONSIZE);

    connect(thumbnailJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            this, SLOT(slotGotThumbnail(const KFileItem*, const QPixmap&)));
}

void ImagesList::slotGotThumbnail(const KFileItem *item, const QPixmap& pix)
{
    Q3ListViewItemIterator it(d->listView);

    while (it.current())
    {
        ImagesListViewItem *selItem = dynamic_cast<ImagesListViewItem*>(*it);
        if (selItem->url() == item->url())
        {
            selItem->setPixmap(0, pix);
        }
        ++it;
    }
}

void ImagesList::slotAddItems()
{
    KIPIPlugins::ImageDialog dlg(this, d->iface, false, true);
    KUrl::List urls = dlg.urls();
    if (!urls.isEmpty())
    {
        slotAddImages(urls);
    }

    emit signalImageListChanged(imageUrls().isEmpty());
}

void ImagesList::slotRemoveItems()
{
    bool find;
    do
    {
        find = false;
        Q3ListViewItemIterator it(d->listView);
        while (it.current())
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

    emit signalImageListChanged(imageUrls().isEmpty());
}

void ImagesList::removeItemByUrl(const KUrl& url)
{
    bool find;
    do
    {
        find = false;
        Q3ListViewItemIterator it(d->listView);
        while (it.current())
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
    Q3ListViewItemIterator it(d->listView);
    while (it.current())
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        list.append(item->url());
        ++it;
    }
    return list;
}

}  // namespace KIPIFlickrExportPlugin
