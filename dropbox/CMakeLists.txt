SUBDIRS(icons)

INCLUDE_DIRECTORIES(${QJSON_INCLUDE_DIR})

SET(kipiplugin_dropbox_PART_SRCS
    mpform.cpp
    plugin_dropbox.cpp
    dbwidget.cpp
    dbwindow.cpp
    dbalbum.cpp
    dbtalker.cpp
    )

KDE4_ADD_PLUGIN(kipiplugin_dropbox ${kipiplugin_dropbox_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_dropbox
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QJSON_LIBRARIES}
                      kipiplugins
                     )

CONFIGURE_FILE(kipiplugin_dropbox.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dropbox.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dropbox.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_dropbox                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_dropboxui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
