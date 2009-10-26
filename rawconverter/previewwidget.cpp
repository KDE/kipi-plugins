/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-22
 * Description : widget to render thumbnail of raw file.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "previewwidget.h"
#include "previewwidget.moc"

// Qt includes

#include <QImage>
#include <QPaintEvent>
#include <QPainter>

// KDE includes

#include <klocale.h>

namespace KIPIRawConverterPlugin
{

class PreviewWidgetPriv
{
public:

    PreviewWidgetPriv()
    {
        textColor = Qt::white;
    }

    QPixmap  pix;
    QPixmap  thumbnail;

    QString  text;
    QColor   textColor;
    QImage   image;
};

PreviewWidget::PreviewWidget(QWidget *parent)
             : QWidget(parent), d(new PreviewWidgetPriv)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(QSize(400, 300));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

PreviewWidget::~PreviewWidget()
{
    delete d;
}

void PreviewWidget::load(const QString& file)
{
    d->text = QString();
    d->pix.fill(Qt::black);
    d->image.load(file);

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
        setIdentity(i18n( "Failed to load image after processing" ));
        return;
    }

    update();
}

void PreviewWidget::setIdentity(const QString& text, const QColor& color)
{
    d->text      = text;
    d->textColor = color;
    update();
}

void PreviewWidget::setThumbnail(const QPixmap& thumbnail)
{
    d->thumbnail = thumbnail;
    update();
}

void PreviewWidget::paintEvent(QPaintEvent*)
{
    d->pix = QPixmap(width(), height());
    d->pix.fill(Qt::black);
    if (!d->text.isEmpty()) 
    {
        QPainter p(&d->pix);

        if (!d->thumbnail.isNull())
        {
            p.drawPixmap(d->pix.width()/2-d->thumbnail.width()/2, d->pix.height()/4-d->thumbnail.height()/2, 
                         d->thumbnail, 0, 0, d->thumbnail.width(), d->thumbnail.height());
            p.setPen(QPen(Qt::white));
            p.drawRect(d->pix.width()/2-d->thumbnail.width()/2, d->pix.height()/4-d->thumbnail.height()/2,
                       d->thumbnail.width()-1, d->thumbnail.height()-1);
            p.setPen(QPen(d->textColor));
            p.drawText(0, d->pix.height()/2, d->pix.width(), d->pix.height()/2,
                       Qt::AlignCenter|Qt::TextWordWrap, d->text);
        }
        else
        {
            p.setPen(QPen(d->textColor));
            p.drawText(0, 0, d->pix.width(), d->pix.height(),
                       Qt::AlignCenter|Qt::TextWordWrap, d->text);
        }

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

void PreviewWidget::resizeEvent(QResizeEvent*)
{
    update();
}

} // namespace KIPIRawConverterPlugin
