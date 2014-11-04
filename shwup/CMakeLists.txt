#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${QCA2_INCLUDE_DIR})

add_definitions(${QCA2_DEFINITIONS})

set(kipiplugin_shwup_PART_SRCS
    plugin_shwup.cpp
    swwindow.cpp
    swlogin.cpp
    swalbum.cpp
    swconnector.cpp
    swwidget.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_shwup ${kipiplugin_shwup_PART_SRCS})

target_link_libraries(kipiplugin_shwup
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${QCA2_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      kipiplugins
                     )

configure_file(kipiplugin_shwup.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_shwup.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_shwup.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_shwup                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_shwupui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
