#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

set(kipiplugin_flickrexport_PART_SRCS
    plugin_flickrexport.cpp
    flickrwindow.cpp
    login.cpp
    flickrtalker.cpp
    flickrwidget.cpp
    flickrlist.cpp
    comboboxdelegate.cpp
    comboboxintermediate.cpp
    mpform.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_flickrexport_PART_SRCS flickralbumdialog.ui)

KDE4_ADD_PLUGIN(kipiplugin_flickrexport ${kipiplugin_flickrexport_PART_SRCS})

target_link_libraries(kipiplugin_flickrexport
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

configure_file(kipiplugin_flickrexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_flickrexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_flickrexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_flickrexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_flickrexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
