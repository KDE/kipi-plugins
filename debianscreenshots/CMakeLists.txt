subdirs(icons)

SET(kipiplugin_debianscreenshots_PART_SRCS
    plugin_debianscreenshots.cpp
    dswindow.cpp
    dswidget.cpp
    dstalker.cpp
    packagedelegate.cpp
    debshots.h
    mpform.cpp
    kclickableimagelabel.cpp
#    dsjob.cpp
   )

QT4_ADD_RESOURCES(kipiplugin_debianscreenshots_QRC_SRCS plugin_debianscreenshots.qrc)

KDE4_ADD_PLUGIN(kipiplugin_debianscreenshots ${kipiplugin_debianscreenshots_PART_SRCS} ${kipiplugin_debianscreenshots_QRC_SRCS} )

TARGET_LINK_LIBRARIES(kipiplugin_debianscreenshots
                      kipiplugins
                      ${KIPI_LIBRARIES}
                      ${QJSON_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                     )

INSTALL(TARGETS kipiplugin_debianscreenshots         DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_debianscreenshots.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(FILES   kipiplugin_debianscreenshotsui.rc    DESTINATION ${DATA_INSTALL_DIR}/kipi)
