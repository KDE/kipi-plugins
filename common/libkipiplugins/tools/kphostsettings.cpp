/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : help wrapper around libkipi Interface to manage easily
 *               KIPI host application settings.
 *
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <libkipi/pluginloader.h>

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
    return d->setting("FileExtensions").toString();
}

bool KPHostSettings::hasFileExtensions() const
{
    return d->hasSetting("FileExtensions");
}

QString KPHostSettings::imageExtensions() const
{
    return d->setting("ImagesExtensions").toString();
}

bool KPHostSettings::hasImageExtensions() const
{
    return d->hasSetting("ImagesExtensions");
}

QString KPHostSettings::rawExtensions() const
{
    return d->setting("RawExtensions").toString();
}

bool KPHostSettings::hasRawExtensions() const
{
    return d->hasSetting("RawExtensions");
}

QString KPHostSettings::videoExtensions() const
{
    return d->setting("VideoExtensions").toString();
}

bool KPHostSettings::hasVideoExtensions() const
{
    return d->hasSetting("VideoExtensions");
}

QString KPHostSettings::audioExtensions() const
{
    return d->setting("AudioExtensions").toString();
}

bool KPHostSettings::hasAudioExtensions() const
{
    return d->hasSetting("AudioExtensions");
}

KIPIPlugins::KPMetaSettings KPHostSettings::metadataSettings() const
{
    KPMetaSettings meta; // if no valid data, default setting is returned.

    if (d->hasValidData())
    {
        meta.writeRawFiles         = d->setting("WriteMetadataToRAW").toBool();
        meta.updateFileTimeStamp   = d->setting("WriteMetadataUpdateFiletimeStamp").toBool();
        meta.useXMPSidecar4Reading = d->setting("UseXMPSidecar4Reading").toBool();
        meta.metadataWritingMode   = (KPMetadata::MetadataWritingMode)d->setting("MetadataWritingMode").toInt();
    }

    kDebug() << meta;
    return meta;
}

}  // namespace KIPIPlugins
