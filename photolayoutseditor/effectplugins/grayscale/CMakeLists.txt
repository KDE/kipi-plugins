SET(photolayoutseditoreffectplugin_grayscale_SRCS
         GrayscalePhotoEffect.cpp
         GrayscalePhotoEffect_p.cpp
   )

KDE4_ADD_PLUGIN(photolayoutseditoreffectplugin_grayscale ${photolayoutseditoreffectplugin_grayscale_SRCS})

TARGET_LINK_LIBRARIES(
    photolayoutseditoreffectplugin_grayscale
    libphotolayoutseditor
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

INSTALL(TARGETS photolayoutseditoreffectplugin_grayscale DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES photolayoutseditoreffectplugin_grayscale.desktop DESTINATION ${SERVICES_INSTALL_DIR})
