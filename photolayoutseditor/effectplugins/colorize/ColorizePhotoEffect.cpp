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

#include "ColorizePhotoEffect.h"
#include "ColorizePhotoEffect_p.h"

#include <kpluginfactory.h>
#include <klocalizedstring.h>

K_PLUGIN_FACTORY( ColorizePhotoEffectFactoryLoader, registerPlugin<ColorizePhotoEffectFactory>(); )
K_EXPORT_PLUGIN ( ColorizePhotoEffectFactoryLoader("photolayoutseditoreffectplugin_colorize") )

ColorizePhotoEffectFactory::ColorizePhotoEffectFactory(QObject * parent, const QVariantList&) :
    AbstractPhotoEffectFactory(parent)
{
}

AbstractPhotoEffectInterface * ColorizePhotoEffectFactory::getEffectInstance()
{
    return new ColorizePhotoEffect(this, this);
}

QString ColorizePhotoEffectFactory::effectName() const
{
    return i18n("Colorize effect");
}


void ColorizePhotoEffectFactory::writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement)
{
    ColorizePhotoEffect * colorizeEffect = dynamic_cast<ColorizePhotoEffect*>(effect);
    if (colorizeEffect)
        effectElement.setAttribute(COLOR_PROPERTY, colorizeEffect->color().name());
}

AbstractPhotoEffectInterface * ColorizePhotoEffectFactory::readFromSvg(QDomElement & element)
{
    ColorizePhotoEffect * effect = (ColorizePhotoEffect*) this->getEffectInstance();
    effect->setColor( QColor(element.attribute(COLOR_PROPERTY)) );
    return effect;
}

