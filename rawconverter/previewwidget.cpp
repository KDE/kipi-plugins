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

// KDE includes.

#include <klocale.h>

// Local includes.

#include "previewwidget.h"

namespace KIPIRawConverterPlugin
{

PreviewWidget::PreviewWidget(QWidget *parent)
             : QWidget(parent, 0, Qt::WRepaintNoErase)
{
    setMinimumSize(QSize(484, 364));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    pix_ = new QPixmap(484,364);
    pix_->fill(Qt::black);

    timer_ = new QTimer(this);

    connect(timer_, SIGNAL(timeout()),
            this, SLOT(slotResize()));
}

PreviewWidget::~PreviewWidget()
{
    delete pix_;
}

void PreviewWidget::load(const QString& file)
{
    text_ = "";
    pix_->fill(Qt::black);
    image_.load(file);

    if (!image_.isNull()) 
    {
        QImage img = image_.scale(width(),height(),QImage::ScaleMin);
        int x = pix_->width()/2 - img.width()/2;
        int y = pix_->height()/2 - img.height()/2;

        QPainter p(pix_);
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

void PreviewWidget::setText(const QString& text)
{
    text_ = text;
    pix_->fill(Qt::black);
    QPainter p(pix_);
    p.setPen(QPen(Qt::white));
    p.drawText(0, 0, pix_->width(), pix_->height(),
               Qt::AlignCenter|Qt::WordBreak, text);
    p.end();
    update();
}

void PreviewWidget::paintEvent(QPaintEvent *e)
{
    QRect r(e->rect());
    bitBlt(this, r.topLeft(), pix_, r, Qt::CopyROP);
}

void PreviewWidget::resizeEvent(QResizeEvent*)
{
    timer_->start(10,true);
}

void PreviewWidget::slotResize()
{
    if (timer_->isActive()) return;

    pix_->resize(width(),height());
    pix_->fill(Qt::black);
    if (!text_.isEmpty()) 
    {
        QPainter p(pix_);
        p.setPen(QPen(Qt::white));
        p.drawText(0, 0, pix_->width(), pix_->height(),
                   Qt::AlignCenter|Qt::WordBreak, text_);
        p.end();
    }
    else 
    {
        if (!image_.isNull()) 
        {
            QImage img = image_.scale(width(),height(),QImage::ScaleMin);
            int x = pix_->width()/2 - img.width()/2;
            int y = pix_->height()/2 - img.height()/2;

            QPainter p(pix_);
            p.drawImage(x, y, img);
            p.setPen(QPen(Qt::white));
            p.drawRect(x,y,img.width(),img.height());
            p.end();
        }
    }

    update();
}

} // NameSpace KIPIRawConverterPlugin

#include "previewwidget.moc"
