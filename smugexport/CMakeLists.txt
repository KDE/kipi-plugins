SET(kipiplugin_smugexport_PART_SRCS
    plugin_smugexport.cpp
    smugwindow.cpp
    smugwidget.cpp
    smugalbum.cpp
    smugtalker.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_smugexport ${kipiplugin_smugexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_smugexport
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

INSTALL(TARGETS kipiplugin_smugexport DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_smugexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
