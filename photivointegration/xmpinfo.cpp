/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-06-21
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// KDE includes

#include <kdebug.h>

// KIPI includes

#include "xmpinfo.h"
#include "xmpmm.h"

namespace KIPIPhotivoIntegrationPlugin
{

// public /////////////////////////////////////////////////////////////////////

QString XmpInfo::isDerivate(const QString& image) const
{
    const KPMetadata meta;

    if (meta.load(image))
    {
        // Read sidecar at first: Embedded metadata has precedence, so if both 
        // exist the doubled entries will be overwritten by embedded data later on
        if (meta.hasSidecar(image))
        {
            //TODO??? Needed? KExiv seams to take care of sidecars by itself on loading
        }

        XmpMM mm(meta);

        // Evaluate
        if (!mm.documentID.isEmpty() && !mm.originalDocumentID.isEmpty())
        {
            if (mm.documentID == mm.originalDocumentID) 
                 return "Original";

            return mm.documentID + " is derived from " + mm.originalDocumentID; //TODO:Mapping UUID -> filename
        }
    }
    else
    {
        kWarning() << "Loading Metadata for file" << image << "failed";
    }

    return QString("- unknown -");
}

// ----------------------------------------------------------------------------

XmpMM XmpInfo::getXmpMM(const QString& image) const
{
    const KPMetadata meta;
    XmpMM            mm;

    if (meta.load(image))
    {
        mm.load(meta);
    }
    else
    {
        kWarning() << "Loading Metadata for file" << image << "failed";
    }

    return mm;
}

// private ////////////////////////////////////////////////////////////////////

} // namespace KIPIPhotivoIntegrationPlugin
