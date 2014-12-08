#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

set(kipiplugin_piwigoexport_PART_SRCS
    plugin_piwigoexport.cpp
    piwigos.cpp
    piwigoconfig.cpp
    piwigowindow.cpp
    piwigotalker.cpp
   )

add_library(kipiplugin_piwigoexport MODULE ${kipiplugin_piwigoexport_PART_SRCS})

target_link_libraries(kipiplugin_piwigoexport
                      KF5::KDcraw
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KIOCore
                      
                      KF5kipiplugins
                     )

configure_file(kipiplugin_piwigoexport.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_piwigoexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_piwigoexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_piwigoexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_piwigoexportui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
install(FILES   piwigo_logo.png                                             DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_piwigoexport/pics)
