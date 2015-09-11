/*
 * This file is a part of kipi-plugins project
 * Copyright 2015  Alexander Potashev <aspotashev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kpsvgpixmaprenderer.h"

#include <QStandardPaths>
#include <QPainter>
#include <QSvgRenderer>

KPSvgPixmapRenderer::KPSvgPixmapRenderer(int width, int height, const QString& filename)
    : m_width(width)
    , m_height(height)
    , m_filename(filename)
{
    if (m_filename.isEmpty())
    {
        m_filename = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                            QStringLiteral("kf5/kipi/pics/kipi-icon.svg"));
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
