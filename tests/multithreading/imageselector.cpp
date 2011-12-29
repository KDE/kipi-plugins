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

#include <QPushButton>

// KDE includes

#include <kurl.h>
#include <kdebug.h>

// Kipiplugins includes

#include "imagedialog.h"

ImageSelector::ImageSelector()
{
    QPushButton* button = new QPushButton(this);
    button->setText("Select Images");

    connect(button, SIGNAL(clicked(bool)),
            this, SLOT(selectImages()));

    mainThread = new ActionThread(this);
    mainThread->start();
}

void ImageSelector::selectImages()
{
   KUrl::List selectedImages = KIPIPlugins::ImageDialog::getImageUrls(this, 0);

   //rotate the selected images by 180 degrees
   //It can be converted to gray scale also, just change the function here
   mainThread->rotate(selectedImages);
}
