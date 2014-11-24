#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

set(kipiplugin_rajceexport_PART_SRCS
    mpform.cpp
    newalbumdialog.cpp
    sessionstate.cpp
    album.cpp
    rajcelogindialog.cpp
    rajcesession.cpp
    rajcewidget.cpp
    plugin_rajceexport.cpp
    rajcewindow.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_rajceexport ${kipiplugin_rajceexport_PART_SRCS})

target_link_libraries(kipiplugin_rajceexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_LIBRARIES}
                      ${QT_QTXMLPATTERNS_LIBRARY}
                      kipiplugins
                     )

configure_file(kipiplugin_rajceexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_rajceexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_rajceexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_rajceexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_rajceexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
