/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-02-20
 * Description : Metadata interface for kipi-plugins.
 *
 * Copyright (C) 2012-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "kpmetadata.h"

// Qt includes

#include <QByteArray>
#include <QFile>
#include <QFileInfo>

// Local includes

#include "kpmetasettings.h"
#include "kphostsettings.h"

// LibKipi includes

#include <KIPI/Interface>
#include <KIPI/PluginLoader>

namespace KIPIPlugins
{

KPMetadata::KPMetadata()
    : KExiv2()
{
    init();
}

KPMetadata::KPMetadata(const QString& filePath)
    : KExiv2()
{
    init();
    load(filePath);
}

KPMetadata::~KPMetadata()
{
}

void KPMetadata::init()
{
    m_iface          = 0;
    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
    }

    KPHostSettings hset;
    setSettings(hset.metadataSettings());
}

void KPMetadata::setSettings(const KPMetaSettings& settings)
{
    setUseXMPSidecar4Reading(settings.useXMPSidecar4Reading);
    setWriteRawFiles(settings.writeRawFiles);
    setMetadataWritingMode(settings.metadataWritingMode);
    setUpdateFileTimeStamp(settings.updateFileTimeStamp);
}

KPMetaSettings KPMetadata::settings() const
{
    KPMetaSettings settings;

    settings.useXMPSidecar4Reading = useXMPSidecar4Reading();
    settings.writeRawFiles         = writeRawFiles();
    settings.metadataWritingMode   = (KPMetadata::MetadataWritingMode)metadataWritingMode();
    settings.updateFileTimeStamp   = updateFileTimeStamp();

    return settings;
}

bool KPMetadata::load(const QString& filePath) const
{
    KPFileReadLocker(m_iface, QUrl(filePath));

    return KExiv2::load(filePath);
}

bool KPMetadata::save(const QString& filePath) const
{
    KPFileWriteLocker(m_iface, QUrl(filePath));

    return KExiv2::save(filePath);
}

bool KPMetadata::applyChanges() const
{
    KPFileWriteLocker(m_iface, QUrl(getFilePath()));

    return KExiv2::applyChanges();
}

// -- Static Methods -------------------------------------------------------------------------

bool KPMetadata::moveSidecar(const QUrl& src, const QUrl& dst)
{
    if (hasSidecar(src.toLocalFile()))
    {
        if (QFile::rename(QString::fromUtf8(QFile::encodeName(sidecarUrl(src).toLocalFile())),
                          QString::fromUtf8(QFile::encodeName(sidecarUrl(dst).toLocalFile()))) != 0)
            return false;
    }

    return true;
}

}  // namespace KIPIPlugins
