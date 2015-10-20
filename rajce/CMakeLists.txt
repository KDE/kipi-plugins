#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_rajce\")

set(kipiplugin_rajce_PART_SRCS
    mpform.cpp
    newalbumdialog.cpp
    sessionstate.cpp
    album.cpp
    rajcelogindialog.cpp
    rajcesession.cpp
    rajcewidget.cpp
    plugin_rajce.cpp
    rajcewindow.cpp
   )

add_library(kipiplugin_rajce MODULE ${kipiplugin_rajce_PART_SRCS})

target_link_libraries(kipiplugin_rajce
                      PRIVATE
                      Qt5::Core
                      Qt5::Xml
                      Qt5::XmlPatterns
                      
                      KF5::Kipi
                      KF5::KIOCore
                      KF5::KIOFileWidgets
                      KF5::WindowSystem

                      KF5kipiplugins
                     )

configure_file(kipiplugin_rajce.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_rajce.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_rajce.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_rajce                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_rajceui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
