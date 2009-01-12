/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : a widget to display the imagelist
 *
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

#define ICONSIZE 64

#include "myimageslist.h"
#include "myimageslist.moc"

// Qt includes.

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>

// KDE includes.

#include <klocale.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes.

#include "imagedialog.h"

using namespace KIPIPlugins;

namespace KIPIRemoveRedEyesPlugin
{

struct MyImagesListPriv
{
    MyImagesListPriv()
    {
        iface           = 0;
        totalLabel      = 0;
        processedLabel  = 0;
        failedLabel     = 0;
    }

    int              total;
    int              processed;
    int              failed;

    QLabel*          totalLabel;
    QLabel*          processedLabel;
    QLabel*          failedLabel;

    KIPI::Interface* iface;
};

MyImagesList::MyImagesList(KIPI::Interface *iface, QWidget* parent)
            : ImagesList(iface, parent), d(new MyImagesListPriv)
{
    d->iface = iface;
    setAllowRAW(false);

    // --------------------------------------------------------

    listView()->setColumn(ImagesListView::User1, i18n("Corrected Eyes"), true);
    listView()->header()->setResizeMode(QHeaderView::Stretch);
    listView()->setWhatsThis(i18n("This is the list of images to remove red eyes from."));

    // --------------------------------------------------------

    QGroupBox* summaryGroupBox = new QGroupBox(i18n("Summary"));

    d->totalLabel     = new QLabel;
    d->processedLabel = new QLabel;
    d->failedLabel    = new QLabel;

    d->totalLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    d->processedLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    d->failedLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);

    QLabel* l1 = new QLabel(i18nc("The total number of images in the list",
                                  "Total:"));
    QLabel* l2 = new QLabel(i18nc("number of images successfully processed",
                                  "Success:"));
    QLabel* l3 = new QLabel(i18nc("number of images failed to process",
                                  "Failed:"));

    QGridLayout* summaryGroupBoxLayout = new QGridLayout;
    summaryGroupBoxLayout->addWidget(l1,                0, 0, 1, 1);
    summaryGroupBoxLayout->addWidget(l2,                1, 0, 1, 1);
    summaryGroupBoxLayout->addWidget(l3,                2, 0, 1, 1);
    summaryGroupBoxLayout->addWidget(d->totalLabel,     0, 1, 1, 1);
    summaryGroupBoxLayout->addWidget(d->processedLabel, 1, 1, 1, 1);
    summaryGroupBoxLayout->addWidget(d->failedLabel,    2, 1, 1, 1);
    summaryGroupBox->setLayout(summaryGroupBoxLayout);

    // --------------------------------------------------------

    QGridLayout* mainLayout = new QGridLayout(plainPage());
    mainLayout->addWidget(summaryGroupBox, 0, 0, 1, 1);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    // --------------------------------------------------------

    connect(this, SIGNAL(signalImageListChanged(bool)),
            this, SLOT(updateSummary()));
}

MyImagesList::~MyImagesList()
{
    delete d;
}

void MyImagesList::addEyeCounterByUrl(const KUrl& url, int eyes)
{
    QTreeWidgetItemIterator it(listView());
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (item->url() == url)
        {
            item->setText(ImagesListView::User1, QString::number(eyes));
            break;
        }
        ++it;
    }
    emit signalImageListChanged(imageUrls().isEmpty());
}

void MyImagesList::resetEyeCounterColumn()
{
    QTreeWidgetItemIterator it(listView());
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        item->setText(ImagesListView::User1, QString(""));
        ++it;
    }
    emit signalImageListChanged(imageUrls().isEmpty());
}

bool MyImagesList::hasUnprocessedImages()
{
    bool hasNone = false;

    QTreeWidgetItemIterator it(listView());
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (item->text(ImagesListView::User1).toInt() <= 0)
        {
            hasNone = true;
            break;
        }
        ++it;
    }
    return hasNone;
}

void MyImagesList::removeUnprocessedImages()
{
    QTreeWidgetItemIterator it(listView());
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        // first, deselect item if selected
        item->setSelected(false);

        // select the item if no corrections were made
        if (item->text(ImagesListView::User1).toInt() <= 0 &&
            !item->text(ImagesListView::User1).isEmpty())
        {
            item->setSelected(true);
        }
        ++it;
    }
    slotRemoveItems();
}

int MyImagesList::totalImages() const
{
    return d->total;
}

int MyImagesList::processedImages() const
{
    return d->processed;
}

int MyImagesList::failedImages() const
{
    return d->failed;
}

void MyImagesList::resetCounters()
{
    d->total       = imageUrls().count();
    d->processed   = 0;
    d->failed      = 0;
}

void MyImagesList::updateSummary()
{
    resetCounters();

    QTreeWidgetItemIterator it(listView());
    while (*it)
    {
        ImagesListViewItem* item = dynamic_cast<ImagesListViewItem*>(*it);
        if (!item->text(ImagesListView::User1).isEmpty())
        {
            if (item->text(ImagesListView::User1).toInt() > 0)
                d->processed++;
            else
                d->failed++;
        }
        ++it;
    }

    d->totalLabel->setText(QString("%1").arg(d->total));
    d->processedLabel->setText(QString("%1").arg(d->processed));
    d->failedLabel->setText(QString("%1").arg(d->failed));
}

}  // namespace KIPIRemoveRedEyesPlugin
