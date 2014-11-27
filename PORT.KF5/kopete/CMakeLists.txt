#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(kipiplugin_kopete_PART_SRCS
    ${kipiplugin_kopete_INTERFACE_SRCS}
    plugin_kopete.cpp
   )

add_library(kipiplugin_kopete MODULE ${kipiplugin_kopete_PART_SRCS} )

target_link_libraries(kipiplugin_kopete
                      kipiplugins
                      KF5::Kipi
                      KF5::KIOCore
                      
                     )

configure_file(kipiplugin_kopete.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_kopete.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_kopete.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_kopete                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_kopeteui.rc                                DESTINATION ${DATA_INSTALL_DIR}/kipi)
