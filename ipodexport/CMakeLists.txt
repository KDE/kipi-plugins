
INCLUDE_DIRECTORIES(${GPOD_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR} ${GDK_INCLUDE_DIR})

SET(kipiplugin_ipodexport_PART_SRCS
    plugin_ipodexport.cpp
    IpodExportDialog.cpp
    IpodHeader.cpp
    IpodListItem.cpp
    ImageList.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_ipodexport ${kipiplugin_ipodexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_ipodexport
                      ${GOBJECT_LIBRARIES}
                      ${GLIB2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${GPOD_LIBRARIES}
                      ${GDK_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KDE4_KIO_LIBRARY}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_ipodexport DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_ipodexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
