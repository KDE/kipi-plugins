#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/importWizard)

set(kipiplugin_flashexport_PART_SRCS plugin_flashexport.cpp
                                     simpleviewer.cpp
                                     flashmanager.cpp
                                     importWizard/intropage.cpp
                                     importWizard/generalpage.cpp
                                     importWizard/lookpage.cpp
                                     importWizard/selectionpage.cpp
                                     importWizard/firstrunpage.cpp
                                     importWizard/importwizarddlg.cpp
                                     importWizard/progresspage.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_flashexport ${kipiplugin_flashexport_PART_SRCS})

target_link_libraries(kipiplugin_flashexport
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

configure_file(kipiplugin_flashexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_flashexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_flashexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_flashexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_flashexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

install(FILES data/space.png       DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport/simpleviewer_html)
install(FILES data/index.template  DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
install(FILES data/index2.template DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
install(FILES data/index3.template DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
install(FILES data/index4.template DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
