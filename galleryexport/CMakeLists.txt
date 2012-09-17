SUBDIRS(icons)

SET(kipiplugin_galleryexport_PART_SRCS
    plugin_galleryexport.cpp
    albumdlg.cpp
    galleries.cpp
    galleryconfig.cpp
    gallerywindow.cpp
    gallerytalker.cpp
    gallerympform.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_galleryexport_PART_SRCS galleryalbumwidget.ui)

KDE4_ADD_PLUGIN(kipiplugin_galleryexport ${kipiplugin_galleryexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_galleryexport
                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      kipiplugins
                     )

CONFIGURE_FILE(kipiplugin_galleryexport.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_galleryexport.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_galleryexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_galleryexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_galleryexportui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
INSTALL(FILES   gallery_logo.png                                             DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_galleryexport/pics)
