#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_mediawiki\")


include_directories(${LIBMEDIAWIKI_INCLUDES})

set(kipiplugin_wikimedia_PART_SRCS
    plugin_wikimedia.cpp
    wmwidget.cpp
    wikimediajob.cpp
    wmwindow.cpp
   )

add_library(kipiplugin_wikimedia MODULE ${kipiplugin_wikimedia_PART_SRCS} ${ui_src})

target_link_libraries(kipiplugin_wikimedia
                      KF5::Kipi
                      KF5::I18n
                      KF5::KIOCore
                      KF5::WindowSystem

                      KF5kipiplugins

                      ${LIBMEDIAWIKI_LIBRARIES}
                     )

configure_file(kipiplugin_wikimedia.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wikimedia.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wikimedia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_wikimedia                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_wikimediaui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
