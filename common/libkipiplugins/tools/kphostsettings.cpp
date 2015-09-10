/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : help wrapper around libkipi Interface to manage easily
 *               KIPI host application settings.
 *
 * Copyright (C) 2012-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

using namespace KIPI;

namespace KIPIPlugins
{

class KPHostSettings::Private
{
public:

    Private()
        : iface(0)
    {
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

    bool hasValidData() const
    {
        return (iface);
    }

    QVariant setting(const QString& name) const
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
            return (!setting(name).isNull());
        }

        return false;
    }

public:

    Interface* iface;
};

KPHostSettings::KPHostSettings()
    : d(new Private)
{
}

KPHostSettings::~KPHostSettings()
{
    delete d;
}

QString KPHostSettings::fileExtensions() const
{
    return d->setting(QLatin1String("FileExtensions")).toString();
}

bool KPHostSettings::hasFileExtensions() const
{
    return d->hasSetting(QLatin1String("FileExtensions"));
}

QString KPHostSettings::imageExtensions() const
{
    return d->setting(QLatin1String("ImagesExtensions")).toString();
}

bool KPHostSettings::hasImageExtensions() const
{
    return d->hasSetting(QLatin1String("ImagesExtensions"));
}

QString KPHostSettings::rawExtensions() const
{
    return d->setting(QLatin1String("RawExtensions")).toString();
}

bool KPHostSettings::hasRawExtensions() const
{
    return d->hasSetting(QLatin1String("RawExtensions"));
}

QString KPHostSettings::videoExtensions() const
{
    return d->setting(QLatin1String("VideoExtensions")).toString();
}

bool KPHostSettings::hasVideoExtensions() const
{
    return d->hasSetting(QLatin1String("VideoExtensions"));
}

QString KPHostSettings::audioExtensions() const
{
    return d->setting(QLatin1String("AudioExtensions")).toString();
}

bool KPHostSettings::hasAudioExtensions() const
{
    return d->hasSetting(QLatin1String("AudioExtensions"));
}

KIPIPlugins::KPMetaSettings KPHostSettings::metadataSettings() const
{
    KPMetaSettings meta; // if no valid data, default setting is returned.

    if (d->hasValidData())
    {
        meta.writeRawFiles         = d->setting(QLatin1String("WriteMetadataToRAW")).toBool();
        meta.updateFileTimeStamp   = d->setting(QLatin1String("WriteMetadataUpdateFiletimeStamp")).toBool();
        meta.useXMPSidecar4Reading = d->setting(QLatin1String("UseXMPSidecar4Reading")).toBool();
        meta.metadataWritingMode   = (KPMetadata::MetadataWritingMode)d->setting(QLatin1String("MetadataWritingMode")).toInt();
    }

    qCDebug(KIPIPLUGINS_LOG) << meta;
    return meta;
}

}  // namespace KIPIPlugins
