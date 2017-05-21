#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_qmlimageviewer\")

add_subdirectory(data)

set(qmlimageviewer_SRCS qmlimageviewer_plugin.cpp
                       qmlimageviewer.cpp
)

add_library(kipiplugin_qmlimageviewer MODULE ${qmlimageviewer_SRCS})

target_link_libraries(kipiplugin_qmlimageviewer
                      PRIVATE
                      Qt5::Gui
                      Qt5::Core

                      KF5::Kipi
                      KF5::I18n
                      KF5::WindowSystem

                      KF5kipiplugins
)

configure_file(kipiplugin_qmlimageviewer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_qmlimageviewer.desktop)
install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_qmlimageviewer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_qmlimageviewer                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_qmlimageviewerui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
