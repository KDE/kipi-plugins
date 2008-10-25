/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

#include "imageviewer.h"
#include "imageviewer.moc"

// Qt includes.

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QSvgRenderer>
#include <QSize>

// KDE includes.

#include <kstandarddirs.h>

namespace KIPISlideShowPlugin
{

ImageViewer::ImageViewer(QWidget* parent)
        : QWidget(parent)
{
    m_svgRenderer = new QSvgRenderer( KStandardDirs::locate("data", "kipiplugin_slideshow/KIPIicon.svg") );
    m_clear = true;
    clear();
}

ImageViewer::~ImageViewer()
{
//    delete m_svgRenderer;
}

QImage ImageViewer::image() const
{
    return m_image;
}

void ImageViewer::setImage(const QImage &image)
{
    m_clear = false;
    m_image = image;
    updateViewer();
    update();
}

void ImageViewer::clear()
{
    m_clear = true;
    m_image = QImage(0, 0);
    updateViewer();
    update();
}

void ImageViewer::updateViewer()
{
    if ( m_image.isNull() )
    {
        m_clear = true;
        update();
    }
    else
        m_pixmap = QPixmap::fromImage(m_image.scaled(size(), Qt::KeepAspectRatio));
}

void ImageViewer::resizeEvent(QResizeEvent *)
{
    updateViewer();
}

void ImageViewer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (m_clear)
    {
        QSize finalSize = m_svgRenderer->defaultSize();
        finalSize.scale(width(), height(), Qt::KeepAspectRatio);
        m_svgRenderer->render(&painter,
                              QRectF(qreal((width() - m_svgRenderer->defaultSize().width()) / 2),
                                     qreal((height() - m_svgRenderer->defaultSize().height()) / 2),
                                     qreal(finalSize.width()),
                                     qreal(finalSize.height())
                                    ));
    }
    else
        painter.drawPixmap( QPoint( (int) ((width() - m_pixmap.width()) / 2),
                                    (int) ((height() - m_pixmap.height()) / 2)), m_pixmap );
}

} // namespace KIPISlideShowPlugin

