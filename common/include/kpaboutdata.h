//////////////////////////////////////////////////////////////////////////////
//
//    KPABOUTDATA.H
//
//    Copyright (C) 2006 Angelo Naselli <anaselli at linux dot it>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __KIPI_ABOUT_DATA__
#define __KIPI_ABOUT_DATA__

#include <kaboutdata.h>

namespace KIPIPlugins
{

  class KPAboutData : public KAboutData
  {
    private:
      QString KipiPluginsVersionString;

    public:

      KPAboutData(const char *pluginName,
                  const char *pluginVersion=0,
                  int licenseType=License_Unknown,
                  const char *pluginDescription=0,
                  const char *copyrightStatement="Copyright 2003-2006, kipi-plugins team");

  };

} // namespace KIPIPlugins

#endif //__KIPI_ABOUT_DATA__

