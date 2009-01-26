SET(kipiplugin_fb_PART_SRCS
    plugin_fb.cpp
    fbwindow.cpp
    fbwidget.cpp
    fbalbum.cpp
    fbtalker.cpp
    mpform.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_fb ${kipiplugin_fb_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_fb
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

INSTALL(TARGETS kipiplugin_fb DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_fb.desktop DESTINATION ${SERVICES_INSTALL_DIR})
