/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : a widget to display the imagelist
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// libkdcraw includes.

#include <libkdcraw/kdcraw.h>

// Local includes.

#include "imagedialog.h"

namespace KIPIRemoveRedEyesPlugin
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
    setText(2, m_url.fileName());
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
    setWhatsThis(i18n("This is the list of images to remove red eyes from."));
    setAcceptDrops(true);
    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(3);
    setHeaderLabels(QStringList() << i18n("Thumbnail") << i18n("Corrected Eyes") << i18n("File Name"));
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

    QLabel*         totalLabel;
    QLabel*         processedLabel;
    QLabel*         failedLabel;

    QPushButton     *addButton;
    QPushButton     *removeButton;

    ImagesListView  *listView;

    KIPI::Interface *iface;
};

ImagesList::ImagesList(KIPI::Interface *iface, QWidget* parent)
          : QWidget(parent),
            d(new ImagesListPriv)
{
    d->iface = iface;

    // --------------------------------------------------------

    QGridLayout* mainLayout = new QGridLayout;
    d->listView             = new ImagesListView;

    // --------------------------------------------------------

    d->addButton    = new QPushButton;
    d->removeButton = new QPushButton;
    d->addButton->setText(i18n("&Add"));
    d->addButton->setIcon(SmallIcon("list-add"));
    d->removeButton->setText(i18n("&Remove"));
    d->removeButton->setIcon(SmallIcon("list-remove"));

    QGroupBox* summaryGroupBox = new QGroupBox(i18n("Summary"));

    d->totalLabel       = new QLabel;
    d->processedLabel   = new QLabel;
    d->failedLabel      = new QLabel;
    d->totalLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    d->processedLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    d->failedLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);

    QGridLayout* summaryGroupBoxLayout = new QGridLayout;
    summaryGroupBoxLayout->addWidget(new QLabel(
            i18nc("The total number of images in the list",
                  "total:")),                           0, 0, 1, 1);
    summaryGroupBoxLayout->addWidget(new QLabel(
            i18nc("number of images successfully processed",
                  "success:")),                         1, 0, 1, 1);
    summaryGroupBoxLayout->addWidget(new QLabel(
            i18nc("number of images failed to process",
                  "failed:")),                          2, 0, 1, 1);
    summaryGroupBoxLayout->addWidget(d->totalLabel,     0, 1, 1, 1);
    summaryGroupBoxLayout->addWidget(d->processedLabel, 1, 1, 1, 1);
    summaryGroupBoxLayout->addWidget(d->failedLabel,    2, 1, 1, 1);
    summaryGroupBox->setLayout(summaryGroupBoxLayout);

    // --------------------------------------------------------

    mainLayout->addWidget(d->listView,     0, 0, 5, 1);
    mainLayout->addWidget(d->addButton,    0, 1, 1, 1);
    mainLayout->addWidget(d->removeButton, 1, 1, 1, 1);
    mainLayout->addWidget(summaryGroupBox, 2, 1, 1, 1);
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

    connect(this, SIGNAL(signalImageListChanged(bool)),
            this, SLOT(slotUpdateSummary()));
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

        // RAW files are not allowed, don't add them to ImagesList
        if (!found)
        {
            if (!isRAWFile(imageUrl.path()))
            {
                new ImagesListViewItem(d->listView, imageUrl);
                urls.append(imageUrl);
            }
            else
            {
                raw = true;
            }
        }
    }

    d->iface->thumbnails(urls, ICONSIZE);

    emit signalImageListChanged(imageUrls().isEmpty());
    emit signalFoundRAWImages(raw);

    slotUpdateSummary();
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

    emit signalImageListChanged(imageUrls().isEmpty());
}

void ImagesList::slotRemoveItems()
{
    bool found;
    do
    {
        found = false;
        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
            if (item->isSelected())
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

void ImagesList::slotAddEyeCounterByUrl(const KUrl& url, int eyes)
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (item->url() == url)
        {
            item->setText(1, QString::number(eyes));
            break;
        }
        ++it;
    }
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

void ImagesList::resetEyeCounterColumn()
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (item)
            item->setText(1, QString(""));
        ++it;
    }

    emit signalImageListChanged(imageUrls().isEmpty());
}

bool ImagesList::hasNoneCorrectedImages()
{
    bool hasNone = false;

    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (item)
        {
            if (item->text(1).isEmpty() || item->text(1).toInt() <= 0)
            {
                hasNone = true;
                break;
            }
        }
        ++it;
    }
    return hasNone;
}

void ImagesList::removeNoneCorrectedImages()
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (item)
        {
            // first, reset any selected items
            item->setSelected(false);

            // now select those with no corrections to remove them
            if (item->text(1).isEmpty() || item->text(1).toInt() <= 0)
            {
                item->setSelected(true);
            }
        }
        ++it;
    }
    slotRemoveItems();
}

bool ImagesList::isRAWFile(const QString & filePath)
{
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());

    QFileInfo fileInfo(filePath);
    if (rawFilesExt.toUpper().contains( fileInfo.suffix().toUpper() ))
        return true;

    return false;
}

void ImagesList::slotUpdateSummary()
{
    int total       = imageUrls().count();
    int processed   = 0;
    int failed      = 0;

    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (!item->text(1).isEmpty())
        {
            if (item->text(1).toInt() > 0)
                processed++;
            else
                failed++;
        }
        ++it;
    }

    d->totalLabel->setText(QString("%1").arg(total));
    d->processedLabel->setText(QString("%1").arg(processed));
    d->failedLabel->setText(QString("%1").arg(failed));
}

}  // namespace KIPIRemoveRedEyesPlugin
