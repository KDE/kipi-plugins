SUBDIRS(icons)

#find_package(LibKVkontakte 0.1.0 REQUIRED)
macro_log_feature(LIBKVKONTAKTE_INCLUDES "libkvkontakte" "KDE C++ library implementing VKontakte.ru Open API" "http://" TRUE)

set(kipiplugin_vkontakte_PART_SRCS
    plugin_vkontakte.cpp
    vkwindow.cpp
    vkalbumdialog.cpp
    albumchooserwidget.cpp
    vkapi.cpp
   )

kde4_add_plugin(kipiplugin_vkontakte ${kipiplugin_vkontakte_PART_SRCS})

target_link_libraries(kipiplugin_vkontakte
                      kipiplugins
#                      ${KDCRAW_LIBRARIES}
                      ${KIPI_LIBRARIES}
#                      ${KEXIV2_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${LIBKVKONTAKTE_LIBRARIES}
                     )

include_directories(${LIBKVKONTAKTE_INCLUDES})

install(TARGETS kipiplugin_vkontakte
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kipiplugin_vkontakte.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
