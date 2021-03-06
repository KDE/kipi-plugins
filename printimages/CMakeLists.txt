#
# Copyright (c) 2010-2016, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_printimages\")


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
    wizard/printhelper.cpp
    wizard/customdlg.cpp
    wizard/printoptionspage.cpp
    wizard/printimagesconfig.cpp
    wizard/wizard.cpp
   )

ki18n_wrap_ui(kipiplugin_printimages_PART_SRCS
                  ui/photopage.ui
                  ui/croppage.ui
                  ui/customlayout.ui
                  ui/printoptionspage.ui
                 )

add_library(kipiplugin_printimages MODULE ${kipiplugin_printimages_PART_SRCS})

target_link_libraries(kipiplugin_printimages
                      PRIVATE
                      Qt5::Core
                      Qt5::PrintSupport
                      Qt5::Gui

                      KF5::Kipi
                      KF5::I18n

                      KF5kipiplugins
                     )

configure_file(plugin/kipiplugin_printimages.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_printimages.desktop)

install(FILES     ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_printimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS   kipiplugin_printimages                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES     plugin/kipiplugin_printimagesui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
install(DIRECTORY templates                                                         DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_printimages)
