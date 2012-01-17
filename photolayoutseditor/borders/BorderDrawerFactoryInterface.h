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

#ifndef BORDERDRAWERFACTORYINTERFACE_H
#define BORDERDRAWERFACTORYINTERFACE_H

#include <QObject>

namespace KIPIPhotoLayoutsEditor
{
    class BorderDrawerInterface;
    class BorderDrawerFactoryInterface : public QObject
    {
        public:

            explicit BorderDrawerFactoryInterface(QObject* parent = 0) :
                QObject(parent)
            {}

            virtual ~BorderDrawerFactoryInterface()
            {}

            virtual QString drawersNames() const = 0;
            virtual BorderDrawerInterface* getDrawerInstance(const QString& name) = 0;
    };
}

Q_DECLARE_INTERFACE(KIPIPhotoLayoutsEditor::BorderDrawerFactoryInterface, "pl.coder89.ple.BorderDrawerFactoryInterface/1.0")

#endif // BORDERDRAWERFACTORYINTERFACE_H
