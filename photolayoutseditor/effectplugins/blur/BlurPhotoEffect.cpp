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

<<<<<<< HEAD
<<<<<<< HEAD
#include "BlurPhotoEffect.h"
#include "StarndardEffectsFactory.h"
=======
=======
>>>>>>> a0bb2ddc52164d737c5d7ba1aa49cd3a08a36cb9
#include "BlurPhotoEffect.moc"
#include "BlurPhotoEffect_p.h"
>>>>>>> a0bb2ddc52164d737c5d7ba1aa49cd3a08a36cb9

#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

BlurPhotoEffect::BlurPhotoEffect(StarndardEffectsFactory * factory, QObject * parent) :
    AbstractPhotoEffectInterface(factory, parent),
    m_radius(10)
{
}

QImage BlurPhotoEffect::apply(const QImage & image) const
{
    int tempRadius = radius();
    if (!tempRadius)
        return image;
    QImage result = image;
    QPainter p(&result);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawImage(0,0,AbstractPhotoEffectInterface::apply(blurred(image, image.rect(), tempRadius)));
    return result;
}

QString BlurPhotoEffect::toString() const
{
    return i18n("Blur effect") + " [" + QString::number(this->radius()) + "]";
}

BlurPhotoEffect::operator QString() const
{
    return toString();
}
