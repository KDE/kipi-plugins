/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-02-20
 * Description : Metadata interface for kipi-plugins.
 *
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kurl.h>

// LibKExiv2 includes

// NOTE: all file included here will be used into plugins as well, to prevent files to include from libkexiv2.
//       developpers just need to include kpmetadata.h, that all...
#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>
#include <libkexiv2/rotationmatrix.h>

// LibKipi includes

#include <libkipi/version.h>

// Local includes

#include "kipiplugins_export.h"

// Wrapper for KIPI file locker

#if KIPI_VERSION >= 0x010500
    #define KPFileReadLocker(iface, url) if (iface) FileReadLocker(iface, url);
#else
    #define KPFileReadLocker(iface, url)
#endif

#if KIPI_VERSION >= 0x010500
    #define KPFileWriteLocker(iface, url) if (iface) FileWriteLocker(iface, url);
#else
    #define KPFileWriteLocker(iface, url)
#endif

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

    // These methods have been factored to libkexiv2 2.3.0. Remove it after KDE 4.8.2
#if KEXIV2_VERSION < 0x020300
    static QString sidecarPath(const QString& path);
    /** Like KExiv2::sidecarFilePathForFile, but works for remote URLs */
    static KUrl sidecarUrl(const KUrl& url);
    /** Gives a file url for a local path */
    static KUrl sidecarUrl(const QString& path);
    /** Performs a QFileInfo based check if the given local file has a sidecar */
    static bool hasSidecar(const QString& path);
#endif // KEXIV2_VERSION < 0x020300

public:

    /** Move XMP sidecar file of source file src to destination dest.
     *  If src do not has an XMP sidecar, nothing is done and true is returned.
     *  If src has an XMP sidecar file, if moving is done successfully, true is returned, else false.
     */
    static bool moveSidecar(const KUrl& src, const KUrl& dest);

    /** Return true if url is a RAW file. File-name extension is used to check type mime.
     */
    static bool isRawFile(const KUrl& url);

private:

    void init();

private:

    Interface* m_iface;
};

}  // namespace KIPIPlugins

#endif /* KPMETADATA_H */
