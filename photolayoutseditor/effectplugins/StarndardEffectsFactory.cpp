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

#include "StarndardEffectsFactory.h"

#include "BlurPhotoEffect.h"
#include "ColorizePhotoEffect.h"
#include "GrayscalePhotoEffect.h"
#include "SepiaPhotoEffect.h"

using namespace KIPIPhotoLayoutsEditor;

StarndardEffectsFactory::StarndardEffectsFactory(QObject* parent) :
    AbstractPhotoEffectFactory(parent)
{}

AbstractPhotoEffectInterface * StarndardEffectsFactory::getEffectInstance(const QString& name)
{
    if (name == i18n("Blur effect"))
        return new BlurPhotoEffect(this);
    if (name == i18n("Colorize effect"))
        return new ColorizePhotoEffect(this);
    if (name == i18n("Grayscale effect"))
        return new GrayscalePhotoEffect(this);
    if (name == i18n("Sepia effect"))
        return new SepiaPhotoEffect(this);
    return 0;
}

QString StarndardEffectsFactory::effectName() const
{
    return i18n("Blur effect") + QString(";") +
           i18n("Colorize effect") + QString(";") +
           i18n("Grayscale effect") + QString(";") +
           i18n("Sepia effect");
}
