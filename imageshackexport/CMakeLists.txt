#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

set(kipiplugin_imageshackexport_PART_SRCS
    plugin_imageshackexport.cpp
    imageshack.cpp
    imageshackwindow.cpp
    imageshackwidget.cpp
    imageshacktalker.cpp
    mpform.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_imageshackexport ${kipiplugin_imageshackexport_PART_SRCS})

target_link_libraries(kipiplugin_imageshackexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      kipiplugins
                     )

configure_file(kipiplugin_imageshackexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imageshackexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imageshackexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_imageshackexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_imageshackexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
