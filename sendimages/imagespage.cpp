/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : images list settings page.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QLayout>
#include <QPushButton>
#include <QPainter>
#include <QPalette>
#include <QDragEnterEvent>
#include <QUrl>
#include <QFileInfo>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>
#include <kiconloader.h>
#include <kdebug.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>
#include <libkipi/imagedialog.h>

// Local includes.

#include "imagespage.h"
#include "imagespage.moc"

namespace KIPISendimagesPlugin
{

ImagesListViewItem::ImagesListViewItem(QListWidget *view, KIPI::Interface *iface, const KUrl& url)
                  : QListWidgetItem(view)
{
    KIPI::ImageInfo imageInfo = iface->info(url);
    
    setThumb(SmallIcon("empty", KIconLoader::SizeLarge, KIconLoader::DisabledState));

    setUrl(url);
    setComments(imageInfo.description());

    setTags(QStringList());
    if (iface->hasFeature(KIPI::HostSupportsTags))
    {
        QMap<QString, QVariant> attribs = imageInfo.attributes();
        setTags(attribs["tags"].toStringList());    
    }

    setRating(-1);
    if (iface->hasFeature(KIPI::HostSupportsRating))
    {
        QMap<QString, QVariant> attribs = imageInfo.attributes();
        setRating(attribs["rating"].toInt());    
    }
}

ImagesListViewItem::~ImagesListViewItem()
{
}

void ImagesListViewItem::setUrl(const KUrl& url)
{
    m_item.orgUrl = url;
    setText(m_item.orgUrl.fileName());
}

KUrl ImagesListViewItem::url() 
{
    return m_item.orgUrl; 
}

void ImagesListViewItem::setComments(const QString& comments)
{
    m_item.comments = comments;
}

QString ImagesListViewItem::comments() 
{ 
    return m_item.comments; 
}

void ImagesListViewItem::setTags(const QStringList& tags) 
{ 
    m_item.tags = tags; 
}

QStringList ImagesListViewItem::tags() 
{ 
    return m_item.tags; 
}

void ImagesListViewItem::setRating(int rating) 
{ 
    m_item.rating = rating; 
}

int ImagesListViewItem::rating() 
{ 
    return m_item.rating; 
}

void ImagesListViewItem::setThumb(const QPixmap& pix) 
{ 
    QPixmap pixmap(ICONSIZE+2, ICONSIZE+2);
    pixmap.fill(Qt::color0);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    setIcon(QIcon(pixmap)); 
}

EmailItem ImagesListViewItem::emailItem() 
{ 
    return m_item; 
}

// ---------------------------------------------------------------------------

ImagesListView::ImagesListView(QWidget *parent)
              : QListWidget(parent)
{
    setIconSize(QSize(ICONSIZE, ICONSIZE));
    setSelectionMode(QAbstractItemView::MultiSelection);    
    setWhatsThis(i18n("<p>This is the list of images to e-mail."));
    setAcceptDrops(true);
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

class ImagesPagePriv
{
public:

    ImagesPagePriv()
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

ImagesPage::ImagesPage(QWidget* parent, KIPI::Interface *iface)
          : QWidget(parent)
{
    d = new ImagesPagePriv;
    d->iface = iface;

    // --------------------------------------------------------

    QGridLayout* grid = new QGridLayout(this);
    d->listView       = new ImagesListView(this);

    // --------------------------------------------------------

    d->addButton    = new QPushButton(this);
    d->removeButton = new QPushButton(this);
    d->addButton->setText( i18n( "&Add" ) );
    d->addButton->setIcon(SmallIcon("edit-add"));
    d->removeButton->setText( i18n( "&Remove" ) );
    d->removeButton->setIcon(SmallIcon("edit-delete"));

    // --------------------------------------------------------

    grid->addWidget(d->listView, 0, 0, 3, 1);
    grid->addWidget(d->addButton, 0, 1, 1, 1);
    grid->addWidget(d->removeButton, 1, 1, 1, 1);
    grid->setColumnStretch(0, 10);                     
    grid->setRowStretch(2, 10);    
    grid->setMargin(0);
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
}

ImagesPage::~ImagesPage()
{
    delete d;
}

void ImagesPage::slotAddImages(const KUrl::List& list)
{
    if ( list.count() == 0 ) return;

    KUrl::List urls;

    for( KUrl::List::ConstIterator it = list.begin(); it != list.end(); ++it )
    {
        KUrl imageUrl = *it;
    
        // Check if the new item already exist in the list.
    
        bool find = false;

        for (int i = 0 ; i < d->listView->count(); i++)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(d->listView->item(i));
    
            if (item->url() == imageUrl)
                find = true;
        }
    
        if (!find)
        {
            new ImagesListViewItem(d->listView, d->iface, imageUrl);
            urls.append(imageUrl);
        }
    }

    d->iface->thumbnails(urls, ICONSIZE);
}

void ImagesPage::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    for (int i = 0 ; i < d->listView->count(); i++)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(d->listView->item(i));
        if (item->url() == url)
        {
            if (pix.isNull())
                item->setThumb(SmallIcon("empty", ICONSIZE, KIconLoader::DisabledState));
            else
                item->setThumb(pix.scaled(ICONSIZE, ICONSIZE, Qt::KeepAspectRatio));

            return;
        }
    }
}

void ImagesPage::slotAddItems()
{
    KIPI::ImageDialog dlg(this, d->iface, false);
    KUrl::List urls = dlg.urls();
    if (!urls.isEmpty())
        slotAddImages(urls);
}

void ImagesPage::slotRemoveItems()
{
    bool find;
    do
    {
        find = false;
        for (int i = 0 ; i < d->listView->count(); i++)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(d->listView->item(i));
            if (item->isSelected())
            {
                delete item;
                find = true;
                break;
            }
        }
    }
    while(find);
}

QList<EmailItem> ImagesPage::imagesList()
{
    QList<EmailItem> list;
    for (int i = 0 ; i < d->listView->count(); i++)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(d->listView->item(i));
        list.append(item->emailItem());
    }
    return list;
}

}  // namespace KIPISendimagesPlugin
