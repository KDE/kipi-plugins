#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

set(kipiplugin_picasawebexport_PART_SRCS
    mpform.cpp
    plugin_picasawebexport.cpp
    picasaweblogin.cpp
    picasawebtalker.cpp
    picasawebwidget.cpp
    picasawebwindow.cpp
    picasawebalbum.cpp
    picasawebreplacedialog.cpp
    picasawebimglist.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_picasawebexport ${kipiplugin_picasawebexport_PART_SRCS})

target_link_libraries(kipiplugin_picasawebexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      kipiplugins
                     )

configure_file(kipiplugin_picasawebexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_picasawebexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_picasawebexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_picasawebexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_picasawebexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
