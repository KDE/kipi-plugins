/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-11-29
 * Description : a preview widget to display correction results
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "previewwidget.h"
#include "previewwidget.moc"

// Qt includes.

#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>

// KDE includes.

#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>

// Local includes.

#include "infomessagewidget.h"
#include "previewzoombar.h"

namespace KIPIRemoveRedEyesPlugin
{

class PreviewWidgetPriv
{
public:

    PreviewWidgetPriv()
    {
        busyLabel           = 0;
        noSelectionLabel    = 0;
        originalLabel       = 0;
        correctedLabel      = 0;
        maskLabel           = 0;
        modeInfo            = 0;
        zoomBar             = 0;
    }

    bool                locked;

    QLabel*             busyLabel;
    QLabel*             noSelectionLabel;

    QLabel*             originalLabel;
    QLabel*             correctedLabel;
    QLabel*             maskLabel;

    QStackedWidget*     stack;


    QScrollArea*        scrollArea;

    QString             image;

    InfoMessageWidget*  modeInfo;

    PreviewZoomBar*     zoomBar;
};

PreviewWidget::PreviewWidget(QWidget* parent)
             : QWidget(parent),
               d(new PreviewWidgetPriv)
{
    QString whatsThis = i18n("<p>This widget will display a correction "
            "preview for the currently selected image</p>"
            "<p><ul>"
            "<li>Move the mouse <b>over</b> the preview to display the original image</li>"
            "<li>Move the mouse <b>out of</b> the preview to display the corrected image</li>"
            "<li><b>Click on</b> the preview to display the correction mask</li>"
            "</ul></p>"
            "<p>If the info widget hides important image parts, click on it to "
            "make it disappear.</p>");

    setWhatsThis(whatsThis);

    // --------------------------------------------------------

    d->locked               = true;
    setBackgroundRole(QPalette::Dark);

    // --------------------------------------------------------

    d->busyLabel            = new QLabel;
    d->noSelectionLabel     = new QLabel;
    d->originalLabel        = new QLabel;
    d->correctedLabel       = new QLabel;
    d->maskLabel            = new QLabel;

    d->originalLabel->setScaledContents(true);
    d->correctedLabel->setScaledContents(true);
    d->maskLabel->setScaledContents(true);

    // --------------------------------------------------------

    d->noSelectionLabel->setText(i18n("<h2>no image selected!</h2>"));
    d->noSelectionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    d->busyLabel->setText(i18n("<h2>generating preview...</h2>"));
    d->busyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    d->stack      = new QStackedWidget;
    d->stack->insertWidget(BusyMode,          d->busyLabel);
    d->stack->insertWidget(LockedMode,        d->noSelectionLabel);
    d->stack->insertWidget(OriginalMode,      d->originalLabel);
    d->stack->insertWidget(CorrectedMode,     d->correctedLabel);
    d->stack->insertWidget(MaskMode,          d->maskLabel);

    d->scrollArea = new QScrollArea;
    d->scrollArea->setWidget(d->stack);
    d->scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->scrollArea->setWidgetResizable(true);

    // --------------------------------------------------------

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(d->scrollArea);
    setLayout(layout);

    // --------------------------------------------------------

    // floating widgets
    d->modeInfo = new InfoMessageWidget(d->scrollArea);
    d->zoomBar  = new PreviewZoomBar(d->scrollArea);

    // --------------------------------------------------------

    connect(this, SIGNAL(settingsChanged()),
            this, SLOT(updateSettings()));

    // --------------------------------------------------------

    reset();
}

PreviewWidget::~PreviewWidget()
{
    delete d;
}

QString& PreviewWidget::image() const
{
    return d->image;
}

void PreviewWidget::setImage(const QString& image)
{
    if (d->image == image)
            return;

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
            QString message = i18n("<p>Can not open preview image<br/>'%1'</p>.",
                                   filename);

            KMessageBox::information(this, message,
                                     i18n("Error loading preview file"));

            return QPixmap();
        }
    }
    return image;
}

void PreviewWidget::enterEvent (QEvent*)
{
    if (d->locked)
        return;

    setMode(OriginalMode);
}

void PreviewWidget::leaveEvent (QEvent*)
{
    if (d->locked)
        return;

    setMode(CorrectedMode);
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent*)
{
    if (d->locked)
        return;

    if (d->stack->currentIndex() == MaskMode)
        setMode(OriginalMode);
    else
        setMode(MaskMode);
}

void PreviewWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    int y = d->scrollArea->viewport()->height() - d->zoomBar->height() - 10;

    d->zoomBar->setMinMaxWidth(d->scrollArea->viewport()->width());
    d->zoomBar->move(0, y);
}

void PreviewWidget::setMode(DisplayMode mode)
{
    d->stack->setCurrentIndex(mode);

    switch (mode)
    {
        case OriginalMode:
            d->modeInfo->display(i18n("Original Image"));
            d->modeInfo->raise();
            d->zoomBar->show();
            d->zoomBar->raise();
            break;

        case CorrectedMode:
            d->modeInfo->display(i18n("Corrected Image"));
            d->modeInfo->raise();
            d->zoomBar->show();
            d->zoomBar->raise();
            break;

        case MaskMode:
            d->modeInfo->display(i18n("Correction Mask"));
            d->modeInfo->raise();
            d->zoomBar->show();
            d->zoomBar->raise();
            break;

        default:
            d->modeInfo->lower();
            d->zoomBar->hide();
            d->zoomBar->lower();
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

bool PreviewWidget::previewsComplete()
{
    if (d->originalLabel->pixmap()->isNull()  ||
        d->correctedLabel->pixmap()->isNull() ||
        d->maskLabel->pixmap()->isNull())
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
        d->modeInfo->reset();
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

void PreviewWidget::zoomIn()
{
//    scaleImage(1.25);
}

void PreviewWidget::zoomOut()
{
//    scaleImage(0.8);
}

void PreviewWidget::normalSize()
{
//    imageLabel->adjustSize();
//    scaleFactor = 1.0;
}

void PreviewWidget::fitToWindow()
{
//    bool fitToWindow = fitToWindowAct->isChecked();
//    scrollArea->setWidgetResizable(fitToWindow);
//    if (!fitToWindow) {
//        normalSize();
//    }
//    updateActions();
}

} // namspace KIPIRemoveRedEyesPlugin
