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

#ifndef XMPINFO_H
#define XMPINFO_H

//local includes

#include "kpmetadata.h"
#include "xmpmm.h"

using namespace KIPIPlugins;

namespace KIPIPhotivoIntegrationPlugin
{

/** Access XMP-Metadata */
class XmpInfo //: protected KPMetadata //inherit protected to simplify the interface
{

public:

    QString isDerivate(const QString& image) const;
    XmpMM   getXmpMM(const QString& image) const;

private:

};

} // namespace KIPIPhotivoIntegrationPlugin

#endif // XMPINFO_H
