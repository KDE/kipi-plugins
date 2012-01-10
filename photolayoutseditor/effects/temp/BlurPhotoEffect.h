/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef BLURPHOTOEFFECT_H
#define BLURPHOTOEFFECT_H

#include "PhotoEffectsLoader.h"

#include <QImage>
#include <QPainter>
#include <QtIntPropertyManager>

namespace KIPIPhotoFramesEditor
{
    class BlurPhotoEffect : public PhotoEffectsLoader
    {
            Q_OBJECT

            int m_radius;

            class BlurUndoCommand;

        public:

            explicit BlurPhotoEffect(int radius, QObject * parent = 0);
            virtual QImage apply(const QImage & image);
            virtual QtAbstractPropertyBrowser * propertyBrowser() const;
            virtual QString toString() const;

          /**
            * Radius property
            */
            Q_PROPERTY(int m_radius READ radius WRITE setRadius)
            void setRadius(int radius)
            {
                m_radius = radius;
                emit effectChanged(this);
            }
            int radius() const
            {
                return m_radius;
            }

          /**
            * Blur effect identifier (name).
            */
            virtual QString effectName() const;

        protected:

            static const QString RADIUS_STRING;

        protected slots:

            virtual void propertyChanged(QtProperty * property);

        private:

            static QImage blurred(const QImage & image, const QRect& rect, int radius)
            {
                int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
                int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

                QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
                int r1 = rect.top();
                int r2 = rect.bottom();
                int c1 = rect.left();
                int c2 = rect.right();

                int bpl = result.bytesPerLine();
                int rgba[4];
                unsigned char* p;

                for (int col = c1; col <= c2; col++)
                {
                    p = result.scanLine(r1) + col * 4;
                    for (int i = 0; i < 4; i++)
                        rgba[i] = p[i] << 4;

                    p += bpl;
                    for (int j = r1; j < r2; j++, p += bpl)
                        for (int i = 0; i < 4; i++)
                            p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
                }

                for (int row = r1; row <= r2; row++)
                {
                    p = result.scanLine(row) + c1 * 4;
                    for (int i = 0; i < 4; i++)
                        rgba[i] = p[i] << 4;

                    p += 4;
                    for (int j = c1; j < c2; j++, p += 4)
                        for (int i = 0; i < 4; i++)
                            p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
                }

                for (int col = c1; col <= c2; col++)
                {
                    p = result.scanLine(r2) + col * 4;
                    for (int i = 0; i < 4; i++)
                        rgba[i] = p[i] << 4;

                    p -= bpl;
                    for (int j = r1; j < r2; j++, p -= bpl)
                        for (int i = 0; i < 4; i++)
                            p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
                }

                for (int row = r1; row <= r2; row++)
                {
                    p = result.scanLine(row) + c2 * 4;
                    for (int i = 0; i < 4; i++)
                        rgba[i] = p[i] << 4;

                    p -= 4;
                    for (int j = c1; j < c2; j++, p -= 4)
                        for (int i = 0; i < 4; i++)
                            p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
                }

                return result;
            }
    };
}

#endif // BLURPHOTOEFFECT_H
