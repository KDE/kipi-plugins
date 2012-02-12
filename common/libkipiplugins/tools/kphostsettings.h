/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : help wrapper around libkipi Interface to manage easily
 *               KIPI host application settings.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPHOSTSETTINGS_H
#define KPHOSTSETTINGS_H

// Qt includes

#include <QString>

// LibKexiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPI
{
    class Interface;
}

using namespace KExiv2Iface;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPHostSettings
{

public:

    /** Contructor with KIPI interface instance get from plugin.
     */
    KPHostSettings(KIPI::Interface* iface);
    ~KPHostSettings();


private:

    class KPHostSettingsPrivate;
    KPHostSettingsPrivate* const d;
};

} // namespace KIPIPlugins

#endif  // KPHOSTSETTINGS_H
