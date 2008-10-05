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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

// QT includes
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QtSvg>


namespace KIPISlideShowPlugin
{

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

    QImage image() const;
    void setImage(const QImage &image);
    void clear() ;

protected:
    void updateViewer();

    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

private:
    QImage          m_image;
    QPixmap         m_pixmap;
    QSvgRenderer*   m_svgRenderer;
    bool            m_clear;
};

} // NameSpace KIPISlideShowPlugin
#endif
