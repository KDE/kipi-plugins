/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-07-01
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

// local includes

#include "xmpmm.h"

namespace KIPIPhotivoIntegrationPlugin
{

// public /////////////////////////////////////////////////////////////////////

void XmpMM::load(const KPMetadata& meta)
{
    // Check if XMP is available at all
    if (meta.hasXmp())
    {
        KExiv2::MetaDataMap mmMap = meta.getXmpTagsDataList(QStringList("xmpMM"));
        kDebug() << "mmMap.size():" << mmMap.size();

        loadIDs(mmMap);
        loadHistory(mmMap);
        //TODO: loadDerivedFrom()
    }
}

// ----------------------------------------------------------------------------

QString XmpMM::pureID (const QString& id) const
{
    int split = id.lastIndexOf (':');

    if (split < 0) //no ':' found => no prefix
        return id;

    else           //split prefix and id after last ':'
        return id.mid(split + 1);
}

// private ////////////////////////////////////////////////////////////////////

void XmpMM::loadHistory(const KExiv2::MetaDataMap& mmMap)
{
    // KPMetadata/KExiv2 can't tell how many elements an array has, 
    // so we try to access the next element in a loop until we fail.
    int i = 0;
    while (mmMap.contains(QString("Xmp.xmpMM.History[%1]").arg(++i)))
    {
        XmpMMHistory hist;
        QString      node  = QString("Xmp.xmpMM.History[%1]/stEvt:").arg(i);

        // Missing key/value pairs will result as an empty string
        hist.action     = mmMap[node + "action"];
        hist.instanceID = mmMap[node + "instanceID"];
        hist.when       = mmMap[node + "when"];
        history.push_back(hist);
    }
    kDebug() << "history.size():" << history.size();
}

// ----------------------------------------------------------------------------

void XmpMM::loadIDs(const KExiv2::MetaDataMap& mmMap)
{
    // Missing key/value pairs will result as an empty string
    documentID         = mmMap["Xmp.xmpMM.DocumentID"];
    instanceID         = mmMap["Xmp.xmpMM.InstanceID"];
    originalDocumentID = mmMap["Xmp.xmpMM.OriginalDocumentID"];
}

} // namespace KIPIPhotivoIntegrationPlugin
