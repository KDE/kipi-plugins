/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageInfo to manage easily
 *               item properties with KIPI host application.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KPIMAGEINFO_H
#define KPIMAGEINFO_H

// Qt includes

#include <QString>
#include <QStringList>

// KDE includes

#include <kurl.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPImageInfo
{

public:

    /** Contructor with KIPI interface instance get from plugin and item url that you want to manage.
     */
    KPImageInfo(KIPI::Interface* iface, const KUrl& url);
    ~KPImageInfo();

    /** Manage description (lead comment) of item.
     */
    void    setDescription(const QString& desc);
    QString description() const;

    /** Get keywords list (tag names) of item.
     */
    QStringList keywords() const;

    /** Manage rating (0-5 stars) of item.
     */
    void setRating(int r);
    int  rating() const;

private:

    class KPImageInfoPrivate;
    KPImageInfoPrivate* const d;
};

} // namespace KIPIPlugins

#endif  // KPIMAGEINFO_H
