SET(kipiplugin_wallpaper_PART_SRCS plugin_wallpaper.cpp)

QT4_ADD_DBUS_INTERFACE(kipiplugin_wallpaper_PART_SRCS ${DBUS_INTERFACES_INSTALL_DIR}/org.kde.kdesktop.Background.xml kdesktop_interface)

KDE4_ADD_PLUGIN(kipiplugin_wallpaper ${kipiplugin_wallpaper_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_wallpaper ${KDE4_KPARTS_LIBS} ${KIPI_LIBRARIES})

INSTALL(TARGETS kipiplugin_wallpaper DESTINATION ${PLUGIN_INSTALL_DIR})

INSTALL(FILES kipiplugin_wallpaper.desktop DESTINATION ${SERVICES_INSTALL_DIR})
