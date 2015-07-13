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

#ifndef ALBUM_H
#define ALBUM_H

// Qt includes

#include <QDateTime>
#include <QString>
#include <QDebug>
#include <QMetaType>

namespace KIPIRajcePlugin
{

struct Album
{
    Album()
    {
        isHidden   = false;
        isSecure   = false;
        photoCount = 0;
        id         = 0;
    }

    bool      isHidden;
    bool      isSecure;

    unsigned  photoCount;
    unsigned  id;

    QString   name;
    QString   description;
    QString   url;
    QString   thumbUrl;
    QString   bestQualityThumbUrl;

    QDateTime createDate;
    QDateTime updateDate;
    QDateTime validFrom;
    QDateTime validTo;
};

} // namespace KIPIRajcePlugin

QTextStream& operator<<(QTextStream& str, const KIPIRajcePlugin::Album& a);
QDebug operator<<(QDebug d, const KIPIRajcePlugin::Album& a);

Q_DECLARE_METATYPE(KIPIRajcePlugin::Album)

#endif // ALBUMLISTCOMMAND_H
