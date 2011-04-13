SUBDIRS(icons)

INCLUDE(${QT_QTXMLPATTERNS_INCLUDE_DIR})

SET(kipiplugin_rajceexport_PART_SRCS
    mpform.cpp
    newalbumdialog.cpp
    sessionstate.cpp
    album.cpp
    rajcelogindialog.cpp
    rajcesession.cpp
    rajcewidget.cpp
    plugin_rajceexport.cpp
    rajcewindow.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_rajceexport ${kipiplugin_rajceexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_rajceexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_LIBRARIES}
                      ${QT_QTXMLPATTERNS_LIBRARY}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_rajceexport
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_rajceexport.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
