SUBDIRS(icons)

SET(kipiplugin_smug_PART_SRCS
    plugin_smug.cpp
    smugwindow.cpp
    smugwidget.cpp
    smugalbum.cpp
    smugtalker.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_smug ${kipiplugin_smug_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_smug
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      kipiplugins
                     )

INSTALL(TARGETS kipiplugin_smug DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_smug.desktop DESTINATION ${SERVICES_INSTALL_DIR})
