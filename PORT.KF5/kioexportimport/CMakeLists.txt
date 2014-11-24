#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(PLUGIN_NAME kipiplugin_kioexportimport)

set(${PLUGIN_NAME}_PART_SRCS
    plugin_kioexportimport.cpp
    KioExportWindow.cpp
    KioExportWidget.cpp
    KioImportWindow.cpp
    KioImportWidget.cpp
   )

KDE4_ADD_PLUGIN(${PLUGIN_NAME} ${${PLUGIN_NAME}_PART_SRCS})

target_link_libraries(${PLUGIN_NAME}
                      ${KDE4_KDEUI_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KDE4_KIO_LIBRARY}
                      kipiplugins
                     )

configure_file(${PLUGIN_NAME}.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS ${PLUGIN_NAME}                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   ${PLUGIN_NAME}ui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
