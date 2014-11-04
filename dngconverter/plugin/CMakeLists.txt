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

KDE4_ADD_APP_ICON(dngconverter_SRCS "../icons/*kipi-dngconverter.png")

KDE4_ADD_EXECUTABLE(dngconverter ${dngconverter_SRCS})

target_link_libraries(dngconverter
                      ${KDE4_KIO_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      kipiplugins
                      libdng
                     )

install(TARGETS  dngconverter         ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS dngconverter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )

# ========================================================================================
# Plugin

set(kipiplugin_dngconverter_PART_SRCS plugin_dngconverter.cpp ${dngconverter_common_SRCS})

KDE4_ADD_PLUGIN(kipiplugin_dngconverter ${kipiplugin_dngconverter_PART_SRCS})

target_link_libraries(kipiplugin_dngconverter
                      ${KDE4_KPARTS_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      kipiplugins
                      libdng
                     )

configure_file(kipiplugin_dngconverter.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dngconverter.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dngconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_dngconverter                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_dngconverterui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
