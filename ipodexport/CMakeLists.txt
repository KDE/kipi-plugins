ADD_DEFINITIONS(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

INCLUDE_DIRECTORIES(${GPOD_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR})

SET(kipiplugin_ipodexport_PART_SRCS
    plugin_ipodexport.cpp
    ipodexportdialog.cpp
    ipodheader.cpp
    ipodlistitem.cpp
    imagelist.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_ipodexport ${kipiplugin_ipodexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_ipodexport
                      ${GOBJECT_LIBRARIES}
                      ${GLIB2_LIBRARIES}
                      ${KDE4_KDE3SUPPORT_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${GPOD_LIBRARIES}
                      kipiplugins
                      kio
                     )

INSTALL(TARGETS kipiplugin_ipodexport DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_ipodexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
