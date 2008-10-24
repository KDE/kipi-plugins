ADD_DEFINITIONS(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

SET(kipiplugin_picasawebexport_PART_SRCS
    plugin_picasawebexport.cpp
    picasawebwindow.cpp
    picasaweblogin.cpp
    picasawebtalker.cpp
    picasawebwidget.cpp
    mpform.cpp
    picasawebviewitem.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_picasawebexport_PART_SRCS uploadwidget.ui)

KDE4_ADD_UI3_FILES(kipiplugin_picasawebexport_PART_SRCS PicasawebNewAlbumDialog.ui)

KDE4_ADD_PLUGIN(kipiplugin_picasawebexport ${kipiplugin_picasawebexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_picasawebexport
                      kipiplugins
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDE3SUPPORT_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                     )

INSTALL(TARGETS kipiplugin_picasawebexport
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_picasawebexport.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
