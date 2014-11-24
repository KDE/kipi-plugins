#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(extra/qtpropertybrowser)
add_subdirectory(resources/app)
add_subdirectory(data)
add_subdirectory(effectplugins)
add_subdirectory(borderplugins)

set(CMAKE_PACKAGE_QTGUI TRUE)
set(QT_USE_QTXML        TRUE)

set(PLE_Common_SRCS
    plugin/photolayoutseditor.cpp
    settings/PLEConfigDialog.cpp
    settings/PLEConfigViewWidget.cpp
    extra/global.cpp
    extra/KEditFactory.cpp
    events/ProgressEvent.cpp
    events/UndoCommandEvent.cpp
    events/UndoCommandEventFilter.cpp
    listeners/TextColorChangeListener.cpp
    listeners/TextFontChangeListener.cpp
    undo/UndoMoveRowsCommand.cpp
   )

set(PLE_Widgets_SRCS
    widgets/canvas/Canvas.cpp
    widgets/canvas/CanvasSize.cpp
    widgets/canvas/CropWidgetItem.cpp
    widgets/canvas/MousePressListener.cpp
    widgets/canvas/RotationWidgetItem.cpp
    widgets/canvas/ScalingWidgetItem.cpp
    widgets/canvas/Scene.cpp
    widgets/canvas/SceneBackground.cpp
    widgets/canvas/SceneBorder.cpp
    widgets/dialogs/CanvasSizeDialog.cpp
    widgets/dialogs/GridSetupDialog.cpp
    widgets/dialogs/ImageFileDialog.cpp
    widgets/dialogs/NewCanvasDialog.cpp
    widgets/dialogs/CanvasSizeWidget.cpp
    widgets/dialogs/TemplatesModel.cpp
    widgets/dialogs/TemplatesView.cpp
    widgets/extra/PatternsComboBox.cpp
    widgets/extra/PLEStatusBar.cpp
    widgets/layers/LayersTree.cpp
    widgets/layers/LayersTreeDelegate.cpp
    widgets/items/AbstractPhoto.cpp
    widgets/items/AbstractPhoto_p.cpp
    widgets/items/PhotoItem.cpp
    widgets/items/TextItem.cpp
    widgets/menus/LayersTreeMenu.cpp
    widgets/items/AbstractItemInterface.cpp
    widgets/tools/AbstractItemsListViewTool.cpp
    widgets/tools/AbstractItemsTool.cpp
    widgets/tools/AbstractTool.cpp
    widgets/tools/BorderEditTool.cpp
    widgets/tools/CanvasEditTool.cpp
    widgets/tools/EffectsEditorTool.cpp
    widgets/tools/TextEditorTool.cpp
    widgets/tools/ToolsDockWidget.cpp
    widgets/tools/ZoomTool.cpp
   )

set(PLE_Models_SRCS
    models/AbstractMovableModel.cpp
    models/LayersModel.cpp
    models/LayersModelItem.cpp
    models/LayersSelectionModel.cpp
   )

set(PLE_Borders_SRCS
    borderplugins/StandardBordersFactory.cpp
    borderplugins/polaroid/PolaroidBorderDrawer.cpp
    borderplugins/solid/SolidBorderDrawer.cpp
    borders/BorderChangeListener.cpp
    borders/BorderDrawerInterface.cpp
    borders/BorderDrawersLoader.cpp
    borders/BordersGroup.cpp
   )

set(PLE_Effects_SRCS
    effectplugins/StandardEffectsFactory.cpp
    effectplugins/blur/BlurPhotoEffect.cpp
    effectplugins/colorize/ColorizePhotoEffect.cpp
    effectplugins/grayscale/GrayscalePhotoEffect.cpp
    effectplugins/sepia/SepiaPhotoEffect.cpp
    effectplugins/negative/NegativePhotoEffect.cpp
    effects/AbstractPhotoEffectInterface.cpp
    effects/PhotoEffectChangeListener.cpp
    effects/PhotoEffectsGroup.cpp
    effects/PhotoEffectsLoader.cpp
   )

