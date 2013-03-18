/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <klocale.h>
#include <kdebug.h>
#include <kpushbutton.h>

// Local includes

#include "actionthread.h"
#include "kpimageslist.h"

using namespace KIPIJPEGLossLessPlugin;

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

    KPImagesList* listView;

    ActionThread* thread;
};

ImageSelector::ImageSelector(KPAboutData* const about)
    : KPToolDialog(0), d(new ImageSelectorPriv)
{
    setButtons(Help | Apply | Close);
    setButtonText(Apply, i18n("Rotate Items"));
    setDefaultButton(Close);
    setModal(false);
    setAboutData(about);

    d->page                 = new QWidget(this);
    setMainWidget(d->page);
    QGridLayout* mainLayout = new QGridLayout(d->page);

    d->listView             = new KPImagesList(d->page);
    d->listView->setControlButtonsPlacement(KPImagesList::ControlButtonsRight);

    d->progressBar          = new QProgressBar(d->page);
    d->progressBar->setMaximumHeight( fontMetrics().height()+2 );

    mainLayout->addWidget(d->listView,    0, 0, 1, 1);
    mainLayout->addWidget(d->progressBar, 1, 0, 1, 1);
    mainLayout->setRowStretch(0, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    d->thread = new ActionThread(this);

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotStart()));

    connect(d->thread, SIGNAL(starting(KUrl,int)),
            this, SLOT(slotStarting(KUrl,int)));

    connect(d->thread, SIGNAL(finished(KUrl,int)),
            this, SLOT(slotFinished(KUrl,int)));

    connect(d->thread, SIGNAL(failed(KUrl,int,QString)),
            this, SLOT(slotFailed(KUrl,int,QString)));
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
    button(Apply)->setDisabled(true);

    // Rotate the selected images by 180 degrees
    // It can be converted to gray scale also, just change the function here
    d->thread->rotate(selectedImages, KIPIJPEGLossLessPlugin::Rot90);
    d->thread->start();
}

void ImageSelector::slotStarting(const KUrl& url, int)
{
    d->listView->processing(url);
}

void ImageSelector::slotFinished(const KUrl& url, int)
{
    d->listView->processed(url, true);
    d->progressBar->setValue(d->progressBar->value()+1);
    d->listView->updateThumbnail(url);
}

void ImageSelector::slotFailed(const KUrl& url, int, const QString&)
{
    d->listView->processed(url, false);
    d->progressBar->setValue(d->progressBar->value()+1);
}
