/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-22
 * Description : preview raw file widget used in single convert
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QPainter>
#include <QImage>
#include <QString>
#include <QEvent>
#include <QPaintEvent>
#include <QTimer>
#include <QFile>

// KDE includes.

#include <klocale.h>

// Local includes.

#include "previewwidget.h"
#include "previewwidget.moc"

namespace KIPIRawConverterPlugin
{

class PreviewWidgetPriv
{
public:

    PreviewWidgetPriv(){}

    QPixmap  pix;
    QPixmap  preview;

    QString  text;

    QImage   image;
};

PreviewWidget::PreviewWidget(QWidget *parent)
             : QWidget(parent)
{
    d = new PreviewWidgetPriv;
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
        QImage img = d->image.scaled(width(), height(), Qt::KeepAspectRatio);
        int x = d->pix.width()/2  - img.width()/2;
        int y = d->pix.height()/2 - img.height()/2;

        QPainter p(&d->pix);
        p.drawImage(x, y, img);
        p.setPen(QPen(Qt::white));
        p.drawRect(x,y,img.width(),img.height());
        p.end();
    }
    else 
    {
        setInfo(i18n( "Failed to load image after processing" ));
        return;
    }

    update();
}

void PreviewWidget::setInfo(const QString& text, const QColor& color, const QPixmap& preview)
{
    d->text    = text;
    d->preview = preview;
    d->pix     = QPixmap(width(), height());
    d->pix.fill(Qt::black);
    QPainter p(&d->pix);
    p.setPen(QPen(color));

    if (!d->preview.isNull())
    {
        p.drawPixmap(d->pix.width()/2-d->preview.width()/2, d->pix.height()/4-d->preview.height()/2, 
                     d->preview, 0, 0, d->preview.width(), d->preview.height());
        p.drawText(0, d->pix.height()/2, d->pix.width(), d->pix.height()/2,
                   Qt::AlignCenter|Qt::TextWordWrap, d->text);
    }
    else
    {
        p.drawText(0, 0, d->pix.width(), d->pix.height(),
                   Qt::AlignCenter|Qt::TextWordWrap, d->text);
    }
    p.end();
    update();
}

void PreviewWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.drawPixmap(0, 0, d->pix);
    p.end();
}

void PreviewWidget::resizeEvent(QResizeEvent*)
{
    d->pix = QPixmap(width(), height());
    d->pix.fill(Qt::black);
    if (!d->text.isEmpty()) 
    {
        QPainter p(&d->pix);
        p.setPen(QPen(Qt::white));

        if (!d->preview.isNull())
        {
            p.drawPixmap(d->pix.width()/2-d->preview.width()/2, d->pix.height()/4-d->preview.height()/2, 
                        d->preview, 0, 0, d->preview.width(), d->preview.height());
            p.drawText(0, d->pix.height()/2, d->pix.width(), d->pix.height()/2,
                    Qt::AlignCenter|Qt::TextWordWrap, d->text);
        }
        else
        {
            p.drawText(0, 0, d->pix.width(), d->pix.height(),
                    Qt::AlignCenter|Qt::TextWordWrap, d->text);
        }

        p.end();
    }
    else 
    {
        if (!d->image.isNull()) 
        {
            QImage img = d->image.scaled(width(),height(), Qt::KeepAspectRatio);
            int x = d->pix.width()/2  - img.width()/2;
            int y = d->pix.height()/2 - img.height()/2;

            QPainter p(&d->pix);
            p.drawImage(x, y, img);
            p.setPen(QPen(Qt::white));
            p.drawRect(x, y, img.width(), img.height());
            p.end();
        }
    }
}

} // NameSpace KIPIRawConverterPlugin
