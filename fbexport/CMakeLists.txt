SET(kipiplugin_fbexport_PART_SRCS
    plugin_fbexport.cpp
    fbwindow.cpp
    fbwidget.cpp
    fbalbum.cpp
    fbtalker.cpp
    mpform.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_fbexport ${kipiplugin_fbexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_fbexport
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

INSTALL(TARGETS kipiplugin_fbexport DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_fbexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
