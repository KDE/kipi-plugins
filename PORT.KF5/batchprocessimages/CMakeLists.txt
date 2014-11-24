#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/tools
                    ${KDE4_INCLUDES}
                    ${KDE4_INCLUDE_DIR}
                    ${QT_INCLUDES}
                   )

set(kipiplugin_batchprocessimages_PART_SRCS
    plugin/plugin_batchprocessimages.cpp
    common/batchprocessimagesdialog.cpp
    common/batchprocessimagesitem.cpp
    common/batchprocessimageslist.cpp
    common/imagepreview.cpp
    tools/borderimagesdialog.cpp
    tools/borderoptionsdialog.cpp
    tools/colorimagesdialog.cpp
    tools/coloroptionsdialog.cpp
    tools/convertimagesdialog.cpp
    tools/convertoptionsdialog.cpp
    tools/effectimagesdialog.cpp
    tools/effectoptionsdialog.cpp
    tools/filterimagesdialog.cpp
    tools/filteroptionsdialog.cpp
    tools/recompressimagesdialog.cpp
    tools/recompressoptionsdialog.cpp
    tools/renameimagesdialog.cpp
    tools/renameimageswidget.cpp
    tools/resizecommandbuilder.cpp
    tools/resizeimagesdialog.cpp
    tools/resizeoptionsdialog.cpp
    )

KDE4_ADD_UI_FILES(kipiplugin_batchprocessimages_PART_SRCS
                  ui/batchprocessimagesdialog.ui
                  ui/imagepreview.ui
                  ui/renameimagesbase.ui
                 )

KDE4_ADD_PLUGIN(kipiplugin_batchprocessimages ${kipiplugin_batchprocessimages_PART_SRCS})

target_link_libraries(kipiplugin_batchprocessimages
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

configure_file(plugin/kipiplugin_batchprocessimages.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_batchprocessimages.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_batchprocessimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_batchprocessimages                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plugin/kipiplugin_batchprocessimagesui.rc                                  DESTINATION ${DATA_INSTALL_DIR}/kipi)
