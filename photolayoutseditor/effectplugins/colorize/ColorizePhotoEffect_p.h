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

#ifndef COLORIZEPHOTOEFFECT_P_H
#define COLORIZEPHOTOEFFECT_P_H

#include "ColorizePhotoEffect_global.h"
#include "AbstractPhotoEffectInterface.h"

using namespace KIPIPhotoLayoutsEditor;

#define COLOR_PROPERTY "Color"

class ColorizePhotoEffectFactory;
class ColorizePhotoEffect : public AbstractPhotoEffectInterface
{
        Q_INTERFACES(KIPIPhotoLayoutsEditor::AbstractPhotoEffectInterface)

        static QColor m_last_color;

    public:

        explicit ColorizePhotoEffect(ColorizePhotoEffectFactory * factory, QObject * parent = 0);
        virtual QImage apply(const QImage & image) const;
        virtual QString effectName() const;
        virtual QString toString() const;
        virtual operator QString() const;
        QColor color() const
        {
            foreach (AbstractPhotoEffectProperty * property, m_properties)
                if (property->id == COLOR_PROPERTY)
                    return (m_last_color = property->value.value<QColor>());
            return Qt::transparent;
        }

    private:

        void setColor(const QColor & color)
        {
            foreach (AbstractPhotoEffectProperty * property, m_properties)
                if (property->id == COLOR_PROPERTY)
                    property->value = color;
        }

        static inline QImage colorized(const QImage & image, const QColor & color)
        {
            QImage result = image;
            unsigned int pixels = result.width() * result.height();
            unsigned int * data = (unsigned int *) result.bits();
            for (unsigned int i = 0; i < pixels; ++i)
            {
                int val = qGray(data[i]);
                data[i] = qRgb(val,val,val);
            }
            QPainter p(&result);
            p.setCompositionMode(QPainter::CompositionMode_Overlay);
            p.fillRect(result.rect(),color);
            p.end();
            return result;
        }

    friend class ColorizePhotoEffectFactory;
};

#endif // COLORIZEPHOTOEFFECT_P_H
