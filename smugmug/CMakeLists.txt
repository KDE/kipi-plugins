SET(kipiplugin_smugmug_PART_SRCS
    smugmugtalker.cpp
    plugin_smugmug.cpp
    smugmugwindow.cpp
    smugmugwidget.cpp
    smugmugalbum.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_smugmug ${kipiplugin_smugmug_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_smugmug
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

INSTALL(TARGETS kipiplugin_smugmug DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_smugmug.desktop DESTINATION ${SERVICES_INSTALL_DIR})
