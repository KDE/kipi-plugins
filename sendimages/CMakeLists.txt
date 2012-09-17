
SET(kipiplugin_sendimages_PART_SRCS plugin_sendimages.cpp 
                                    sendimagesdialog.cpp 
                                    myimagelist.cpp
                                    settingswidget.cpp
                                    imageresize.cpp
                                    sendimages.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_sendimages ${kipiplugin_sendimages_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_sendimages
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      kipiplugins
                     )

CONFIGURE_FILE(kipiplugin_sendimages.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_sendimages.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_sendimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_sendimages                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_sendimagesui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
