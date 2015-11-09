#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(PLUGIN_NAME kipiplugin_remotestorage)

add_definitions(-DTRANSLATION_DOMAIN=\"${PLUGIN_NAME}\")

set(${PLUGIN_NAME}_PART_SRCS
    plugin_remotestorage.cpp
    KioExportWindow.cpp
    KioExportWidget.cpp
    KioImportWindow.cpp
    KioImportWidget.cpp
   )

add_library(${PLUGIN_NAME} MODULE ${${PLUGIN_NAME}_PART_SRCS})

target_link_libraries(${PLUGIN_NAME}
                      PRIVATE
                      KF5::I18n
                      KF5::Kipi
                      KF5::KIOCore
                      KF5::KIOFileWidgets
                      KF5::WindowSystem

                      KF5kipiplugins
)

configure_file(${PLUGIN_NAME}.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS ${PLUGIN_NAME}                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   ${PLUGIN_NAME}ui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
