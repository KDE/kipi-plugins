/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-10-22
 * Description : preview raw file widget used in single convert
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
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

// Qt includes.

#include <qpainter.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qevent.h>
#include <qtimer.h>
#include <qimage.h>

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

    PreviewWidgetPriv()
    {
        pix   = 0;
        timer = 0;
    }

    QPixmap *pix;

    QTimer  *timer;

    QString  text;

    QImage   image;
};

PreviewWidget::PreviewWidget(QWidget *parent)
             : QWidget(parent, 0, Qt::WRepaintNoErase)
{
    d = new PreviewWidgetPriv;
    setMinimumSize(QSize(484, 364));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->pix = new QPixmap(484,364);
    d->pix->fill(Qt::black);

    d->timer = new QTimer(this);

    connect(d->timer, SIGNAL(timeout()),
            this, SLOT(slotResize()));
}

PreviewWidget::~PreviewWidget()
{
    delete d;
}

void PreviewWidget::load(const QString& file)
{
    d->text = "";
    d->pix->fill(Qt::black);
    d->image.load(file);

    if (!d->image.isNull()) 
    {
        QImage img = d->image.scale(width(),height(),QImage::ScaleMin);
        int x = d->pix->width()/2 - img.width()/2;
        int y = d->pix->height()/2 - img.height()/2;

        QPainter p(d->pix);
        p.drawImage(x, y, img);
        p.setPen(QPen(Qt::white));
        p.drawRect(x,y,img.width(),img.height());
        p.end();
    }
    else 
    {
        setText(i18n( "Failed to load image after processing" ));
        return;
    }

    update();
}

void PreviewWidget::setText(const QString& text, const QColor& color)
{
    d->text = text;
    d->pix->fill(Qt::black);
    QPainter p(d->pix);
    p.setPen(QPen(color));
    p.drawText(0, 0, d->pix->width(), d->pix->height(),
               Qt::AlignCenter|Qt::WordBreak, text);
    p.end();
    update();
}

void PreviewWidget::paintEvent(QPaintEvent *e)
{
    QRect r(e->rect());
    bitBlt(this, r.topLeft(), d->pix, r, Qt::CopyROP);
}

void PreviewWidget::resizeEvent(QResizeEvent*)
{
    d->timer->start(10,true);
}

void PreviewWidget::slotResize()
{
    if (d->timer->isActive()) return;

    d->pix->resize(width(),height());
    d->pix->fill(Qt::black);
    if (!d->text.isEmpty()) 
    {
        QPainter p(d->pix);
        p.setPen(QPen(Qt::white));
        p.drawText(0, 0, d->pix->width(), d->pix->height(),
                   Qt::AlignCenter|Qt::WordBreak, d->text);
        p.end();
    }
    else 
    {
        if (!d->image.isNull()) 
        {
            QImage img = d->image.scale(width(),height(),QImage::ScaleMin);
            int x = d->pix->width()/2 - img.width()/2;
            int y = d->pix->height()/2 - img.height()/2;

            QPainter p(d->pix);
            p.drawImage(x, y, img);
            p.setPen(QPen(Qt::white));
            p.drawRect(x,y,img.width(),img.height());
            p.end();
        }
    }

    update();
}

} // NameSpace KIPIRawConverterPlugin

