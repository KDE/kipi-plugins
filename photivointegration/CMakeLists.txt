# ============================================================
#
# This file is a part of kipi-plugins project
# http://www.digikam.org
#
# Date        : 2012-06-11
# Description : Integration of the Photivo RAW-Processor.
#
# Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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
# ============================================================ */

ADD_SUBDIRECTORY(icons)

###REMOVE_DEFINITIONS(-DQT_NO_STL)

SET(kipiplugin_photivointegration_PART_SRCS plugin_photivointegration.cpp
                                            xmpinfo.cpp
)

KDE4_ADD_PLUGIN(kipiplugin_photivointegration ${kipiplugin_photivointegration_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_photivointegration
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_photivointegration DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_photivointegration.desktop DESTINATION ${SERVICES_INSTALL_DIR})
