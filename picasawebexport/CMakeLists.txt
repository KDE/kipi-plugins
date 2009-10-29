SUBDIRS(icons)

SET(kipiplugin_picasawebexport_PART_SRCS
    mpform.cpp
    plugin_picasawebexport.cpp
    picasaweblogin.cpp
    picasawebtalker.cpp
    picasawebwidget.cpp
    picasawebwindow.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_picasawebexport_PART_SRCS uploadwidget.ui picasawebnewalbumdialog.ui)

KDE4_ADD_PLUGIN(kipiplugin_picasawebexport ${kipiplugin_picasawebexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_picasawebexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_picasawebexport
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_picasawebexport.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
