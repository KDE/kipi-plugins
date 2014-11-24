#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons cursors)

set(kipiplugin_imageviewer_PART_SRCS
    texture.cpp
    timer.cpp
    plugin_viewer.cpp
    viewerwidget.cpp
    helpdialog.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_imageviewer ${kipiplugin_imageviewer_PART_SRCS})

target_link_libraries(kipiplugin_imageviewer
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${QT_QTOPENGL_LIBRARY}
                      ${OPENGL_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}

                      # For Linux Only.
                      ${X11_LIBRARIES}
                      ${X11_Xrandr_LIB}

                      kipiplugins
                     )

configure_file(kipiplugin_imageviewer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imageviewer.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imageviewer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_imageviewer                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_imageviewerui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
