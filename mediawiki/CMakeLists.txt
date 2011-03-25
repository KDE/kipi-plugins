SUBDIRS(icons)

INCLUDE_DIRECTORIES(${MEDIAWIKI_INCLUDE_DIR})

KDE4_ADD_UI_FILES(ui_src wmwindow.ui)

SET(kipiplugin_wikimedia_PART_SRCS
    plugin_wikimedia.cpp
    wmwindow.cpp
    wmwidget.cpp
    wikimediajob.cpp
    wmwidget.cpp
    wmlogin.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_wikimedia ${kipiplugin_wikimedia_PART_SRCS} ${ui_src})

TARGET_LINK_LIBRARIES(kipiplugin_wikimedia
                      kipiplugins
                      mediawiki
                      ${MEDIAWIKI_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

INSTALL(TARGETS kipiplugin_wikimedia DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_wikimedia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
