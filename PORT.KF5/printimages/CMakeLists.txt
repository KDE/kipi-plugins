#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/wizard
                    ${CMAKE_CURRENT_SOURCE_DIR}/tools
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                   )

set(kipiplugin_printimages_PART_SRCS
    plugin/plugin_printimages.cpp
    tools/tphoto.cpp
    tools/utils.cpp
    tools/atkinspagelayout.cpp
    tools/templateicon.cpp
    tools/layouttree.cpp
    wizard/cropframe.cpp
    wizard/wizard.cpp
    wizard/printoptionspage.cpp
    wizard/printhelper.cpp
    wizard/customdlg.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_printimages_PART_SRCS
                  ui/photopage.ui
                  ui/croppage.ui
                  ui/customlayout.ui
                  ui/printoptionspage.ui
                 )

KDE4_ADD_KCFG_FILES(kipiplugin_printimages_PART_SRCS
                    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/printimagesconfig.kcfgc
                   )

KDE4_ADD_PLUGIN(kipiplugin_printimages ${kipiplugin_printimages_PART_SRCS})

target_link_libraries(kipiplugin_printimages
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

configure_file(plugin/kipiplugin_printimages.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_printimages.desktop)

install(FILES     ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_printimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS   kipiplugin_printimages                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES     plugin/kipiplugin_printimagesui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
install(FILES     TEMPLATE_HOWTO                                                    DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_printimages)
install(DIRECTORY templates                                                         DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_printimages)
