set(kipiplugin_wallpaper_PART_SRCS plugin_wallpaper.cpp)

QT4_ADD_DBUS_INTERFACE(kipiplugin_wallpaper_PART_SRCS ${DBUS_INTERFACES_DIR}/org.kde.kdesktop.Background.xml  kdesktop_interface )

kde4_add_plugin(kipiplugin_wallpaper ${kipiplugin_wallpaper_PART_SRCS})

target_link_libraries(kipiplugin_wallpaper ${KDE4_KPARTS_LIBS} ${KIPI_LIBRARIES})

install(TARGETS kipiplugin_wallpaper DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kipiplugin_wallpaper.desktop DESTINATION ${SERVICES_INSTALL_DIR})
