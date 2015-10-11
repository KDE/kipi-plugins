#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_smug\")

set(kipiplugin_smug_PART_SRCS
    plugin_smug.cpp
    mpform.cpp
    smugtalker.cpp
    smugalbum.cpp
    smugwidget.cpp
    smugwindow.cpp
   )

add_library(kipiplugin_smug MODULE ${kipiplugin_smug_PART_SRCS})

target_link_libraries(kipiplugin_smug
                      PRIVATE
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      KF5::KIOCore
                      KF5::WindowSystem
                      KF5kipiplugins
                     )

configure_file(kipiplugin_smug.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_smug.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_smug.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_smug                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_smugui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
