/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-02-13
 * Description : Metadata Settings Container for kipi-plugins.
 *
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPMETASETTINGS_H
#define KPMETASETTINGS_H

// Qt includes

#include <QMetaType>
#include <QDebug>

// Local includes

#include "kpmetadata.h"
#include "kipiplugins_export.h"

namespace KIPIPlugins
{

/**
    The class KPMetaSettings encapsulates all metadata related settings given 
    from KIPI host application through KIPI::Interface.
*/
class KIPIPLUGINS_EXPORT KPMetaSettings
{

public:

    KPMetaSettings();
    virtual ~KPMetaSettings();

public:

    /** Write metadata to RAW files.
     */
    bool                            writeRawFiles;

    /** Update file time-stamp when metadata are changed.
     */
    bool                            updateFileTimeStamp;

    /** Use XMP sidecar to read metadata of image.
     */
    bool                            useXMPSidecar4Reading;

    /** Metadata writing mode between file and XMP sidecar. See libkexiv2 library for details.
     */
    KPMetadata::MetadataWritingMode metadataWritingMode;
};

//! kDebug() stream operator. Writes property @a inf to the debug output in a nicely formatted way.
KIPIPLUGINS_EXPORT QDebug operator<<(QDebug dbg, const KPMetaSettings& inf);

}  // namespace KIPIPlugins

#endif  // KPMETASETTINGS_H
