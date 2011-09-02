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

#include "BlurPhotoEffect.moc"
#include "BlurPhotoEffect_p.h"

#include <kpluginfactory.h>
#include <klocalizedstring.h>

K_PLUGIN_FACTORY( BlurPhotoEffectFactoryLoader, registerPlugin<BlurPhotoEffectFactory>(); )
K_EXPORT_PLUGIN ( BlurPhotoEffectFactoryLoader("photolayoutseditoreffectplugin_blur") )

BlurPhotoEffectFactory::BlurPhotoEffectFactory(QObject * parent, const QVariantList&) :
    AbstractPhotoEffectFactory(parent)
{
}

AbstractPhotoEffectInterface * BlurPhotoEffectFactory::getEffectInstance()
{
    return new BlurPhotoEffect(this, this);
}

QString BlurPhotoEffectFactory::effectName() const
{
    return i18n("Blur effect");
}

void BlurPhotoEffectFactory::writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement)
{
    BlurPhotoEffect * blurEffect = dynamic_cast<BlurPhotoEffect*>(effect);
    if (blurEffect)
        effectElement.setAttribute(RADIUS_PROPERTY, blurEffect->radius());
}

AbstractPhotoEffectInterface * BlurPhotoEffectFactory::readFromSvg(QDomElement & element)
{
    BlurPhotoEffect * effect = (BlurPhotoEffect*) this->getEffectInstance();
    effect->setRadius( element.attribute(RADIUS_PROPERTY).toInt() );
    return effect;
}