set(PLE_Threads_SRCS
    threads/AbstractPhotoItemLoader.cpp
    threads/CanvasLoadingThread.cpp
    threads/CanvasSavingThread.cpp
    threads/ImageLoadingThread.cpp
    threads/PhotoItemLoader.cpp
    threads/SceneBackgroundLoader.cpp
    threads/SceneBorderLoader.cpp
    threads/TextItemLoader.cpp
   )

set(PhotoLayoutsEditor_SRCS
    ${PLE_Common_SRCS}
    ${PLE_Widgets_SRCS}
    ${PLE_Models_SRCS}
    ${PLE_Borders_SRCS}
    ${PLE_Effects_SRCS}
    ${PLE_Threads_SRCS}
   )

set(PhotoLayoutsEditor_RCS
    resources/icons.qrc
   )

QT4_ADD_RESOURCES(PhotoLayoutsEditor_SRCS ${PhotoLayoutsEditor_RCS})

set(INCLUDE_DIRS
    borders
    borderplugins
    borderplugins/polaroid
    borderplugins/solid
    effects
    effectplugins
    effectplugins/blur
    effectplugins/colorize
    effectplugins/grayscale
    effectplugins/sepia
    effectplugins/negative
    events
    listeners
    models
    plugin
    settings
    threads
    undo
    extra
    extra/qtpropertybrowser/src
    widgets
    widgets/canvas
    widgets/dialogs
    widgets/extra
    widgets/items
    widgets/layers
    widgets/menus
    widgets/tools
   )

include_directories(${QT_INCLUDE}
                    ${KDE4_INCLUDES}
                    ${INCLUDE_DIRS}
                    ${KIPI_INCLUDE_DIR}
                   )

KDE4_ADD_KCFG_FILES( PhotoLayoutsEditor_SRCS GENERATE_MOC ./settings/PLEConfigSkeleton.kcfgc )

# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (bug: #269903)
foreach(_currentfile ${PhotoLayoutsEditor_SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

if(NOT MSVC)
    # Causes cmake error on openSUSE 12 (cmake 2.8.6)
    #    set_target_properties( libphotolayoutseditor PROPERTIES COMPILE_FLAGS "-fPIC -fdata-sections -ffunction-sections" )
    #    set_target_properties( libphotolayoutseditor PROPERTIES LINK_FLAGS "--gc-sections" )
endif()

KDE4_ADD_LIBRARY( libphotolayoutseditor STATIC ${PhotoLayoutsEditor_SRCS} )

target_link_libraries(libphotolayoutseditor 
                      PropertyBrowser
                      ${QT_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KPARTS_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

install(FILES ./settings/photolayoutseditor.kcfg DESTINATION ${KCFG_INSTALL_DIR})

# =======================================================
# PhotoLayoutsEditor plugin

set(kipiplugin_PhotoLayoutsEditor_PART_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_photolayoutseditor.cpp)

KDE4_ADD_PLUGIN(kipiplugin_photolayoutseditor ${kipiplugin_PhotoLayoutsEditor_PART_SRCS})

target_link_libraries(kipiplugin_photolayoutseditor libphotolayoutseditor)

configure_file(plugin/kipiplugin_photolayoutseditor.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_photolayoutseditor.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/plugin/kipiplugin_photolayoutseditor.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_photolayoutseditor                                            DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   plugin/kipiplugin_photolayoutseditorui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)

# =======================================================
# Stand-alone version of PhotoLayoutsEditor

set(PhotoLayoutsEditor_SRCS plugin/main.cpp)

KDE4_ADD_APP_ICON(PhotoLayoutsEditor_SRCS "./resources/app/*photolayoutseditor.png")

KDE4_ADD_EXECUTABLE(photolayoutseditor ${PhotoLayoutsEditor_SRCS})

target_link_libraries(photolayoutseditor libphotolayoutseditor)

install(TARGETS photolayoutseditor ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS plugin/photolayoutseditor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES plugin/photolayoutseditorui.rc DESTINATION ${DATA_INSTALL_DIR}/photolayoutseditor)
install(FILES effects/photolayoutseditoreffectplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES borders/photolayoutseditorborderplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
