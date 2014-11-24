#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${QJSON_INCLUDE_DIR})

set(kipiplugin_dropbox_PART_SRCS
    mpform.cpp
    plugin_dropbox.cpp
    dbwidget.cpp
    dbwindow.cpp
    dbalbum.cpp
    dbtalker.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_dropbox ${kipiplugin_dropbox_PART_SRCS})

target_link_libraries(kipiplugin_dropbox
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QJSON_LIBRARIES}
                      kipiplugins
                     )

configure_file(kipiplugin_dropbox.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dropbox.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dropbox.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_dropbox                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_dropboxui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
