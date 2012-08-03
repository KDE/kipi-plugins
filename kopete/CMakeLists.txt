SET(kipiplugin_kopete_PART_SRCS
    ${kipiplugin_kopete_INTERFACE_SRCS}
    plugin_kopete.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_kopete ${kipiplugin_kopete_PART_SRCS} )

TARGET_LINK_LIBRARIES(kipiplugin_kopete
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

INSTALL(TARGETS kipiplugin_kopete DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_kopete.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(FILES kipiplugin_kopeteui.rc DESTINATION ${DATA_INSTALL_DIR}/kipi)
