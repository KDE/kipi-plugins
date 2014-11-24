#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(icons)

###remove_definitions(-DQT_NO_STL)

set(kipiplugin_photivointegration_PART_SRCS xmpmm.cpp
                                            xmpinfo.cpp
                                            piwindow.cpp
                                            piimglist.cpp
                                            piwidget.cpp
                                            plugin_photivointegration.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_photivointegration ${kipiplugin_photivointegration_PART_SRCS})

target_link_libraries(kipiplugin_photivointegration
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

configure_file(kipiplugin_photivointegration.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_photivointegration.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_photivointegration.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_photivointegration                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_photivointegrationui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
