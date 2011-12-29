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

// KDE includes

#include <kurl.h>
#include <kdebug.h>

// Kipiplugins includes

#include "imagedialog.h"

ImageSelector::ImageSelector()
{
    setText("Select Images");

    connect(this, SIGNAL(clicked(bool)),
            this, SLOT(selectImages()));

    m_mainThread = new ActionThread(this);
}

void ImageSelector::selectImages()
{
    KUrl::List selectedImages = KIPIPlugins::ImageDialog::getImageUrls(this, 0);
    if (selectedImages.isEmpty()) return;
    
    kDebug() << selectedImages;
    setDisabled(true);
    
    // Rotate the selected images by 180 degrees
    // It can be converted to gray scale also, just change the function here
    m_mainThread->rotate(selectedImages);
    m_mainThread->start();
}
