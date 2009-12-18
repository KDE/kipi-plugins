/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a widget to preview image effect.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "previewimage.moc"

// Qt includes

#include <QTimer>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>

// KDE includes

#include <klocale.h>
#include <kiconloader.h>

namespace KIPIPlugins
{

class PreviewImagePriv
{
public:

    PreviewImagePriv()
    {
        textColor     = Qt::white;
        progressPix   = SmallIcon("process-working", 22);
        progressCount = 0;
        progressTimer = 0;
        busy          = false;
    }

    bool     busy;

    QPixmap  pix;
    QPixmap  thumbnail;

    QString  text;
    QColor   textColor;
    QImage   image;

    QPixmap  processAnim;
    QPixmap  progressPix;
    int      progressCount;
    QTimer*  progressTimer;
};

PreviewImage::PreviewImage(QWidget* parent)
             : QWidget(parent), d(new PreviewImagePriv)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(QSize(400, 300));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->progressTimer = new QTimer(this);

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));
}

PreviewImage::~PreviewImage()
{
    delete d;
}

void PreviewImage::load(const QString& file)
{
    d->text.clear();
    d->thumbnail = QPixmap();
    d->pix.fill(Qt::black);
    d->image.load(file);
    setBusy(false);

    if (!d->image.isNull())
    {
        QImage img = d->image.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x      = d->pix.width()/2  - img.width()/2;
        int y      = d->pix.height()/2 - img.height()/2;

        QPainter p(&d->pix);
        p.drawImage(x, y, img);
        p.setPen(QPen(Qt::white));
        p.drawRect(x, y, img.width(), img.height());
        p.end();
    }
    else
    {
        setText(i18n( "Failed to load image after processing" ));
        return;
    }

    update();
}

void PreviewImage::setText(const QString& text, const QColor& color)
{
    d->text      = text;
    d->textColor = color;
    update();
}

void PreviewImage::setThumbnail(const QPixmap& thumbnail)
{
    d->thumbnail = thumbnail;
    update();
}

void PreviewImage::setBusy(bool b, const QString& text)
{
    d->busy = b;

    if (d->busy)
    {
        setCursor( Qt::WaitCursor );
        d->text      = text;
        d->textColor = Qt::white;
        d->progressTimer->start(300);
    }
    else
    {
        unsetCursor();
        d->progressTimer->stop();
    }
}

void PreviewImage::paintEvent(QPaintEvent*)
{
    d->pix = QPixmap(width(), height());
    d->pix.fill(Qt::black);

    if (!d->text.isEmpty() || d->busy || !d->thumbnail.isNull())
    {
        QPainter p(&d->pix);

        if (d->busy)
        {
            p.drawPixmap((d->pix.width()/2)  - (d->processAnim.width()/2),
                         (d->pix.height()/3) - (d->processAnim.height()/2),
                         d->processAnim);
        }
        else if (!d->thumbnail.isNull())
        {
            p.drawPixmap(d->pix.width()/2-d->thumbnail.width()/2, d->pix.height()/4-d->thumbnail.height()/2,
                         d->thumbnail, 0, 0, d->thumbnail.width(), d->thumbnail.height());
            p.setPen(QPen(Qt::white));
            p.drawRect(d->pix.width()/2-d->thumbnail.width()/2, d->pix.height()/4-d->thumbnail.height()/2,
                       d->thumbnail.width()-1, d->thumbnail.height()-1);
        }

        p.setPen(QPen(d->textColor));
        p.drawText(0, 0, d->pix.width(), d->pix.height(),
                    Qt::AlignCenter|Qt::TextWordWrap, d->text);

        p.end();
    }
    else
    {
        if (!d->image.isNull())
        {
            QImage img = d->image.scaled(width(),height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int x      = d->pix.width()/2  - img.width()/2;
            int y      = d->pix.height()/2 - img.height()/2;

            QPainter p(&d->pix);
            p.drawImage(x, y, img);
            p.setPen(QPen(Qt::white));
            p.drawRect(x, y, img.width()-1, img.height()-1);
            p.end();
        }
    }

    QPainter p(this);
    p.drawPixmap(0, 0, d->pix);
    p.end();
}

void PreviewImage::resizeEvent(QResizeEvent*)
{
    update();
}

void PreviewImage::slotProgressTimerDone()
{
    d->processAnim = QPixmap(d->progressPix.copy(0, d->progressCount*22, 22, 22));
    update();

    d->progressCount++;
    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}

} // namespace KIPIPlugins
