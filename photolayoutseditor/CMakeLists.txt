  PROJECT( photolayoutseditor )

  #SET(CMAKE_INSTALL_PREFIX /usr/)

  CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

  ADD_SUBDIRECTORY( ./utils/qtpropertybrowser )

  FIND_PACKAGE(KDE4 REQUIRED)
  INCLUDE( KDE4Defaults )

  FIND_PACKAGE(Kipi REQUIRED)

  SET( CMAKE_PACKAGE_QTGUI TRUE )
  SET( QT_USE_QTXML TRUE )

<<<<<<< HEAD
  SET( PLE_Common_SRCS
      plugin/photolayoutseditor.cpp
      settings/PLEConfigDialog.cpp
      settings/PLEConfigViewWidget.cpp
      utils/global.cpp
      utils/KEditFactory.cpp
      events/UndoCommandEvent.cpp
      events/UndoCommandEventFilter.cpp
      listeners/TextColorChangeListener.cpp
      listeners/TextFontChangeListener.cpp
      undo/UndoMoveRowsCommand.cpp
)

  SET( PLE_Widgets_SRCS
      widgets/canvas/Canvas.cpp
      widgets/canvas/CanvasSize.cpp
      widgets/canvas/CropWidgetItem.cpp
      widgets/canvas/MousePressListener.cpp
      widgets/canvas/RotationWidgetItem.cpp
      widgets/canvas/ScalingWidgetItem.cpp
      widgets/canvas/Scene.cpp
      widgets/canvas/SceneBackground.cpp
      widgets/dialogs/CanvasSizeDialog.cpp
      widgets/dialogs/GridSetupDialog.cpp
      widgets/dialogs/ImageFileDialog.cpp
      widgets/extra/PatternsComboBox.cpp
      widgets/layers/LayersTree.cpp
      widgets/layers/LayersTreeDelegate.cpp
=======
  SET( PhotoLayoutsEditor_SRCS
      widgets/canvas/Scene.cpp
      widgets/canvas/Canvas.cpp
      widgets/canvas/RotationWidgetItem.cpp
      widgets/canvas/ScalingWidgetItem.cpp
      widgets/canvas/CropWidgetItem.cpp
      widgets/canvas/MousePressListener.cpp
      widgets/canvas/CanvasSize.cpp
      widgets/canvas/SceneBackground.cpp
      widgets/tools/EffectsEditorTool.cpp
      widgets/tools/ToolsDockWidget.cpp
      widgets/tools/BorderEditTool.cpp
      widgets/tools/TextEditorTool.cpp
      widgets/tools/CanvasEditTool.cpp
      widgets/tools/AbstractTool.cpp
      widgets/tools/AbstractItemsTool.cpp
      widgets/tools/AbstractItemsListViewTool.cpp
>>>>>>> a0bb2ddc52164d737c5d7ba1aa49cd3a08a36cb9
      widgets/items/AbstractPhoto.cpp
      widgets/items/AbstractItemInterface.cpp
      widgets/items/PhotoItem.cpp
      widgets/items/TextItem.cpp
<<<<<<< HEAD
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
)
=======
      widgets/dialogs/ImageFileDialog.cpp
      widgets/dialogs/CanvasSizeDialog.cpp
      widgets/dialogs/GridSetupDialog.cpp
      widgets/layers/LayersTree.cpp
      widgets/layers/LayersTreeDelegate.cpp
      widgets/menus/LayersTreeMenu.cpp
      widgets/extra/PatternsComboBox.cpp

      models/LayersModel.cpp
      models/LayersModelItem.cpp
      models/LayersSelectionModel.cpp
      models/AbstractMovableModel.cpp

      undo/UndoMoveRowsCommand.cpp

      plugin/photolayoutseditor.cpp

      utils/KEditFactory.cpp
      utils/global.cpp

      events/UndoCommandEvent.cpp
      events/UndoCommandEventFilter.cpp

      settings/PLEConfigDialog.cpp
      settings/PLEConfigViewWidget.cpp

      listeners/TextColorChangeListener.cpp
      listeners/TextFontChangeListener.cpp

      borders/BordersGroup.cpp
      borders/BorderDrawersLoader.cpp
      borders/BorderChangeListener.cpp

      effects/PhotoEffectsLoader.cpp
      effects/PhotoEffectsGroup.cpp
  )
