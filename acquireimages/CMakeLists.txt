#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_acquireimages\")

set(kipiplugin_acquireimages_PART_SRCS
    scandialog.cpp
    saveimgthread.cpp
    plugin_acquireimages.cpp
)

add_library(kipiplugin_acquireimages MODULE ${kipiplugin_acquireimages_PART_SRCS})

target_link_libraries(kipiplugin_acquireimages
                      KF5::Kipi
                      KF5::Sane
                      KF5::KExiv2
                      KF5kipiplugins
                     )

configure_file(kipiplugin_acquireimages.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_acquireimages.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_acquireimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_acquireimages                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_acquireimagesui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
