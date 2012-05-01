/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2006-2010 Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2010-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPABOUTDATA_H
#define KPABOUTDATA_H

// KDE includes

#include <kaboutdata.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPAboutData : public KAboutData
{

public:

    explicit KPAboutData(const KLocalizedString& pluginName,
                        const QByteArray& pluginVersion            = QByteArray(),
                        enum  LicenseKey licenseType               = License_Unknown,
                        const KLocalizedString& pluginDescription  = KLocalizedString(),
                        const KLocalizedString& copyrightStatement = ki18n("Copyright 2003-2012, kipi-plugins team"));

    KPAboutData(const KPAboutData& other);

public:

    QString handbookEntry;
};

} // namespace KIPIPlugins

#endif //KPABOUTDATA_H
