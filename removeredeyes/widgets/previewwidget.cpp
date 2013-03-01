/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-29
 * Description : a preview widget to display correction results
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "previewwidget.moc"

// Qt includes

#include <QGraphicsScene>
#include <QStackedWidget>

// KDE includes

#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>

// Local includes

#include "controlwidget.h"
#include "infomessagewidget.h"

namespace KIPIRemoveRedEyesPlugin
{

struct PreviewWidget::Private
{
    Private() :
        locked(false),
        busyLabel(0),
        noSelectionLabel(0),
        originalLabel(0),
        correctedLabel(0),
        maskLabel(0),
        stack(0),
        controller(0),
        modeInfo(0)
    {
    }

    bool                locked;

    QLabel*             busyLabel;
    QLabel*             noSelectionLabel;
    QLabel*             originalLabel;
    QLabel*             correctedLabel;
    QLabel*             maskLabel;

    QStackedWidget*     stack;

    QString             image;

    ControlWidget*      controller;

    InfoMessageWidget*  modeInfo;
};

PreviewWidget::PreviewWidget(QWidget* const parent)
    : QGraphicsView(parent), d(new Private)
{
    QString whatsThis = i18n("<p>This widget will display a correction "
                             "preview for the currently selected image.</p>"
                             "<p><ul>"
                             "<li>Move the mouse <b>over</b> the preview to display the original image.</li>"
                             "<li>Move the mouse <b>out of</b> the preview to display the corrected image.</li>"
                             "<li><b>Click on</b> the preview to display the correction mask.</li>"
                             "</ul></p>"
                             "<p>The zoom buttons and panning widget allow you to view certain parts of the image "
                             "more closely.</p>");

    setWhatsThis(whatsThis);

    // --------------------------------------------------------

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setCacheMode(QGraphicsView::CacheBackground);

    // --------------------------------------------------------

    d->locked               = true;
    d->busyLabel            = new QLabel;
    d->correctedLabel       = new QLabel;
    d->maskLabel            = new QLabel;
    d->noSelectionLabel     = new QLabel;
    d->originalLabel        = new QLabel;

    d->correctedLabel->setScaledContents(true);
    d->maskLabel->setScaledContents(true);
    d->originalLabel->setScaledContents(true);

    d->noSelectionLabel->clear();

    d->busyLabel->setText(i18n("<h2>generating preview...</h2>"));
    d->busyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // --------------------------------------------------------

    d->stack = new QStackedWidget;
    d->stack->insertWidget(BusyMode,      d->busyLabel);
    d->stack->insertWidget(LockedMode,    d->noSelectionLabel);
    d->stack->insertWidget(OriginalMode,  d->originalLabel);
    d->stack->insertWidget(CorrectedMode, d->correctedLabel);
    d->stack->insertWidget(MaskMode,      d->maskLabel);

    // --------------------------------------------------------

    QGraphicsScene* const scene = new QGraphicsScene;
    scene->addWidget(d->stack);
    setScene(scene);

    // --------------------------------------------------------

    // floating widgets
    d->modeInfo   = new InfoMessageWidget(this);
    d->controller = new ControlWidget(this);

    // --------------------------------------------------------

    connect(this, SIGNAL(settingsChanged()),
            this, SLOT(updateSettings()));

    connect(d->controller, SIGNAL(zoomInClicked()),
            this, SLOT(zoomInClicked()));

    connect(d->controller, SIGNAL(zoomOutClicked()),
            this, SLOT(zoomOutClicked()));

    connect(d->controller, SIGNAL(originalClicked()),
            this, SLOT(originalClicked()));

    connect(d->controller, SIGNAL(correctedClicked()),
            this, SLOT(correctedClicked()));

    connect(d->controller, SIGNAL(maskClicked()),
            this, SLOT(maskClicked()));

    // --------------------------------------------------------

    reset();
}

PreviewWidget::~PreviewWidget()
{
    delete d;
}

QString& PreviewWidget::currentImage() const
{
    return d->image;
}

void PreviewWidget::setCurrentImage(const QString& image)
{
    if (d->image == image)
    {
        return;
    }

    d->image = image;
    resetPreviews();
    emit settingsChanged();
}

void PreviewWidget::setPreviewImage(ImageType type, const QString& filename)
{
    switch (type)
    {
        case OriginalImage:
            d->originalLabel->setPixmap(openFile(filename));
            break;

        case CorrectedImage:
            d->correctedLabel->setPixmap(openFile(filename));
            break;

        case MaskImage:
            d->maskLabel->setPixmap(openFile(filename));
            break;
    }

    emit settingsChanged();
}

QPixmap PreviewWidget::openFile(const QString& filename)
{
    QPixmap image;

    if (!filename.isEmpty())
    {
        image.load(filename);

        if (image.isNull())
        {
            QString message = i18n("<p>Can not open preview image<br/>'%1'</p>.", filename);

            KMessageBox::information(this, message, i18n("Error loading preview file"));
            return QPixmap();
        }
    }

    return image;
}

void PreviewWidget::originalClicked()
{
    if (d->locked)
    {
        return;
    }

    setMode(OriginalMode);
}

void PreviewWidget::correctedClicked()
{
    if (d->locked)
    {
        return;
    }

    setMode(CorrectedMode);
}

void PreviewWidget::maskClicked()
{
    if (d->locked)
    {
        return;
    }

    if (d->stack->currentIndex() == MaskMode)
    {
        setMode(OriginalMode);
    }
    else
    {
        setMode(MaskMode);
    }
}

void PreviewWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    d->controller->move((width()/2) -  (d->controller->width()/2),
                        (height()/2) - (d->controller->height()/2));
}

