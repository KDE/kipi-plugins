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

#ifndef GRAYSCALEPHOTOEFFECT_P_H
#define GRAYSCALEPHOTOEFFECT_P_H

#include "GrayscalePhotoEffect_global.h"
#include "AbstractPhotoEffectInterface.h"

using namespace KIPIPhotoLayoutsEditor;

class GrayscalePhotoEffectFactory;
class GrayscalePhotoEffect : public AbstractPhotoEffectInterface
{
        Q_INTERFACES(KIPIPhotoLayoutsEditor::AbstractPhotoEffectInterface)

    public:

        explicit GrayscalePhotoEffect(GrayscalePhotoEffectFactory * factory, QObject * parent = 0);
        virtual QImage apply(const QImage & image) const;
        virtual QString effectName() const;
        virtual QString toString() const;
        virtual operator QString() const;

    private:
        static inline QImage greyscaled(const QImage & image)
        {
            QImage result = image;
            unsigned int pixels = result.width() * result.height();
            unsigned int * data = (unsigned int *) result.bits();
            for (unsigned int i = 0; i < pixels; ++i)
            {
                int val = qGray(data[i]);
                data[i] = qRgb(val,val,val);
            }
            return result;
        }
};

#endif // GRAYSCALEPHOTOEFFECT_P_H
