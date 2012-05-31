# ============================================================
#
# This file is a part of kipi-plugins project
# http://www.digikam.org
#
# Date        : 2012-02-16
# Description : an Hello World plugin.
#
# Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# ============================================================

SET(kipiplugin_helloworld_PART_SRCS plugin_helloworld.cpp)

KDE4_ADD_PLUGIN(kipiplugin_helloworld ${kipiplugin_helloworld_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_helloworld
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_helloworld DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_helloworld.desktop DESTINATION ${SERVICES_INSTALL_DIR})
