#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

set(kipiplugin_galleryexport_PART_SRCS
    plugin_galleryexport.cpp
    albumdlg.cpp
    galleries.cpp
    galleryconfig.cpp
    gallerywindow.cpp
    gallerytalker.cpp
    gallerympform.cpp
   )

ki18n_wrap_ui(kipiplugin_galleryexport_PART_SRCS galleryalbumwidget.ui)

add_library(kipiplugin_galleryexport MODULE ${kipiplugin_galleryexport_PART_SRCS})

target_link_libraries(kipiplugin_galleryexport
                      KF5::KDcraw
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KIOCore
                      
                      KF5kipiplugins
                     )

configure_file(kipiplugin_galleryexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_galleryexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_galleryexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_galleryexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_galleryexportui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
install(FILES   gallery_logo.png                                             DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_galleryexport/pics)
