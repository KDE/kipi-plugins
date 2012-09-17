SUBDIRS(icons)

SET(kipiplugin_imgurexport_PART_SRCS
    plugin_imgurexport.cpp
    imgurtalker.cpp
    imgurwidget.cpp
    imgurwindow.cpp
    imgurimageslist.cpp
    mpform.cpp
)

KDE4_ADD_PLUGIN(kipiplugin_imgurexport ${kipiplugin_imgurexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_imgurexport
                      ${KIPI_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KEXIV2_LIBRARIES}
                      ${QJSON_LIBRARIES}
                      kipiplugins
                     )

CONFIGURE_FILE(kipiplugin_imgurexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imgurexport.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_imgurexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_imgurexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_imgurexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
