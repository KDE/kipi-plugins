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

#include "templateicon.h"

// KDE includes

#include <kdebug.h>

//define next to get debug output
#undef DEBUG_OUTPUT

namespace KIPIPrintImagesPlugin
{

TemplateIcon::TemplateIcon(int height, const QSize &template_size)
    : m_paper_size(template_size)
{
    m_icon_margin        = 2;
    m_icon_size          = QSize( height-2*m_icon_margin, height-2*m_icon_margin );

    // remark: m_icon_size is the real size of the icon, in the combo-box there is no space
    // between the icons, therefore the variable m_icon_margin
    //FIX static analysis (UNINIT_CTOR)
    m_icon_size.rwidth() = (int)(float(m_icon_size.height()) * float(m_paper_size.width()) / float(m_paper_size.height()));
    scale_width          = float(m_icon_size.width())                                      / float(m_paper_size.width());
    scale_height         = float(m_icon_size.height())                                     / float(m_paper_size.height());
    rotate               = false;

    pixmap               = NULL;
    painter              = NULL;
    icon                 = NULL;
}

TemplateIcon::~TemplateIcon()
{
    delete pixmap;
    delete painter;
    delete icon;
}

void TemplateIcon::begin()
 {
    // compute scaling values
    m_icon_size.rwidth() = (int)(float(m_icon_size.height()) * float(m_paper_size.width()) / float(m_paper_size.height()));
    scale_width          = float(m_icon_size.width())                                      / float(m_paper_size.width());
    scale_height         = float(m_icon_size.height())                                     / float(m_paper_size.height());

#ifdef DEBUG_OUTPUT
    kDebug() << "begin: m_paper_size.width =" <<  m_paper_size.width();
    kDebug() << "begin: m_paper_size.height=" <<  m_paper_size.height();
    kDebug() << "begin: m_icon_size.width  =" <<  m_icon_size.width();
    kDebug() << "begin: m_icon_size.height =" <<  m_icon_size.height();
    kDebug() << "begin: scale_width      =" <<  scale_width;
    kDebug() << "begin: scale_height     =" <<  scale_height;
#endif

    // icon back ground
    pixmap  = new QPixmap( m_icon_size );
    pixmap->fill( Qt::color0);

    painter = new QPainter();
    painter->begin( pixmap );

    painter->setPen( Qt::color1 );
    painter->drawRect(pixmap->rect());
}

void TemplateIcon::fillRect( int x, int y, int w, int h, const QColor& color )
{
#ifdef DEBUG_OUTPUT
    kDebug() << "fillRect: x1=" << x << " => " << x     * scale_width;
    kDebug() << "fillRect: y1=" << y << " => " << y     * scale_height;
    kDebug() << "fillRect: x2=" << w << " => " << (x + w) * scale_width;
    kDebug() << "fillRect: y2=" << h << " => " << (y + h) * scale_height;
#endif

    painter->fillRect( (int)(m_icon_margin + x * scale_width),
                       (int)(m_icon_margin + y * scale_height),
                       (int)(w * scale_width),
                       (int)(h * scale_height),
                       color );
}

void TemplateIcon::end()
{
    // paint boundary of template
    painter->setPen( Qt::color1 );

    painter->drawRect( m_icon_margin,
                       m_icon_margin,
                       (int)(m_paper_size.width()  * scale_width),
                       (int)(m_paper_size.height() * scale_height) );

    painter->end();
    icon = new QIcon( *pixmap );
}

QIcon& TemplateIcon::getIcon() const
{
    return *icon;
}

QSize& TemplateIcon::getSize()
{
    return m_icon_size;
}

QPainter& TemplateIcon::getPainter() const
{
    return *painter;
}
} // namespace KIPIPrintImagesPlugin
