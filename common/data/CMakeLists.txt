#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# desktop file used to setup kipi-plugins handbook link and to share kipi-plugins version string.
configure_file(kipiplugins.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugins.desktop)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/kipiplugins.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

install(FILES tips DESTINATION ${DATA_INSTALL_DIR}/kipi)
