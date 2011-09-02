SET(photolayoutseditoreffectplugin_blur_SRCS
         BlurPhotoEffect.cpp
         BlurPhotoEffect_p.cpp
   )

SET( QT_USE_QTXML TRUE )

KDE4_ADD_PLUGIN(photolayoutseditoreffectplugin_blur ${photolayoutseditoreffectplugin_blur_SRCS})

TARGET_LINK_LIBRARIES(
    photolayoutseditoreffectplugin_blur
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    )

INSTALL(TARGETS photolayoutseditoreffectplugin_blur DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES photolayoutseditoreffectplugin_blur.desktop DESTINATION ${SERVICES_INSTALL_DIR})