>>>>>>> a0bb2ddc52164d737c5d7ba1aa49cd3a08a36cb9

  SET( PLE_Models_SRCS
      models/AbstractMovableModel.cpp
      models/LayersModel.cpp
      models/LayersModelItem.cpp
      models/LayersSelectionModel.cpp
)

  SET( PLE_Borders_SRCS
      borders/BorderChangeListener.cpp
      borders/BorderDrawersLoader.cpp
      borders/BordersGroup.cpp
)

  SET( PLE_Effects_SRCS
      effectplugins/StarndardEffectsFactory.cpp
      effectplugins/blur/BlurPhotoEffect.cpp
      effects/AbstractPhotoEffectInterface.cpp
      effects/PhotoEffectChangeListener.cpp
      effects/PhotoEffectsGroup.cpp
      effects/PhotoEffectsLoader.cpp
)

  ADD_SUBDIRECTORY( effectplugins )

  SET( PhotoLayoutsEditor_SRCS
      ${PLE_Common_SRCS}
      ${PLE_Widgets_SRCS}
      ${PLE_Models_SRCS}
      ${PLE_Borders_SRCS}
      ${PLE_Effects_SRCS}
)

  SET( PhotoLayoutsEditor_RCS
      resources/icons.qrc
  )

  QT4_ADD_RESOURCES( PhotoLayoutsEditor_SRCS ${PhotoLayoutsEditor_RCS} )

  SET( INCLUDE_DIRS
      borders
      borderplugins
      effects
      effectplugins
      effectplugins/blur
      effectplugins/colorize
      effectplugins/grayscale
      effectplugins/sepia
      events
      listeners
      models
      plugin
      settings
      undo
      utils
      utils/qtpropertybrowser/src
      widgets
      widgets/canvas
      widgets/dialogs
      widgets/extra
      widgets/items
      widgets/layers
      widgets/menus
      widgets/tools
  )

  INCLUDE_DIRECTORIES(${QT_INCLUDE}
                      ${KDE4_INCLUDES}
                      ${INCLUDE_DIRS}
                      ${KIPI_INCLUDE_DIR})

  ADD_SUBDIRECTORY( ./resources/app )

  KDE4_ADD_KCFG_FILES( PhotoLayoutsEditor_SRCS GENERATE_MOC ./settings/PLEConfigSkeleton.kcfgc )
  INSTALL( FILES ./settings/PLEConfigSkeleton.kcfgc DESTINATION ${KCFG_INSTALL_DIR} )

  # Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (B.K.O: #269903)
  FOREACH( _currentfile ${PhotoLayoutsEditor_SRCS} )
      IF(NOT MSVC)
          SET_SOURCE_FILES_PROPERTIES( ${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC" )
      ENDIF(NOT MSVC)
  ENDFOREACH( _currentfile ${PhotoLayoutsEditor_SRCS} )

  KDE4_ADD_LIBRARY( libphotolayoutseditor STATIC ${PhotoLayoutsEditor_SRCS} )

  SET_TARGET_PROPERTIES( libphotolayoutseditor PROPERTIES COMPILE_FLAGS "-fPIC" )

  TARGET_LINK_LIBRARIES( libphotolayoutseditor 
                                         PropertyBrowser
                                         ${QT_LIBRARIES}
                                         ${KDE4_KDEUI_LIBS}
                                         ${KDE4_KPARTS_LIBS}
                                         ${KIPI_LIBRARIES}
                                         kipiplugins )

# =======================================================
# PhotoLayoutsEditor plugin

  SET( kipiplugin_PhotoLayoutsEditor_PART_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/plugin/plugin_photolayoutseditor.cpp )

  KDE4_ADD_PLUGIN( kipiplugin_photolayoutseditor ${kipiplugin_PhotoLayoutsEditor_PART_SRCS} )

  TARGET_LINK_LIBRARIES( kipiplugin_photolayoutseditor libphotolayoutseditor )

  INSTALL( TARGETS kipiplugin_photolayoutseditor DESTINATION ${PLUGIN_INSTALL_DIR} )
  INSTALL( FILES plugin/kipiplugin_photolayoutseditor.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

# =======================================================
# Stand-alone version of PhotoLayoutsEditor

  SET( PhotoLayoutsEditor_SRCS main.cpp )

  KDE4_ADD_APP_ICON( PhotoLayoutsEditor_SRCS "photolayoutseditor" )

  KDE4_ADD_EXECUTABLE( photolayoutseditor ${PhotoLayoutsEditor_SRCS} )

  TARGET_LINK_LIBRARIES( photolayoutseditor libphotolayoutseditor )
  INSTALL( TARGETS photolayoutseditor ${INSTALL_TARGETS_DEFAULT_ARGS} )
  INSTALL( PROGRAMS plugin/photolayoutseditor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
  INSTALL( FILES plugin/photolayoutseditorui.rc DESTINATION ${DATA_INSTALL_DIR}/photolayoutseditor )
  INSTALL( FILES effects/photolayoutseditoreffectplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )
  INSTALL( FILES borders/photolayoutseditorborderplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )

<<<<<<< HEAD
  ADD_SUBDIRECTORY( ./borderplugins )

=======
# =======================================================
# Build plugins

  ADD_SUBDIRECTORY( ./borderplugins )
  ADD_SUBDIRECTORY( ./effectplugins )
>>>>>>> a0bb2ddc52164d737c5d7ba1aa49cd3a08a36cb9
