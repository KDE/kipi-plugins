ADD_SUBDIRECTORY(icons)
ADD_SUBDIRECTORY(data)

IF(HUPNP_FOUND AND HUPNPAV_FOUND)

    INCLUDE_DIRECTORIES(${HUPNP_INCLUDE_DIR})

ELSE(HUPNP_FOUND AND HUPNPAV_FOUND)

    ADD_SUBDIRECTORY(extra/qtsoap)
    ADD_SUBDIRECTORY(extra/hupnp)
    ADD_SUBDIRECTORY(extra/hupnp_av)

    INCLUDE_DIRECTORIES(extra/hupnp/include)
    INCLUDE_DIRECTORIES(extra/hupnp_av/include)

ENDIF(HUPNP_FOUND AND HUPNPAV_FOUND)

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
                     )

IF(HUPNP_FOUND AND HUPNPAV_FOUND)
    TARGET_LINK_LIBRARIES(kipiplugin_dlnaexport
                          ${HUPNP_LIBS}
                          ${HUPNPAV_LIBS}
                         )

ELSE(HUPNP_FOUND AND HUPNPAV_FOUND)

    TARGET_LINK_LIBRARIES(kipiplugin_dlnaexport
                          HUpnp
                          HUpnpAv
                         )

ENDIF(HUPNP_FOUND AND HUPNPAV_FOUND)

INSTALL(TARGETS kipiplugin_dlnaexport
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_dlnaexport.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
