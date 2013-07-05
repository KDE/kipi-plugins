/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : a widget to manage preview.
 *
 * Copyright (C) 2009-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "kppreviewmanager.moc"

// Qt includes

#include <QStyle>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QFrame>
#include <QPushButton>

// KDE includes

#include <klocale.h>
#include <kiconloader.h>
#include <kvbox.h>
#include <khbox.h>
#include <kpixmapsequence.h>

// Local includes

#include "kppreviewimage.h"

namespace KIPIPlugins
{

class KPPreviewManager::Private
{
public:

    Private()
    {
        progressPix   = KPixmapSequence("process-working", KIconLoader::SizeSmallMedium);
        progressCount = 0;
        progressTimer = 0;
        progressLabel = 0;
        thumbLabel    = 0;
        busy          = false;
        textLabel     = 0;
        preview       = 0;
        button        = 0;
    }

    bool            busy;

    QLabel*         textLabel;
    QLabel*         thumbLabel;

    QPushButton*    button;

    int             progressCount;
    KPixmapSequence progressPix;
    QTimer*         progressTimer;
    QLabel*         progressLabel;

    KPPreviewImage* preview;
};

KPPreviewManager::KPPreviewManager(QWidget* const parent)
    : QStackedWidget(parent), d(new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(QSize(400, 300));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    KVBox* vbox      = new KVBox(this);
    QLabel* space1   = new QLabel(vbox);
    d->progressLabel = new QLabel(vbox);
    d->progressLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QLabel* space2   = new QLabel(vbox);
    d->thumbLabel    = new QLabel(vbox);
    d->thumbLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QLabel* space3   = new QLabel(vbox);
    d->textLabel     = new QLabel(vbox);
    d->textLabel->setScaledContents(true);
    d->textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    KHBox *hbox      = new KHBox(vbox);
    QLabel* space4   = new QLabel(hbox);
    d->button        = new QPushButton(hbox);
    d->button->hide();
    QLabel* space5   = new QLabel(hbox);
    hbox->setStretchFactor(space4, 10);
    hbox->setStretchFactor(space5, 10);
    QLabel* space6   = new QLabel(vbox);

    vbox->setStretchFactor(space1, 10);
    vbox->setStretchFactor(d->progressLabel, 5);
    vbox->setStretchFactor(space2, 1);
    vbox->setStretchFactor(d->thumbLabel, 5);
    vbox->setStretchFactor(space3, 1);
    vbox->setStretchFactor(d->textLabel, 5);
    vbox->setStretchFactor(space3, 1);
    vbox->setStretchFactor(hbox, 5);
    vbox->setStretchFactor(space6, 10);
    vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    vbox->setLineWidth( style()->pixelMetric(QStyle::PM_DefaultFrameWidth) );

    d->preview = new KPPreviewImage(this);

    insertWidget(MessageMode, vbox);
    insertWidget(PreviewMode, d->preview);

    d->progressTimer = new QTimer(this);

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    connect(d->button, SIGNAL(clicked()),
            this, SIGNAL(signalButtonClicked()));
}

KPPreviewManager::~KPPreviewManager()
{
    delete d;
}

void KPPreviewManager::slotLoad(const KUrl& url)
{
    load(url.toLocalFile());
}

void KPPreviewManager::setImage(const QImage& img, bool fit)
{
    setBusy(false);

    if (!d->preview->setImage(img))
    {
        setText(i18n( "Failed to load image" ));
        return;
    }

    setCurrentIndex(PreviewMode);
    if (fit)
    {
        d->preview->slotZoom2Fit();
    }
}

bool KPPreviewManager::load(const QString& file, bool fit)
{
    setBusy(false);

    if (!d->preview->load(file))
    {
        setText(i18n( "Failed to load image" ));
        return false;
    }

    setCurrentIndex(PreviewMode);
    if (fit)
    {
        d->preview->slotZoom2Fit();
    }
    return true;
}

void KPPreviewManager::setThumbnail(const QPixmap& thumbnail)
{
    d->thumbLabel->setPixmap(thumbnail);
    setCurrentIndex(MessageMode);
}

void KPPreviewManager::setButtonText(const QString& text)
{
    d->button->setText(text);
}

void KPPreviewManager::setButtonVisible(bool b)
{
    d->button->setVisible(b);
}

void KPPreviewManager::setSelectionAreaPossible(bool b)
{
    d->preview->enableSelectionArea(b);
}

QRectF KPPreviewManager::getSelectionArea()
{
    return d->preview->getSelectionArea();
}

void KPPreviewManager::setSelectionArea(QRectF rectangle)
{
    d->preview->setSelectionArea(rectangle);
}

void KPPreviewManager::setText(const QString& text, const QColor& color)
{
    d->textLabel->setText(QString("<qt text=\"%1\">%2</qt>").arg(color.name()).arg(text));
    setCurrentIndex(MessageMode);
}

void KPPreviewManager::setBusy(bool b, const QString& text)
{
    d->busy = b;

    if (d->busy)
    {
        setCursor( Qt::WaitCursor );
        d->progressTimer->start(300);
        setText(text);
    }
    else
    {
        unsetCursor();
        d->progressTimer->stop();
        setText(text);
        d->progressLabel->setPixmap(QPixmap());
    }
}

void KPPreviewManager::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(d->progressPix.frameAt(d->progressCount));
    d->progressCount++;
    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}

} // namespace KIPIPlugins
