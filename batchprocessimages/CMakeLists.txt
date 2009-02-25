
find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT3_SUPPORT)

include(KDE4Defaults)

include(MacroLibrary)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(data)


########### next target ###############

set(kipiplugin_batchprocessimages_PART_SRCS
    plugin_batchprocessimages.cpp
    batchprocessimagesitem.cpp
    outputdialog.cpp
    imagepreview.cpp
    renameimagesdialog.cpp
    borderimagesdialog.cpp
    borderoptionsdialog.cpp
    convertimagesdialog.cpp
    convertoptionsdialog.cpp
    filterimagesdialog.cpp
    filteroptionsdialog.cpp
    colorimagesdialog.cpp
    coloroptionsdialog.cpp
    effectimagesdialog.cpp
    effectoptionsdialog.cpp
    recompressimagesdialog.cpp
    recompressoptionsdialog.cpp
    resizeimagesdialog.cpp
    resizeoptionsdialog.cpp
    batchprocessimagesdialog.cpp
    batchprocessimageslist.cpp
    renameimageswidget.cpp
    dialogutils.cpp
    )

kde4_add_ui_files(kipiplugin_batchprocessimages_PART_SRCS renameimagesbase.ui)

kde4_add_plugin(kipiplugin_batchprocessimages ${kipiplugin_batchprocessimages_PART_SRCS})

target_link_libraries(
    kipiplugin_batchprocessimages
    kipiplugins
    ${KIPI_LIBRARIES}
    ${KEXIV2_LIBRARIES}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kipiplugin_batchprocessimages DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  kipiplugin_batchprocessimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
