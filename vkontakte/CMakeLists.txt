#
# Copyright (c) 2010-2016, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_vkontakte\")

set(kipiplugin_vkontakte_PART_SRCS
    plugin_vkontakte.cpp
    vkwindow.cpp
    vkalbumdialog.cpp
    albumchooserwidget.cpp
    authinfowidget.cpp
   )

add_library(kipiplugin_vkontakte MODULE ${kipiplugin_vkontakte_PART_SRCS})

target_link_libraries(kipiplugin_vkontakte
                      PRIVATE
                      KF5::I18n
                      KF5::Kipi
                      KF5::WindowSystem

                      KF5kipiplugins

                      KF5::Vkontakte
)

configure_file(kipiplugin_vkontakte.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_vkontakte.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_vkontakte.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_vkontakte                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_vkontakteui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
