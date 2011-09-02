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

#ifndef SOLIDBORDERDRAWER_H
#define SOLIDBORDERDRAWER_H

#include "SolidBorderDrawer_global.h"
#include "BorderDrawerFactoryInterface.h"

#include <QVariantList>

using namespace KIPIPhotoLayoutsEditor;

class SOLIDBORDERDRAWERSHARED_EXPORT SolidBorderDrawerFactory : public BorderDrawerFactoryInterface
{
        Q_OBJECT
        Q_INTERFACES(KIPIPhotoLayoutsEditor::BorderDrawerFactoryInterface)

    public:

        SolidBorderDrawerFactory(QObject * parent, const QVariantList&);

        virtual QString drawerName() const;

        virtual BorderDrawerInterface * getDrawerInstance(QObject * parent = 0);
};

#endif // SOLIDBORDERDRAWER_H
