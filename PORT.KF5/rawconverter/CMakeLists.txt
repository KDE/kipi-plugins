#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/manager
                    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
                    ${JPEG_INCLUDE_DIR}
                    ${PNG_PNG_INCLUDE_DIR}
                   )

set(kipiplugin_rawconverter_PART_SRCS plugin/plugin_rawconverter.cpp
                                      manager/rawdecodingiface.cpp
                                      manager/actionthread.cpp
                                      manager/task.cpp
                                      dialogs/batchdialog.cpp
                                      dialogs/singledialog.cpp
                                      dialogs/myimagelist.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_rawconverter ${kipiplugin_rawconverter_PART_SRCS})

target_link_libraries(kipiplugin_rawconverter
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

configure_file(plugin/kipiplugin_rawconverter.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_rawconverter.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_rawconverter.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_rawconverter                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_rawconverterui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
