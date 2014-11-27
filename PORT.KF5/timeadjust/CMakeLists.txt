#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

set(kipiplugin_timeadjust_PART_SRCS plugin_timeadjust.cpp
                                    timeadjustdialog.cpp
                                    clockphotodialog.cpp
                                    actionthread.cpp
                                    task.cpp
                                    myimagelist.cpp
                                    settingswidget.cpp
   )

add_library(kipiplugin_timeadjust MODULE ${kipiplugin_timeadjust_PART_SRCS})

target_link_libraries(kipiplugin_timeadjust
                      KF5::Parts
                      KF5::ThreadWeaver
                      
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::KDcraw
                      kipiplugins
                     )

configure_file(kipiplugin_timeadjust.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_timeadjust.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_timeadjust.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_timeadjust                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_timeadjustui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
