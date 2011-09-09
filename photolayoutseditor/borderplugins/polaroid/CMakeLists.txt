SET(photolayoutseditorborderplugin_polaroid_SRCS
         PolaroidBorderDrawer.cpp
         PolaroidBorderDrawer_p.cpp
   )

KDE4_ADD_PLUGIN(photolayoutseditorborderplugin_polaroid ${photolayoutseditorborderplugin_polaroid_SRCS})

TARGET_LINK_LIBRARIES(
    photolayoutseditorborderplugin_polaroid
    libphotolayoutseditor
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

INSTALL(TARGETS photolayoutseditorborderplugin_polaroid DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES photolayoutseditorborderplugin_polaroid.desktop DESTINATION ${SERVICES_INSTALL_DIR})
