#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_facebook\")

set(kipiplugin_facebook_PART_SRCS
    plugin_facebook.cpp
    fbwindow.cpp
    fbwidget.cpp
    fbalbum.cpp
    fbtalker.cpp
    mpform.cpp
   )

add_library(kipiplugin_facebook MODULE ${kipiplugin_facebook_PART_SRCS})

target_link_libraries(kipiplugin_facebook
                      PRIVATE
                      KF5kipiplugins
                      KF5::Kipi
                      KF5::I18n
                      KF5::KIOCore
                      KF5::KIOWidgets
                      KF5::WindowSystem
                     )

configure_file(kipiplugin_facebook.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_facebook.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_facebook.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_facebook                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_facebookui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
