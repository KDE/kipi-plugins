#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_imageshack\")

set(kipiplugin_imageshack_PART_SRCS
    newalbumdlg.cpp
    imageshack.cpp
    mpform.cpp
    plugin_imageshack.cpp
    imageshacktalker.cpp
    imageshackwidget.cpp
    imageshackwindow.cpp
   )

add_library(kipiplugin_imageshack MODULE ${kipiplugin_imageshack_PART_SRCS})

target_link_libraries(kipiplugin_imageshack
                      PRIVATE
                      KF5::KDcraw
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KIOCore

                      KF5kipiplugins
                     )

configure_file(kipiplugin_imageshack.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imageshack.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imageshack.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_imageshack                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_imageshackui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
