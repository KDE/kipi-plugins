#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${QJSON_INCLUDE_DIR})

set(kipiplugin_facebook_PART_SRCS
    plugin_facebook.cpp
    fbwindow.cpp
    fbwidget.cpp
    fbalbum.cpp
    fbtalker.cpp
    mpform.cpp
    facebookjob.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_facebook ${kipiplugin_facebook_PART_SRCS})

target_link_libraries(kipiplugin_facebook
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${QJSON_LIBRARIES}
                     )

configure_file(kipiplugin_facebook.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_facebook.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_facebook.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_facebook                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_facebookui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
