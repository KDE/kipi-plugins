#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

set(kipiplugin_smug_PART_SRCS
    plugin_smug.cpp
    smugwindow.cpp
    smugwidget.cpp
    smugalbum.cpp
    smugtalker.cpp
    mpform.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_smug ${kipiplugin_smug_PART_SRCS})

target_link_libraries(kipiplugin_smug
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      kipiplugins
                     )

configure_file(kipiplugin_smug.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_smug.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_smug.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_smug                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_smugui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
