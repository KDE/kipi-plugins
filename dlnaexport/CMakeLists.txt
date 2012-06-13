ADD_SUBDIRECTORY(icons)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(extra/qtsoap)
ADD_SUBDIRECTORY(extra/hupnp)
ADD_SUBDIRECTORY(extra/hupnp_av)

#INCLUDE_DIRECTORIES(${HUPNP_INCLUDE_DIR})

INCLUDE_DIRECTORIES(extra/hupnp/include)
INCLUDE_DIRECTORIES(extra/hupnp_av/include)

SET(kipiplugin_dlnaexport_PART_SRCS
    dlnawidget.cpp
    plugin_dlnaexport.cpp
    dlnawindow.cpp
    mediaserver.cpp
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
                      kipiplugins

#                      ${HUPNP_LIBS}
#                      ${HUPNPAV_LIBS}

                      HUpnp
                      HUpnpAv
                     )

INSTALL(TARGETS kipiplugin_dlnaexport
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_dlnaexport.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
