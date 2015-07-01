#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# ========================================================================================
# Stand alone version

set(dngconverter_SRCS dngconverter.cpp ${dngconverter_common_SRCS})

if(WIN32)
    remove_definitions(${KDE4_ENABLE_EXCEPTIONS})
endif()

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/*kipi-dngconverter.png")
ecm_add_app_icon(dngconverter_SRCS ICONS ${ICONS_SRCS})

add_executable(dngconverter ${dngconverter_SRCS})

target_link_libraries(dngconverter
                      KF5::KIOCore
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      ${CMAKE_THREAD_LIBS_INIT}
                      KF5kipiplugins
                      libdng
                     )

install(TARGETS  dngconverter         ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS dngconverter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )

# ========================================================================================
# Plugin

set(kipiplugin_dngconverter_PART_SRCS plugin_dngconverter.cpp ${dngconverter_common_SRCS})

add_library(kipiplugin_dngconverter MODULE ${kipiplugin_dngconverter_PART_SRCS})

target_link_libraries(kipiplugin_dngconverter
                      KF5::Parts
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      ${CMAKE_THREAD_LIBS_INIT}
                      KF5kipiplugins
                      libdng
                     )

configure_file(kipiplugin_dngconverter.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dngconverter.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dngconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_dngconverter                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_dngconverterui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
