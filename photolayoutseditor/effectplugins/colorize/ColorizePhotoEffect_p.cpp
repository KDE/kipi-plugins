/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "ColorizePhotoEffect_p.h"
#include "ColorizePhotoEffect.h"

#include <klocalizedstring.h>

QColor ColorizePhotoEffect::m_last_color = QColor(255,255,255,0);

ColorizePhotoEffect::ColorizePhotoEffect(ColorizePhotoEffectFactory * factory, QObject * parent) :
    AbstractPhotoEffectInterface(factory, parent)
{
    AbstractPhotoEffectProperty  * color = new AbstractPhotoEffectProperty(COLOR_PROPERTY);
    color->value = m_last_color;
    m_properties.push_back(color);
}

QImage ColorizePhotoEffect::apply(const QImage & image) const
{
    QColor tempColor = color();
    if (!strength() || !tempColor.alpha())
        return image;
    QImage result = image;
    QPainter p(&result);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawImage(0,0,AbstractPhotoEffectInterface::apply(colorized(image, tempColor)));
    return result;
}

QString ColorizePhotoEffect::effectName() const
{
    return factory()->effectName();
}

QString ColorizePhotoEffect::toString() const
{
    return factory()->effectName() + " [" + color().name() + "]";
}

ColorizePhotoEffect::operator QString() const
{
    return toString();
}
