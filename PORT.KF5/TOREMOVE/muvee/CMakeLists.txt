#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_muvee\")

include_directories(${Qca-qt5_INCLUDEDIR})

set(kipiplugin_muvee_PART_SRCS
    plugin_muvee.cpp
    swwindow.cpp
    swlogin.cpp
    swalbum.cpp
    swwidget.cpp
    swconnector.cpp
   )

add_library(kipiplugin_muvee MODULE ${kipiplugin_muvee_PART_SRCS})

target_link_libraries(kipiplugin_muvee
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      ${Qca-qt5_LIBRARY}
                      KF5::KIOCore
                      KF5kipiplugins
                     )

configure_file(kipiplugin_muvee.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_muvee.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_muvee.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_muvee                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_muveeui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
