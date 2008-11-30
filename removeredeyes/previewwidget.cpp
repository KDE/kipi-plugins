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

#include <QString>
#include <QVBoxLayout>
#include <QWidget>

// KDE includes.

#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>

// Local includes.

#include "infomessagewidget.h"

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
    }

    bool                busy;

    QLabel*             busyLabel;
    QLabel*             noSelectionLabel;

    QLabel*             originalLabel;
    QLabel*             correctedLabel;
    QLabel*             maskLabel;

    KUrl                currentImage;

    InfoMessageWidget*  modeInfo;
};

PreviewWidget::PreviewWidget(QWidget* parent)
             : QStackedWidget(parent),
               d(new PreviewWidgetPriv)
{
    QString whatsThis = i18n("<p>This widget will display a correction preview for the currently selected image</p>"
                             "<p><ul>"
                             "<li>Move the mouse <b>over</b> the preview to display the original image</li>"
                             "<li>Move the mouse <b>out of</b> the preview to display the corrected image</li>"
                             "<li><b>Click on</b> the preview to display the correction mask</li>"
                             "</ul></p>"
                             "<p>If the info widget hides important image parts, click on it to make it disappear.</p>");

    setWhatsThis(whatsThis);

    // --------------------------------------------------------

    d->busy                 = false;
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

    insertWidget(BusyMode,          d->busyLabel);
    insertWidget(NoSelectionMode,   d->noSelectionLabel);
    insertWidget(OriginalMode,      d->originalLabel);
    insertWidget(CorrectedMode,     d->correctedLabel);
    insertWidget(MaskMode,          d->maskLabel);

    // --------------------------------------------------------

    d->modeInfo     = new InfoMessageWidget(this);
}

PreviewWidget::~PreviewWidget()
{
    delete d;
}

void PreviewWidget::setPreviewImage(const QString& filename, ImageType type)
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

void PreviewWidget::setBusy(bool busy)
{
    d->busy = busy;

    if (busy)
        setMode(BusyMode);
    else
        setMode(CorrectedMode);
}

void PreviewWidget::enterEvent (QEvent*)
{
    if (d->currentImage.isEmpty())
    {
        setMode(NoSelectionMode);
        return;
    }

    if (!d->busy)
        setMode(OriginalMode);
}

void PreviewWidget::leaveEvent (QEvent*)
{
    if (d->currentImage.isEmpty())
    {
        setMode(NoSelectionMode);
        return;
    }

    if (!d->busy)
        setMode(CorrectedMode);
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent*)
{
    if (d->currentImage.isEmpty())
    {
        setMode(NoSelectionMode);
        return;
    }

    if (!d->busy)
    {
        if (currentIndex() == MaskMode)
            setMode(OriginalMode);
        else
            setMode(MaskMode);
    }
}

void PreviewWidget::setCurrentImage(const KUrl& url)
{
    d->currentImage = url;
    d->modeInfo->reset();
}

void PreviewWidget::setMode(PreviewMode mode)
{
    setCurrentIndex(mode);

    switch (mode)
    {
        case OriginalMode:
            d->modeInfo->display(i18n("Original Image"));
            d->modeInfo->raise();
            break;

        case CorrectedMode:
            d->modeInfo->display(i18n("Corrected Image"));
            d->modeInfo->raise();
            break;

        case MaskMode:
            d->modeInfo->display(i18n("Correction Mask"));
            d->modeInfo->raise();
            break;

        default:
            d->modeInfo->lower();
    }
}

void PreviewWidget::reset()
{
    d->currentImage.clear();
    d->modeInfo->reset();
    setMode(NoSelectionMode);
}

} // namspace KIPIRemoveRedEyesPlugin
