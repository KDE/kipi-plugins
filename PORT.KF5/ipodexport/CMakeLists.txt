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

KDE4_ADD_PLUGIN(kipiplugin_ipodexport ${kipiplugin_ipodexport_PART_SRCS})

add_definitions(-DQT_NO_KEYWORDS)

target_link_libraries(kipiplugin_ipodexport
                      ${GOBJECT_LIBRARIES}
                      ${GLIB2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${IPOD_LIBRARIES}
                      ${GDK_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KIO_LIBRARY}
                      kipiplugins
                     )

configure_file(kipiplugin_ipodexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_ipodexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_ipodexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_ipodexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_ipodexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
