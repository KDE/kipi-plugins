/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-09-09
 * Description : a pixmap to render KIPI svg logo.
 *
 * Copyright (C) 2015 by Alexander Potashev <aspotashev at gmail dot com>
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

#include "kpsvgpixmaprenderer.h"

// Qt includes

#include <QStandardPaths>
#include <QPainter>
#include <QSvgRenderer>

namespace KIPIAdvancedSlideshowPlugin
{

KPSvgPixmapRenderer::KPSvgPixmapRenderer(int width, int height, const QString& filename)
    : m_width(width),
      m_height(height),
      m_filename(filename)
{
    if (m_filename.isEmpty())
    {
        m_filename = QString::fromLatin1(":/icons/kipi-icon.svg");
    }
}

QPixmap KPSvgPixmapRenderer::getPixmap() const
{
    QPixmap pixmap = QPixmap(m_width, m_height);
    pixmap.fill(Qt::transparent);

    QSvgRenderer svgRenderer(m_filename);
    QPainter painter(&pixmap);
    svgRenderer.render(&painter);

    return pixmap;
}

} // namespace KIPIAdvancedSlideshowPlugin
