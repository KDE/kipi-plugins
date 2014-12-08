/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imagepreview.h"

// Qt includes

#include <QImage>

// KDE includes

#include <klocale.h>

// Local includes

#include "kpaboutdata.h"
#include "kppreviewmanager.h"

namespace KIPIBatchProcessImagesPlugin
{

ImagePreview::ImagePreview(const QString& fileOrig, 
                           const QString& fileDest,
                           bool cropActionOrig, 
                           const QString& EffectName,
                           const QString& FileName, 
                           QWidget* const parent)
        : KPToolDialog(parent)
{
    setCaption(i18n("Batch Process Preview (%1 - %2)", EffectName, FileName));
    setModal(true);
    setButtons(Ok | Help);
    setDefaultButton(Ok);

    KPAboutData* about = new KPAboutData(ki18n("Batch Process Images"),
                                         QByteArray(),
                                         KAboutData::License_GPL,
                                         ki18n("A Kipi plugin for batch process images using \"ImageMagick\""),
                                         ki18n("(c) 2003-2012, Gilles Caulier\n"
                                               "(c) 2007-2009, Aurélien Gateau"));

    about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");

    setAboutData(about);

    //---------------------------------------------

    QWidget* box = new QWidget(this);
    setupUi(box);
    setMainWidget(box);
    resize(700, 400);

    QImage original(fileOrig);
    const QImage cropped = original.copy(0, 0, 300, 300);

    if (cropActionOrig)
    {
        m_origView->setImage(cropped, true);
    }

    else
    {
        m_origView->load(fileOrig, true);
    }

    m_destView->load(fileDest, true);
}

ImagePreview::~ImagePreview()
{
}

}  // namespace KIPIBatchProcessImagesPlugin
