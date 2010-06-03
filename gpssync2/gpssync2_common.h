/* ============================================================
 *
 * Date        : 2010-06-01
 * Description : A simple backend to search OSM and Geonames.org
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef GPSSYNC2_COMMON_H
#define GPSSYNC2_COMMON_H

#include <QString>

namespace KIPIGPSSyncPlugin
{

inline QString getKipiUserAgentName()
{
    return "KIPI-Plugins GPSSync - kde-imaging@kde.org";
}

} /* KIPIGPSSyncPlugin */

#endif /* GPSSYNC2_COMMON_H */
