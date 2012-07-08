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

#ifndef XMPMM_H
#define XMPMM_H

// C++ includes

#include <vector>

// local includes

#include "kpmetadata.h"

using std::vector;
using namespace KIPIPlugins;

namespace KIPIPhotivoIntegrationPlugin
{

/* NOTE: As we only need to use known properties for this purpose we can use 
 * simple structs. This simple, fast and less error prone.
 * For more generic requirements a hash map could be used instead. */

// forward declarations for some data structs declared below
struct XmpMMHistory;
struct XmpMMDerivedFrom;

/*!****************************************************************************
 * Encapsulate and manage xmpMM data 
 */
class XmpMM
{

public:

    QString originalDocumentID;
    QString documentID;
    QString instanceID;
    vector<XmpMMDerivedFrom> derivedFrom;
    vector<XmpMMHistory>     history;

    XmpMM() = default;
    XmpMM(const KPMetadata meta) { load(meta); }

    void load(const KPMetadata meta);

private:

    void loadHistory(const KExiv2::MetaDataMap &mmMap);
    void loadIDs(const KExiv2::MetaDataMap &mmMap);

};

/*!****************************************************************************
 * Simple data-only struct to encapsulate a single xmpMM "history" item
 */
struct XmpMMHistory
{
    QString action;
    QString instanceID;
    QString when;
};

/*!****************************************************************************
 *Simple data-only struct to encapsulate a single xmpMM "derivedFrom" item
 */
struct XmpMMDerivedFrom
{
    QString documentID;
    QString instanceID;
};

} // namespace KIPIPhotivoIntegrationPlugin

#endif // XMPMM_H
