SUBDIRS(icons)

FIND_PACKAGE(KDE4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)

INCLUDE_DIRECTORIES(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES})

SET(kipiplugin_batchprocessimages_PART_SRCS
    plugin_batchprocessimages.cpp
    batchprocessimagesdialog.cpp
    batchprocessimagesitem.cpp
    batchprocessimageslist.cpp
    outputdialog.cpp
    imagepreview.cpp
    dialogutils.cpp
    pixmapview.cpp
    resizecommandbuilder.cpp
    borderimagesdialog.cpp
    borderoptionsdialog.cpp
    colorimagesdialog.cpp
    coloroptionsdialog.cpp
    convertimagesdialog.cpp
    convertoptionsdialog.cpp
    effectimagesdialog.cpp
    effectoptionsdialog.cpp
    filterimagesdialog.cpp
    filteroptionsdialog.cpp
    recompressimagesdialog.cpp
    recompressoptionsdialog.cpp
    renameimagesdialog.cpp
    renameimageswidget.cpp
    resizeimagesdialog.cpp
    resizeoptionsdialog.cpp
    )

KDE4_ADD_UI_FILES(kipiplugin_batchprocessimages_PART_SRCS
                  batchprocessimagesdialog.ui
                  imagepreview.ui
                  renameimagesbase.ui
                 )

KDE4_ADD_PLUGIN(kipiplugin_batchprocessimages ${kipiplugin_batchprocessimages_PART_SRCS})

TARGET_LINK_LIBRARIES(
    kipiplugin_batchprocessimages
    ${KIPI_LIBRARIES}
    ${KEXIV2_LIBRARIES}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KDEUI_LIBS}
    kipiplugins
    )

INSTALL(TARGETS kipiplugin_batchprocessimages DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_batchprocessimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
