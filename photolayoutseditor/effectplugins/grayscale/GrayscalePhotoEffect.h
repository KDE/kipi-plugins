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

#ifndef GRAYSCALEPHOTOEFFECT_H
#define GRAYSCALEPHOTOEFFECT_H

#include "GrayscalePhotoEffect_global.h"
#include "AbstractPhotoEffectFactory.h"

using namespace KIPIPhotoLayoutsEditor;

class GRAYSCALEPHOTOEFFECTSHARED_EXPORT GrayscalePhotoEffectFactory : public AbstractPhotoEffectFactory
{
        Q_OBJECT
        Q_INTERFACES(KIPIPhotoLayoutsEditor::AbstractPhotoEffectFactory)

    public:

        GrayscalePhotoEffectFactory(QObject * parent, const QVariantList&);
        virtual AbstractPhotoEffectInterface * getEffectInstance();
        virtual QString effectName() const;

    protected:

        virtual void writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement);
        virtual AbstractPhotoEffectInterface * readFromSvg(QDomElement & element);
};

#endif // GRAYSCALEPHOTOEFFECT_H
