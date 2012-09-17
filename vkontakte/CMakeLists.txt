SUBDIRS(icons)

INCLUDE_DIRECTORIES(${LIBKVKONTAKTE_INCLUDES})

SET(kipiplugin_vkontakte_PART_SRCS
    plugin_vkontakte.cpp
    vkwindow.cpp
    vkalbumdialog.cpp
    albumchooserwidget.cpp
    authinfowidget.cpp
    vkapi.cpp
   )

KDE4_ADD_PLUGIN(kipiplugin_vkontakte ${kipiplugin_vkontakte_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_vkontakte
                      kipiplugins
                      ${KIPI_LIBRARIES}
#                      ${KDCRAW_LIBRARIES}
#                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${LIBKVKONTAKTE_LIBRARIES}
                     )

CONFIGURE_FILE(kipiplugin_vkontakte.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_vkontakte.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_vkontakte.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_vkontakte                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_vkontakteui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
