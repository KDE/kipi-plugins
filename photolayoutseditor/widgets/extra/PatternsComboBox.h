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

#ifndef PATTERNSCOMBOBOX_H
#define PATTERNSCOMBOBOX_H

#include <QComboBox>

namespace KIPIPhotoLayoutsEditor
{
    class PatternsComboBox : public QComboBox
    {
            Q_OBJECT

        public:

            explicit PatternsComboBox(QWidget * parent = 0);
            Qt::BrushStyle pattern() const;
            void setPattern(Qt::BrushStyle pattern);

        signals:

            void currentPatternChanged(Qt::BrushStyle pattern);

        protected:

            virtual void paintEvent(QPaintEvent *e);

        protected slots:

            void emitPatternChanged(int index);

    };
}

#endif // PATTERNSCOMBOBOX_H
