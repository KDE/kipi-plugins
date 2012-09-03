SUBDIRS(icons)

SET(kipiplugin_piwigoexport_PART_SRCS
    plugin_piwigoexport.cpp
    piwigos.cpp
    piwigoconfig.cpp
    piwigowindow.cpp
    piwigotalker.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_piwigoexport ${kipiplugin_piwigoexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_piwigoexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_piwigoexport DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_piwigoexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(FILES kipiplugin_piwigoexportui.rc    DESTINATION ${DATA_INSTALL_DIR}/kipi)
INSTALL(FILES piwigo_logo.png DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_piwigoexport/pics)
