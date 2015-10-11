#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_yandexfotki\")

set(kipiplugin_yandexfotki_PART_SRCS
    plugin_yandexfotki.cpp
    yfwidget.cpp
    yfalbum.cpp
    yfphoto.cpp
    yftalker.cpp
    yfwindow.cpp
    yfalbumdialog.cpp
    yandexrsa.cpp
    yandexauth.cpp
)

add_library(kipiplugin_yandexfotki MODULE ${kipiplugin_yandexfotki_PART_SRCS})

target_link_libraries(kipiplugin_yandexfotki
    PRIVATE
        KF5::KDcraw
        KF5::Kipi
        KF5::KExiv2
        KF5::KIOCore
        KF5::WindowSystem
        KF5kipiplugins
)

configure_file(kipiplugin_yandexfotki.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_yandexfotki.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_yandexfotki.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_yandexfotki                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_yandexfotkiui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
