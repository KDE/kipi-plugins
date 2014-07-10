# desktop file used to setup kipi-plugins handbook link and to share kipi-plugins version string.
CONFIGURE_FILE(kipiplugins.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugins.desktop)
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/kipiplugins.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

INSTALL(FILES tips DESTINATION ${DATA_INSTALL_DIR}/kipi)
