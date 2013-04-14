ADD_SUBDIRECTORY(icons)

SET(kipiplugin_jalbumexport_PART_SRCS
    jalbum.cpp
    jalbumconfig.cpp
    jalbumwindow.cpp
    plugin_jalbumexport.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_jalbumexport ${kipiplugin_jalbumexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_jalbumexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_LIBRARIES}
                      ${QT_QTXMLPATTERNS_LIBRARY}
                      kipiplugins
                     )

CONFIGURE_FILE(kipiplugin_jalbumexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_jalbumexport.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_jalbumexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_jalbumexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_jalbumexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
