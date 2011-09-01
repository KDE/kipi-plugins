SET(photolayoutseditoreffectplugin_sepia_SRCS
         SepiaPhotoEffect.cpp
         SepiaPhotoEffect_p.cpp
   )

KDE4_ADD_PLUGIN(photolayoutseditoreffectplugin_sepia ${photolayoutseditoreffectplugin_sepia_SRCS})

TARGET_LINK_LIBRARIES(
    photolayoutseditoreffectplugin_sepia
    libphotolayoutseditor
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

INSTALL(TARGETS photolayoutseditoreffectplugin_sepia DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES photolayoutseditoreffectplugin_sepia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
