SET(kipiplugin_printimages_SRCS
    plugin_printimages.cpp
    printhelper.cpp
    tphoto.cpp
    cropframe.cpp
    wizard.cpp
    utils.cpp
   )

KDE4_ADD_UI_FILES(kipiplugin_printimages_SRCS
                  intropage.ui
                  infopage.ui
                  photopage.ui
                  croppage.ui
                  )

KDE4_ADD_PLUGIN(kipiplugin_printimages ${kipiplugin_printimages_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_printimages
                      ${KDE4_KPARTS_LIBS}
                      ${QT_AND_KDECORE_LIBS}
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_printimages
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_printimages.desktop kipiplugin_printassistant.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
