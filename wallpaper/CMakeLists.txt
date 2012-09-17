SET(kipiplugin_wallpaper_PART_SRCS plugin_wallpaper.cpp)

KDE4_ADD_PLUGIN(kipiplugin_wallpaper ${kipiplugin_wallpaper_PART_SRCS})

TARGET_LINK_LIBRARIES(kipiplugin_wallpaper ${KDE4_KPARTS_LIBS} ${KIPI_LIBRARIES})

CONFIGURE_FILE(kipiplugin_wallpaper.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wallpaper.desktop)

INSTALL(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_wallpaper.desktop DESTINATION ${SERVICES_INSTALL_DIR})
INSTALL(TARGETS kipiplugin_wallpaper                                     DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kipiplugin_wallpaperui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
