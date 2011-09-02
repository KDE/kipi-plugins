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

#include "GrayscalePhotoEffect.h"
#include "GrayscalePhotoEffect_p.h"

#include <kpluginfactory.h>
#include <klocalizedstring.h>

K_PLUGIN_FACTORY( GrayscalePhotoEffectFactoryLoader, registerPlugin<GrayscalePhotoEffectFactory>(); )
K_EXPORT_PLUGIN ( GrayscalePhotoEffectFactoryLoader("photolayoutseditoreffectplugin_grayscale") )

GrayscalePhotoEffectFactory::GrayscalePhotoEffectFactory(QObject * parent, const QVariantList&) :
    AbstractPhotoEffectFactory(parent)
{
}

AbstractPhotoEffectInterface * GrayscalePhotoEffectFactory::getEffectInstance()
{
    return new GrayscalePhotoEffect(this, this);
}

QString GrayscalePhotoEffectFactory::effectName() const
{
    return i18n("Grayscale effect");
}

void GrayscalePhotoEffectFactory::writeToSvg(AbstractPhotoEffectInterface * /*effect*/, QDomElement & /*effectElement*/)
{}

AbstractPhotoEffectInterface * GrayscalePhotoEffectFactory::readFromSvg(QDomElement & /*element*/)
{
    GrayscalePhotoEffect * effect = (GrayscalePhotoEffect*) this->getEffectInstance();
    return effect;
}
