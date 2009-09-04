/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#include "imagepreview.h"
#include "imagepreview.moc"

// Qt includes

#include <QLCDNumber>
#include <QSlider>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "kpaboutdata.h"
#include "pixmapview.h"

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

    if (cropActionOrig == true || cropActionDest == true)
        INIT_ZOOM_FACTOR = 20;
    else
        INIT_ZOOM_FACTOR = 5;

    m_zoomSlider->setValue(INIT_ZOOM_FACTOR);
    m_zoomLcd->display(INIT_ZOOM_FACTOR * 5);

    connect(m_zoomSlider, SIGNAL(valueChanged(int)),
            this, SLOT(slotZoomFactorValueChanged(int)));

    connect(m_origView, SIGNAL(wheelChanged(int)),
            this, SLOT(slotWheelChanged(int)));

    connect(m_destView, SIGNAL(wheelChanged(int)),
            this, SLOT(slotWheelChanged(int)));

    m_origView->setZoom(INIT_ZOOM_FACTOR * 5);
    m_destView->setZoom(INIT_ZOOM_FACTOR * 5);

    m_origView->setImage(fileOrig, tmpPath, cropActionOrig);
    m_destView->setImage(fileDest, tmpPath, cropActionDest);
}

ImagePreview::~ImagePreview()
{
    delete m_about;
}

void ImagePreview::slotHelp(void)
{
    KToolInvocation::invokeHelp("", "kipi-plugins");
}

void ImagePreview::slotWheelChanged(int delta)
{
    if (delta > 0)
        m_zoomSlider->setValue(m_zoomSlider->value() - 1);
    else
        m_zoomSlider->setValue(m_zoomSlider->value() + 1);

    slotZoomFactorValueChanged(m_zoomSlider->value());
}

void ImagePreview::slotZoomFactorValueChanged(int ZoomFactorValue)
{
    m_zoomLcd->display(QString::number(ZoomFactorValue * 5));

    m_origView->setZoom(ZoomFactorValue * 5);
    m_destView->setZoom(ZoomFactorValue * 5);
}

}  // namespace KIPIBatchProcessImagesPlugin
