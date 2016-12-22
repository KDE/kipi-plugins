/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 * Copyright (C) 2011-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imageselector.h"

// Qt includes

#include <QGridLayout>
#include <QProgressBar>
#include <QDebug>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "actionthread.h"
#include "kpimageslist.h"

class ImageSelector::Private
{
public:

    Private()
    {
        page        = 0;
        progressBar = 0;
        listView    = 0;
        thread      = 0;
    }

    QWidget*      page;

    QProgressBar* progressBar;

    KPImagesList* listView;

    ActionThread* thread;
};

ImageSelector::ImageSelector(KPAboutData* const about)
    : KPToolDialog(0), d(new Private)
{
    startButton()->setText(i18n("Rotate Items"));
    setModal(false);
    setAboutData(about);

    d->page                       = new QWidget(this);
    setMainWidget(d->page);
    QGridLayout* const mainLayout = new QGridLayout(d->page);

    d->listView                   = new KPImagesList(d->page);
    d->listView->setControlButtonsPlacement(KPImagesList::ControlButtonsRight);

    d->progressBar                = new QProgressBar(d->page);
    d->progressBar->setMaximumHeight( fontMetrics().height()+2 );

    mainLayout->addWidget(d->listView,    0, 0, 1, 1);
    mainLayout->addWidget(d->progressBar, 1, 0, 1, 1);
    mainLayout->setRowStretch(0, 10);

    d->thread = new ActionThread(this);

    connect(startButton(), &QPushButton::clicked,
            this, &ImageSelector::slotStart);

    connect(d->thread, &ActionThread::starting,
            this, &ImageSelector::slotStarting);

    connect(d->thread, &ActionThread::finished,
            this, &ImageSelector::slotFinished);

    connect(d->thread, &ActionThread::failed,
            this, &ImageSelector::slotFailed);
}

ImageSelector::~ImageSelector()
{
    delete d;
}

void ImageSelector::slotStart()
{
    QList<QUrl> selectedImages = d->listView->imageUrls();
    if (selectedImages.isEmpty()) return;

    qDebug() << selectedImages;
    d->progressBar->setMaximum(selectedImages.count());
    d->progressBar->setValue(0);
    startButton()->setEnabled(false);

    // Rotate the selected images by 180 degrees
    // It can be converted to gray scale also, just change the function here
    d->thread->rotate(selectedImages);
    d->thread->start();
}

void ImageSelector::slotStarting(const QUrl& url)
{
    d->listView->processing(url);
}

void ImageSelector::slotFinished(const QUrl& url)
{
    d->listView->processed(url, true);
    d->progressBar->setValue(d->progressBar->value()+1);
    d->listView->updateThumbnail(url);
}

void ImageSelector::slotFailed(const QUrl& url, const QString&)
{
    d->listView->processed(url, false);
    d->progressBar->setValue(d->progressBar->value()+1);
}
