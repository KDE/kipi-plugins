
SET(kipiplugin_printimages_PART_SRCS
    plugin_printimages.cpp
    printhelper.cpp
    tphoto.cpp
    cropframe.cpp
    wizard.cpp
    utils.cpp
    printoptionspage.cpp
    layouttree.cpp
    atkinspagelayout.cpp 
   )

KDE4_ADD_UI_FILES(kipiplugin_printimages_PART_SRCS
                  intropage.ui
                  infopage.ui
                  photopage.ui
                  croppage.ui
                  printoptionspage.ui
                 )
KDE4_ADD_KCFG_FILES(kipiplugin_printimages_PART_SRCS
                    printimagesconfig.kcfgc
                   )

KDE4_ADD_PLUGIN(kipiplugin_printimages ${kipiplugin_printimages_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_printimages
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${KIPI_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_printimages
        DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_printimages.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
