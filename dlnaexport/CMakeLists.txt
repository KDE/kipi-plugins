SUBDIRS(icons)

INCLUDE_DIRECTORIES(${HUPNP_INCLUDE_DIR})

SET(kipiplugin_dlnaexport_PART_SRCS
    dlnawidget.cpp
    plugin_dlnaexport.cpp
    dlnawindow.cpp
    mediaserver_window.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_dlnaexport ${kipiplugin_dlnaexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_dlnaexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_LIBRARIES}
                      ${QT_QTXMLPATTERNS_LIBRARY}
                      ${HUPNP_LIBS}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_dlnaexport
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_dlnaexport.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
