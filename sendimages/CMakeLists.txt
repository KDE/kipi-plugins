SET(kipiplugin_sendimages_PART_SRCS plugin_sendimages.cpp 
                                    sendimagesdialog.cpp 
                                    imagespage.cpp
                                    #listimageserrordialog.cpp 
                                    #sendimages.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_sendimages ${kipiplugin_sendimages_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_sendimages 
                      ${KDE4_KPARTS_LIBS} 
                      ${QT_AND_KDECORE_LIBS} 
                      ${KIPI_LIBRARIES} 
                      ${KEXIV2_LIBRARIES} 
                      ${KDCRAW_LIBRARIES} 
                      ${QT_QT3SUPPORT_LIBRARY} 
                      kipiplugins
                     )

KDE4_INSTALL_ICONS(${ICON_INSTALL_DIR})

INSTALL(TARGETS kipiplugin_sendimages DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_sendimages.desktop DESTINATION ${SERVICES_INSTALL_DIR})
