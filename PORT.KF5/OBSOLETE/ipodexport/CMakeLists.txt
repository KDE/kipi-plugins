#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${IPOD_INCLUDE_DIRS} ${GLIB2_INCLUDE_DIR} ${GDK_INCLUDE_DIR})

set(kipiplugin_ipodexport_PART_SRCS
    plugin_ipodexport.cpp
    uploaddialog.cpp
    ipodheader.cpp
    ipodlistitem.cpp
    imagelist.cpp
   )

add_library(kipiplugin_ipodexport MODULE ${kipiplugin_ipodexport_PART_SRCS})

add_definitions(-DQT_NO_KEYWORDS)

target_link_libraries(kipiplugin_ipodexport
                      ${GOBJECT_LIBRARIES}
                      ${GLIB2_LIBRARIES}
                      
                      ${IPOD_LIBRARIES}
                      ${GDK_LIBRARIES}
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KIOCore
                      KF5kipiplugins
                     )

configure_file(kipiplugin_ipodexport.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_ipodexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_ipodexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_ipodexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_ipodexportui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
