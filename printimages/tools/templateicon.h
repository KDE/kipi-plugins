/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-06-26
 * Description : a kipi plugin to print images
 *
 * Copyright 2008 by Andreas Trink <atrink at nociaro dot org>
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

#ifndef TEMPLATEICON_H
#define TEMPLATEICON_H

// QT incudes

#include <QPrinter>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QColor>
#include <QSize>

namespace KIPIPrintImagesPlugin
{

class TemplateIcon
{

public:

    /**
        Constructor: The height of the icon is <icon_height>. The width is computed
        during invocation of method 'begin()' according to the paper-size.
    */
    TemplateIcon(int icon_height, const QSize& template_size);

    /**
        Destructor:
    */
    ~TemplateIcon();

    /**
        Begin painting the icon
    */
    void begin();

    /**
        End painting the icon
    */
    void end();

    /**
        Returns a pointer to the icon.
    */
    QIcon& getIcon() const;

    /**
        Returns the size of the icon.
    */
    QSize& getSize();

    /**
        Returns the painter.
    */
    QPainter& getPainter() const;

    /**
        Draw a filled rectangle with color <color> at position <x>/<y> (relative
        to template-origin) and width <w> and height <h>.
    */
    void fillRect( int x, int y, int w, int h, const QColor& color );

private:

    QSize     m_paper_size;
    QSize     m_icon_size;
    int       m_icon_margin;

    float     scale_width;
    float     scale_height;
    bool      rotate;

    QPixmap*  pixmap;
    QPainter* painter;
    QIcon*    icon;
};

} // namespace KIPIPrintImagesPlugin

#endif /* TEMPLATEICON_H */
