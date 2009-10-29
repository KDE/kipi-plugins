SUBDIRS(icons)

SET(kipiplugin_shwup_PART_SRCS
    plugin_shwup.cpp
    swwindow.cpp
    swlogin.cpp
    swalbum.cpp
    swconnector.cpp
    swwidget.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_shwup ${kipiplugin_shwup_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_shwup
                      ${KIPI_LIBRARIES}
                      ${KEXIV2_LIBRARIES}
                      ${KDCRAW_LIBRARIES}
                      ${QCA2_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      kipiplugins
                     )

INCLUDE_DIRECTORIES(
    ${QCA2_INCLUDE_DIR}
    )

ADD_DEFINITIONS(
    ${QCA2_DEFINITIONS}
    )

INSTALL(TARGETS kipiplugin_shwup DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES kipiplugin_shwup.desktop DESTINATION ${SERVICES_INSTALL_DIR})
