#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_imgur\")

set(kipiplugin_imgur_PART_SRCS
    plugin_imgur.cpp
    imgurtalker.cpp
    imgurwidget.cpp
    imgurwindow.cpp
    imgurimageslist.cpp
    mpform.cpp
   )

if(QTKOAUTH_FOUND)
    include_directories(${QTKOAUTH_INCLUDE_DIR})
    add_definitions(-DOAUTH_ENABLED ${QTKOAUTH_DEFINITIONS})
    set(kipiplugin_imgur_PART_SRCS ${kipiplugin_imgur_PART_SRCS} imgurtalkerauth.cpp)
endif()

add_library(kipiplugin_imgur MODULE ${kipiplugin_imgur_PART_SRCS})

target_link_libraries(kipiplugin_imgur
    PRIVATE
        KF5kipiplugins
        KF5::Kipi
        KF5::KIOCore
        KF5::WindowSystem
)

if(QTKOAUTH_FOUND)
    target_link_libraries(kipiplugin_imgur ${QTKOAUTH_LIBRARIES})
endif()

configure_file(kipiplugin_imgur.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imgur.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imgur.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_imgur                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_imgurui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
