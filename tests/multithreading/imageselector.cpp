/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-12-28
 * Description : Simple gui to select images
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy@gmail.com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "imageselector.moc"

// Qt includes

#include <QGridLayout>
#include <QProgressBar>

// KDE includes

#include <kurl.h>
#include <kdebug.h>
#include <kiconloader.h>

// Kipiplugins includes

#include "imagedialog.h"
#include "imageslist.h"

using namespace KIPIPlugins;

class ImageSelector::ImageSelectorPriv
{
public:

    ImageSelectorPriv()
    {
        page               = 0;
        progressBar        = 0;
        listView           = 0;
        thread             = 0;
    }

    QWidget*      page;

    QProgressBar* progressBar;

    ImagesList*   listView;

    ActionThread* thread;
};

ImageSelector::ImageSelector()
    : KDialog(0), d(new ImageSelectorPriv)
{
    setButtons(Apply | Close);
    setDefaultButton(KDialog::Close);
    setModal(false);

    d->page = new QWidget(this);
    setMainWidget(d->page);
    QGridLayout* mainLayout = new QGridLayout(d->page);

    d->listView    = new ImagesList(0, d->page);
    d->listView->setControlButtonsPlacement(ImagesList::ControlButtonsRight);

    d->progressBar = new QProgressBar(d->page);
    d->progressBar->setMaximumHeight( fontMetrics().height()+2 );

    mainLayout->addWidget(d->listView,    0, 0, 1, 1);
    mainLayout->addWidget(d->progressBar, 1, 0, 1, 1);
    mainLayout->setRowStretch(0, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());
    
    d->thread = new ActionThread(this);

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotStart()));

    connect(d->thread, SIGNAL(signalStartToProcess(const KUrl&)),
            this, SLOT(slotStartToProcess(const KUrl&)));

    connect(d->thread, SIGNAL(signalEndToProcess(const KUrl&, bool)),
            this, SLOT(slotEndToProcess(const KUrl&, bool)));
}

ImageSelector::~ImageSelector()
{
    delete d;
}

void ImageSelector::slotStart()
{
    KUrl::List selectedImages = d->listView->imageUrls();
    if (selectedImages.isEmpty()) return;
    
    kDebug() << selectedImages;
    d->progressBar->setMaximum(selectedImages.count());
    d->progressBar->setValue(0);
    
    // Rotate the selected images by 180 degrees
    // It can be converted to gray scale also, just change the function here
    d->thread->rotate(selectedImages);
    d->thread->start();
}

void ImageSelector::slotStartToProcess(const KUrl& url)
{
    ImagesListViewItem* item = d->listView->listView()->findItem(url);
    if (item)
    {
        item->setProcessedIcon(SmallIcon("run-build"));
    }
}

void ImageSelector::slotEndToProcess(const KUrl& url, bool state)
{
    ImagesListViewItem* item = d->listView->listView()->findItem(url);
    if (item)
    {
        item->setProcessedIcon(SmallIcon(state ?  "dialog-ok" : "dialog-cancel"));
        d->progressBar->setValue(d->progressBar->value()+1);
    }
}
