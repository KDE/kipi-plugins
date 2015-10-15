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

#ifndef KPMETADATA_H
#define KPMETADATA_H

// Qt includes

#include <QUrl>

// LibKExiv2 includes

// NOTE: all file included here will be used into plugins as well, to prevent files to include from libkexiv2.
//       developpers just need to include kpmetadata.h, that all...
#include <libkexiv2_version.h>
#include <KExiv2/KExiv2>
#include <KExiv2/RotationMatrix>

// LibKipi includes

#include <libkipi_version.h>

// Local includes

#include "kipiplugins_export.h"

// Wrapper for KIPI file locker

#define KPFileReadLocker(iface, url) if (iface) FileReadLocker(iface, url);
#define KPFileWriteLocker(iface, url) if (iface) FileWriteLocker(iface, url);

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KExiv2Iface;

namespace KIPIPlugins
{

class KPMetaSettings;

/** This class must be used in plugins instead KExiv2 metadata interface to handle file lock mechanism
 *  with KIPI host application to prevent concurrent operations on file during load and save operations.
 */
class KIPIPLUGINS_EXPORT KPMetadata : public KExiv2
{

public:

    /** Standard contructor.
     *  KIPI interface from plugin loader instance is used to lock item.
     *  If no interface is available, for ex when plugin is loaded as stand-alone application,
     *  default metadata settings is used, and no file lock will be performed with
     *  read and write operations. In this case, this contructor work like KExiv2 constructor.
     */
    KPMetadata();
    virtual ~KPMetadata();

    /** Constructor to load metadata from file. Same behavior than KPMetadata() constructor.
     */
    KPMetadata(const QString& filePath);

public:

    /** Apply metadata settings from KIPI host application to this interface. To use before load and save operations.
     */
    void setSettings(const KPMetaSettings& settings);

    /** Return all metadata settings configured.
     */
    KPMetaSettings settings() const;

    /** Load metadata operation from a file.
     *  Re-implemented from libKexiv2 to use lock mechanism with KIPI host application through KIPI::Interface.
     */
    bool load(const QString& filePath) const;

    /** Save metadata operation to a file.
     *  Re-implemented from libKexiv2 to use lock mechanism with KIPI host application through KIPI::Interface.
     */
    bool save(const QString& filePath) const;

    /** Perform save metadata operation to current loaded file.
     *  Re-implemented from libKexiv2 to use lock mechanism with KIPI host application through KIPI::Interface.
     */
    bool applyChanges() const;

public:

    /** Move XMP sidecar file of source file src to destination dest.
     *  If src do not has an XMP sidecar, nothing is done and true is returned.
     *  If src has an XMP sidecar file, if moving is done successfully, true is returned, else false.
     */
    static bool moveSidecar(const QUrl& src, const QUrl& dest);

private:

    void init();

private:

    Interface* m_iface;
};

}  // namespace KIPIPlugins

#endif /* KPMETADATA_H */
