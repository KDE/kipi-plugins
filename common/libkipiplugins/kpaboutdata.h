/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins shared library.
 * 
 * Copyright (C) 2006 Angelo Naselli <anaselli at linux dot it>
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

#ifndef __KIPI_ABOUT_DATA__
#define __KIPI_ABOUT_DATA__

// KDE includes.

#include <kaboutdata.h>

// Libkipi includes.

#include <libkipi/libkipi_export.h>

namespace KIPIPlugins
{

class LIBKIPI_EXPORT KPAboutData : public KAboutData
{
    private:
      QString KipiPluginsVersionString;

    public:

      KPAboutData(const char *pluginName,
                  const char *pluginVersion=0,
                  int licenseType=License_Unknown,
                  const char *pluginDescription=0,
                  const char *copyrightStatement="Copyright 2003-2007, kipi-plugins team");

};

} // namespace KIPIPlugins

#endif //__KIPI_ABOUT_DATA__
