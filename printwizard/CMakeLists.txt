SET(kipiplugin_printwizard_SRCS
    cropframe.cpp
    wizard.cpp
    plugin_printwizard.cpp
    tphoto.cpp
    utils.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_printwizard_SRCS
                  intropage.ui
                  infopage.ui
                  photopage.ui
                  croppage.ui
                 )

KDE4_ADD_PLUGIN(kipiplugin_printwizard ${kipiplugin_printwizard_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_printwizard
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_printwizard
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_printwizard.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
