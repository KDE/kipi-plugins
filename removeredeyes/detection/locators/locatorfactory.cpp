/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-01-18
 * Description : factory to create locator objects
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "locatorfactory.h"

// Qt includes

#include <QString>

// Local includes

#include "locator.h"
#include "haarclassifierlocator.h"

namespace KIPIRemoveRedEyesPlugin
{

Locator* LocatorFactory::create(const QString& type)
{
    if (type.isEmpty())
    {
        return 0;
    }

    Locator* locator = 0;

    if (type == "HaarClassifierLocator")
    {
        locator = new HaarClassifierLocator;
    }

    return locator;
}


} // namespace KIPIRemoveRedEyesPlugin
