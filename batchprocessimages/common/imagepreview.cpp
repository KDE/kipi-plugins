/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imagepreview.moc"

// Qt includes

#include <QImage>
#include <QPixmap>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "kpaboutdata.h"
#include "kppreviewmanager.h"

namespace KIPIBatchProcessImagesPlugin
{

ImagePreview::ImagePreview(const QString &fileOrig, const QString &fileDest, const QString &tmpPath,
                           bool cropActionOrig, bool cropActionDest, const QString &EffectName,
                           const QString &FileName, QWidget *parent)
        : KDialog(parent)
{
    setCaption(i18n("Batch Process Preview (%1 - %2)", EffectName, FileName));
    setModal(true);
    setButtons(Ok | Help);
    setDefaultButton(Ok);
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch process images"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("An interface to preview the \"Batch Process Images\" "
                                                 "Kipi plugin.\n"
                                                 "This plugin uses the \"convert\" program from the \"ImageMagick\" "
                                                 "package."),
                                           ki18n("(c) 2003-2009, Gilles Caulier\n"
                                                 "(c) 2007-2009, Aurélien Gateau"));

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");

    DialogUtils::setupHelpButton(this, m_about);

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
    delete m_about;
}

void ImagePreview::slotHelp(void)
{
    KToolInvocation::invokeHelp("", "kipi-plugins");
}

}  // namespace KIPIBatchProcessImagesPlugin
