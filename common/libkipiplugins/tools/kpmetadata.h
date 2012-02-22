/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-02-20
 * Description : Metadata interface for kipi-plugins.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

// LibKExiv2 includes

// NOTE: all file included here will be used into plugins as well, to prevent files to include from libkexiv2.
//       developpers just need to include kpmetadata.h, that all...
#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>
#include <libkexiv2/rotationmatrix.h>

// Local includes

#include "kipiplugins_export.h"

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
 *  with KIPI host application to prevent concurent operations on file during load and save operations.
 */
class KIPIPLUGINS_EXPORT KPMetadata : public KExiv2
{

public:

    /** Empty contructor. KIPI interface is null. Default metadata settings is used, 
     *  and no file lock will be performed with read and write operations.
     *  This contructor work like KExiv2 constructor.
     */
    KPMetadata();
    virtual ~KPMetadata();

    /** Constructor to load metadata from file. Same behavior than KPMetadata() constructor.
     */
    KPMetadata(const QString& filePath);

    /** Constructor with KIPI interface. Metadata settings is taken from KIPI host application and file 
     *  lock will be performed with read and write operations.
     *  If interface is null, default settings is used, and no file lock will be performed.
     *  With this constructor, just load metadata from file using load() method.
     */
    KPMetadata(Interface* const iface);

    /** Constructor to load metadata from file. Same behavior than KPMetadata(Interface* const iface) constructor.
     */
    KPMetadata(const QString& filePath, Interface* const iface);

    /** Copy Constructor.
     */
    KPMetadata(const KPMetadata& other);

    /** Assignement operator.
     */
    KPMetadata& operator=(const KPMetadata& other);
    
public:

    /** Apply metadata settings from KIPI host application to this interface. To use before load and save operations.
     */
    void setSettings(const KPMetaSettings& settings);

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

private:

    class KPMetadataPrivate;
    KPMetadataPrivate* d;
};

}  // namespace KIPIPlugins

#endif /* KPMETADATA_H */
