#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(icons)

set(kipiplugin_jalbumexport_PART_SRCS
    jalbum.cpp
    jalbumconfig.cpp
    jalbumwindow.cpp
    plugin_jalbumexport.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_jalbumexport ${kipiplugin_jalbumexport_PART_SRCS})

target_link_libraries(kipiplugin_jalbumexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_LIBRARIES}
                      ${QT_QTXMLPATTERNS_LIBRARY}
                      kipiplugins
                     )

configure_file(kipiplugin_jalbumexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_jalbumexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_jalbumexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_jalbumexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_jalbumexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
