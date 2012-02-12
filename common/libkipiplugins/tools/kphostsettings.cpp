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

#include "kphostsettings.h"

// Qt includes

#include <QVariant>

// KDE includes

#include <kdebug.h>

// Libkipi includes

#include <libkipi/interface.h>

namespace KIPIPlugins
{

class KPHostSettings::KPHostSettingsPrivate
{
public:

    KPHostSettingsPrivate()
    {
        iface = 0;
    }

    bool hasValidData() const
    {
        return (iface);
    }

    QVariant settings(const QString& name) const
    {
        QVariant v;
        if (hasValidData())
        {
            v = iface->hostSetting(name);
        }
        return v;
    }

    bool hasSetting(const QString& name) const
    {
        if (hasValidData())
        {
            return (!settings(name).isNull());
        }
        return false;
    }

public:

    KIPI::Interface* iface;
};

KPHostSettings::KPHostSettings(KIPI::Interface* iface)
    : d(new KPHostSettingsPrivate)
{
    d->iface = iface;
}

KPHostSettings::~KPHostSettings()
{
    delete d;
}


}  // namespace KIPIPlugins
