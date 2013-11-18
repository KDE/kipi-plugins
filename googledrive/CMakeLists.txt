SUBDIRS(icons)

INCLUDE_DIRECTORIES(${QJSON_INCLUDE_DIR})

SET(kipiplugin_googledrive_PART_SRCS
    mpform.cpp
    plugin_googledrive.cpp
    gdwidget.cpp
    gdwindow.cpp
    gdalbum.cpp
    gdtalker.cpp
    )

KDE4_ADD_PLUGIN(kipiplugin_googledrive ${kipiplugin_googledrive_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_googledrive
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QJSON_LIBRARIES}
                      kipiplugins
                     )

CONFIGURE_FILE(kipiplugin_googledrive.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_googledrive.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_googledrive.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_googledrive                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_googledriveui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
