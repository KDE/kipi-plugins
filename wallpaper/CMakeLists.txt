#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(kipiplugin_wallpaper_PART_SRCS plugin_wallpaper.cpp)

KDE4_ADD_PLUGIN(kipiplugin_wallpaper ${kipiplugin_wallpaper_PART_SRCS})

target_link_libraries(kipiplugin_wallpaper ${KDE4_KPARTS_LIBS} ${KIPI_LIBRARIES})

configure_file(kipiplugin_wallpaper.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wallpaper.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wallpaper.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_wallpaper                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_wallpaperui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
