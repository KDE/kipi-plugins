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

#ifndef KPSVGPIXMAPRENDERER_H
#define KPSVGPIXMAPRENDERER_H

// Qt includes

#include <QString>
#include <QPixmap>

namespace KIPIAdvancedSlideshowPlugin
{

class KPSvgPixmapRenderer
{
public:

    KPSvgPixmapRenderer(int width, int height, const QString& filename = QString());

    QPixmap getPixmap() const;

private:

    int     m_width;
    int     m_height;
    QString m_filename;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif // KPSVGPIXMAPRENDERER_H
