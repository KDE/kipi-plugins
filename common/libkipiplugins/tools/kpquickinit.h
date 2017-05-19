/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageInfo to manage easily
 *               item properties with KIPI host application.
 *
 * Copyright (C) 2017 by Artem Serebriyskiy <v.for.vandal@gmail.com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KPQUICK_INIT_H
#define KPQUICK_INIT_H

// Qt includes

#include <QQmlEngine>

// Libkipi includes
#include <KIPI/Interface>

// Local includes

#include "kipiplugins_export.h"


namespace KIPIPlugins
{
    /** Registers all QtQuick wrappers form kipiplugins
     */
    void KIPIPLUGINS_EXPORT InitKIPIQuick();
    void KIPIPLUGINS_EXPORT InitKIPIQmlEngine(QQmlEngine& engine, KIPI::Interface* interface);

}

#endif

