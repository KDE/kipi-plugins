SUBDIRS(icons)

SET(kipiplugin_picasawebexport_PART_SRCS
    mpform.cpp
    plugin_picasawebexport.cpp
    picasaweblogin.cpp
    picasawebtalker.cpp
    picasawebwidget.cpp
    picasawebwindow.cpp
    picasawebalbum.cpp
    picasawebreplacedialog.cpp
    picasawebimglist.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_picasawebexport ${kipiplugin_picasawebexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_picasawebexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      kipiplugins
                     )

CONFIGURE_FILE(kipiplugin_picasawebexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_picasawebexport.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_picasawebexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_picasawebexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_picasawebexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
