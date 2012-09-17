SUBDIRS(icons)

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

CONFIGURE_FILE(kipiplugin_rajceexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_rajceexport.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_rajceexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_rajceexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_rajceexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
