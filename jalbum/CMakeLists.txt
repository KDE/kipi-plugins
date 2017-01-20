#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_jalbum\")

set(kipiplugin_jalbum_PART_SRCS
    jalbum.cpp
    jalbumconfig.cpp
    jalbumwindow.cpp
    plugin_jalbum.cpp
   )

add_library(kipiplugin_jalbum MODULE ${kipiplugin_jalbum_PART_SRCS})

target_link_libraries(kipiplugin_jalbum
                      Qt5::Network

                      KF5::Kipi
                      KF5::WindowSystem
                      KF5::I18n

                      KF5kipiplugins
                     )

configure_file(kipiplugin_jalbum.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_jalbum.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_jalbum.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_jalbum                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_jalbumui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
