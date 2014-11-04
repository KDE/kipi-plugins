#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${MEDIAWIKI_INCLUDE_DIR})

KDE4_ADD_UI_FILES(ui_src wmwindow.ui)

set(kipiplugin_wikimedia_PART_SRCS
    plugin_wikimedia.cpp
    wmwindow.cpp
    wmwidget.cpp
    wikimediajob.cpp
    wmwidget.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_wikimedia ${kipiplugin_wikimedia_PART_SRCS} ${ui_src})

target_link_libraries(kipiplugin_wikimedia
                      kipiplugins
                      mediawiki
                      ${MEDIAWIKI_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

configure_file(kipiplugin_wikimedia.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wikimedia.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wikimedia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_wikimedia                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_wikimediaui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
