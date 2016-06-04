#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(icons)
add_subdirectory(data)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/importWizard)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_flashexport\")

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

add_library(kipiplugin_flashexport MODULE ${kipiplugin_flashexport_PART_SRCS})

target_link_libraries(kipiplugin_flashexport
                      PRIVATE
                      KF5::Archive
                      KF5::Kipi
                      KF5::I18n

                      KF5::KIOCore
                      KF5::JobWidgets

                      KF5kipiplugins
)

configure_file(kipiplugin_flashexport.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_flashexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_flashexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_flashexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_flashexportui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
