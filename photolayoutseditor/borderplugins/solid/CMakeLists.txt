SET(photolayoutseditorborderplugin_solid_SRCS
         SolidBorderDrawer.cpp
         SolidBorderDrawer_p.cpp
   )

KDE4_ADD_PLUGIN(photolayoutseditorborderplugin_solid ${photolayoutseditorborderplugin_solid_SRCS})

TARGET_LINK_LIBRARIES(
    photolayoutseditorborderplugin_solid
    libphotolayoutseditor
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

INSTALL(TARGETS photolayoutseditorborderplugin_solid DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES photolayoutseditorborderplugin_solid.desktop DESTINATION ${SERVICES_INSTALL_DIR})
