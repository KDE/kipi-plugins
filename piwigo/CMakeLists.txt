#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_piwigo\")

set(kipiplugin_piwigo_PART_SRCS
    plugin_piwigo.cpp
    piwigos.cpp
    piwigoconfig.cpp
    piwigowindow.cpp
    piwigotalker.cpp
   )

add_library(kipiplugin_piwigo MODULE ${kipiplugin_piwigo_PART_SRCS})

target_link_libraries(kipiplugin_piwigo
                      PRIVATE
                      KF5::KDcraw
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KIOCore

                      KF5kipiplugins
                     )

configure_file(kipiplugin_piwigo.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_piwigo.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_piwigo.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_piwigo                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_piwigoui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
install(FILES   piwigo_logo.png                                       DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_piwigo/pics)
