subdirs(icons)

SET(kipiplugin_flashexport_PART_SRCS plugin_flashexport.cpp 
                                      firstrundlg.cpp
                                      selectionpage.cpp
                                      generalpage.cpp
                                      lookpage.cpp
                                      svedialog.cpp 
                                      simpleviewer.cpp 
   )

KDE4_ADD_PLUGIN(kipiplugin_flashexport ${kipiplugin_flashexport_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_flashexport 
                      ${KDE4_KPARTS_LIBS} 
                      ${KDE4_KIO_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_flashexport DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_flashexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})

INSTALL(FILES space.png      DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport/simpleviewer_html)
INSTALL(FILES index.template DESTINATION ${DATA_INSTALL_DIR}/kipiplugin_flashexport)
