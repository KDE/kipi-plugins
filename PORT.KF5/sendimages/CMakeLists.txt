#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(kipiplugin_sendimages_PART_SRCS plugin_sendimages.cpp
                                    sendimagesdialog.cpp
                                    myimagelist.cpp
                                    settingswidget.cpp
                                    imageresize.cpp
                                    sendimages.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_sendimages ${kipiplugin_sendimages_PART_SRCS})

target_link_libraries(kipiplugin_sendimages
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

configure_file(kipiplugin_sendimages.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_sendimages.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_sendimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_sendimages                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_sendimagesui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