void PreviewWidget::setMode(DisplayMode mode)
{
    d->stack->setCurrentIndex(mode);

    switch (mode)
    {
        case OriginalMode:
            d->modeInfo->display(i18n("Original Image"));
            d->modeInfo->raise();
            d->controller->raise();
            break;

        case CorrectedMode:
            d->modeInfo->display(i18n("Corrected Image"));
            d->modeInfo->raise();
            d->controller->raise();
            break;

        case MaskMode:
            d->modeInfo->display(i18n("Correction Mask"));
            d->modeInfo->raise();
            d->controller->raise();
            break;

        case LockedMode:
            d->modeInfo->display(i18n("No image selected"), InfoMessageWidget::Warning);
            d->modeInfo->raise();
            d->controller->hide();
            d->controller->lower();
            break;

        case BusyMode:
            d->modeInfo->lower();
            d->controller->hide();
            d->controller->lower();
            break;
    }

    d->stack->adjustSize();
}

void PreviewWidget::reset()
{
    d->image.clear();
    resetPreviews();
}

void PreviewWidget::resetPreviews()
{
    d->originalLabel->setPixmap(0);
    d->correctedLabel->setPixmap(0);
    d->maskLabel->setPixmap(0);
    updateSettings();
}

bool PreviewWidget::previewsComplete() const
{
    if (d->originalLabel->pixmap()->isNull()  || d->correctedLabel->pixmap()->isNull() || d->maskLabel->pixmap()->isNull())
    {
        return false;
    }

    return true;
}

void PreviewWidget::updateSettings()
{
    if (d->image.isEmpty())
    {
        d->locked = true;
        setMode(LockedMode);
        return;
    }

    if (!previewsComplete())
    {
        d->locked = true;
        setMode(BusyMode);
        d->modeInfo->reset();
        return;
    }

    d->locked = false;
    setMode(CorrectedMode);
}

void PreviewWidget::zoomInClicked()
{
    scale(1.5, 1.5);
}

void PreviewWidget::zoomOutClicked()
{
    scale(1.0 / 1.5, 1.0 / 1.5);
}

void PreviewWidget::enterEvent(QEvent*)
{
    d->controller->triggerShow();
}

} // namspace KIPIRemoveRedEyesPlugin
