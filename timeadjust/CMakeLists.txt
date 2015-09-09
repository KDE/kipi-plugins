#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_timeadjust\")

set(kipiplugin_timeadjust_PART_SRCS myimagelist.cpp
                                    plugin_timeadjust.cpp
                                    timeadjustdialog.cpp
                                    clockphotodialog.cpp
                                    actionthread.cpp
                                    task.cpp
                                    settingswidget.cpp
)

add_library(kipiplugin_timeadjust MODULE ${kipiplugin_timeadjust_PART_SRCS})

target_link_libraries(kipiplugin_timeadjust
                      KF5::Parts

                      ${KF5Kipi_LIBRARIES}
                      ${KF5KExiv2_LIBRARIES}
                      ${KF5KDcraw_LIBRARIES}

                      KF5kipiplugins
)

if(NOT ENABLE_PUREQT5)
    target_link_libraries(kipiplugin_timeadjust
                          KF5::KDELibs4Support
    )
endif()

configure_file(kipiplugin_timeadjust.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_timeadjust.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_timeadjust.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_timeadjust                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_timeadjustui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
