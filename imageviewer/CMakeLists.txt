#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons cursors)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_imageviewer\")

set(kipiplugin_imageviewer_PART_SRCS
    texture.cpp
    timer.cpp
    plugin_viewer.cpp
    viewerwidget.cpp
    helpdialog.cpp
   )

add_library(kipiplugin_imageviewer MODULE ${kipiplugin_imageviewer_PART_SRCS})

target_link_libraries(kipiplugin_imageviewer
                      PRIVATE
                      Qt5::OpenGL

                      KF5::Kipi
                      KF5kipiplugins

                      ${OPENGL_LIBRARIES}

                      # For Linux Only.
                      ${X11_LIBRARIES}
                      ${X11_Xrandr_LIB}
                     )

configure_file(kipiplugin_imageviewer.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imageviewer.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imageviewer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_imageviewer                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_imageviewerui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
