/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#include "album.h"

QTextStream& operator << (QTextStream& str, const KIPIRajceExportPlugin::Album& a)
{
    str << "Album[";
    str << "id=" << a.id;
    str << ", name='" << a.name << "'";
    str << ", description='" << a.description << "'";
    str << ", url='" << a.url << "'";
    str << ", createDate=" << a.createDate.toString(Qt::ISODate);
    str << ", updateDate=" << a.updateDate.toString(Qt::ISODate);
    str << ", validFrom=" << a.validFrom.toString(Qt::ISODate);
    str << ", validTo=" << a.validTo.toString(Qt::ISODate);
    str << ", isHidden=" << a.isHidden;
    str << ", isSecure=" << a.isSecure;
    str << ", photoCount=" << a.photoCount;
    str << ", thumbUrl='" << a.thumbUrl << "'";
    str << ", bestQualityThumbUrl='" << a.bestQualityThumbUrl << "'";
    str << "]";

    return str;
}

QDebug operator << (QDebug d, const KIPIRajceExportPlugin::Album& a)
{
    QString     s;
    QTextStream str(&s);

    str << a;

    d << *str.string();

    return d;
}
