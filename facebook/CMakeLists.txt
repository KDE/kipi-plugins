subdirs(icons)

SET(kipiplugin_facebook_PART_SRCS
    plugin_facebook.cpp
    fbwindow.cpp
    fbwidget.cpp
    fbalbum.cpp
    fbtalker.cpp
    mpform.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_facebook ${kipiplugin_facebook_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_facebook
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

INSTALL(TARGETS kipiplugin_facebook DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_facebook.desktop DESTINATION ${SERVICES_INSTALL_DIR})
