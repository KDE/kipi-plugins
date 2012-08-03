
INCLUDE_DIRECTORIES(${IPOD_INCLUDE_DIRS} ${GLIB2_INCLUDE_DIR} ${GDK_INCLUDE_DIR})

SET(kipiplugin_ipodexport_PART_SRCS
    plugin_ipodexport.cpp
    uploaddialog.cpp
    ipodheader.cpp
    ipodlistitem.cpp
    imagelist.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_ipodexport ${kipiplugin_ipodexport_PART_SRCS})
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

TARGET_LINK_LIBRARIES(kipiplugin_ipodexport
                      ${GOBJECT_LIBRARIES}
                      ${GLIB2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${IPOD_LIBRARIES}
                      ${GDK_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KIO_LIBRARY}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_ipodexport DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_ipodexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(FILES kipiplugin_ipodexportui.rc    DESTINATION ${DATA_INSTALL_DIR}/kipi)
