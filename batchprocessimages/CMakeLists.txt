SUBDIRS(icons)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common
                    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
                    ${CMAKE_CURRENT_SOURCE_DIR}/tools
                    ${KDE4_INCLUDES}
                    ${KDE4_INCLUDE_DIR}
                    ${QT_INCLUDES}
                   )

SET(kipiplugin_batchprocessimages_PART_SRCS
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

TARGET_LINK_LIBRARIES(
    kipiplugin_batchprocessimages
    kipiplugins
    ${KIPI_LIBRARIES}
    ${KEXIV2_LIBRARIES}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KDEUI_LIBS}
    )

INSTALL(TARGETS kipiplugin_batchprocessimages DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES plugin/kipiplugin_batchprocessimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(FILES plugin/kipiplugin_batchprocessimagesui.rc    DESTINATION ${DATA_INSTALL_DIR}/kipi)
