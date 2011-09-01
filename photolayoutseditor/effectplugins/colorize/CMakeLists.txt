SET(photolayoutseditoreffectplugin_colorize_SRCS
         ColorizePhotoEffect.cpp
         ColorizePhotoEffect_p.cpp
   )

KDE4_ADD_PLUGIN(photolayoutseditoreffectplugin_colorize ${photolayoutseditoreffectplugin_colorize_SRCS})

TARGET_LINK_LIBRARIES(
    photolayoutseditoreffectplugin_colorize
    libphotolayoutseditor
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

INSTALL(TARGETS photolayoutseditoreffectplugin_colorize DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES photolayoutseditoreffectplugin_colorize.desktop DESTINATION ${SERVICES_INSTALL_DIR})
