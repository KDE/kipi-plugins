#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_subdirectory(icons)
add_subdirectory(data)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_dlnaexport\")

include_directories(extra/hupnp/include)
include_directories(extra/hupnp_av/include)

add_subdirectory(extra)

set(kipiplugin_dlnaexport_PART_SRCS
    finalpage.cpp
    plugin_dlnaexport.cpp
    hupnpmediaserver.cpp
    wizard.cpp
    welcomepage.cpp
   )

add_library(kipiplugin_dlnaexport MODULE ${kipiplugin_dlnaexport_PART_SRCS})

target_link_libraries(kipiplugin_dlnaexport
                      KF5::KDcraw
                      KF5::Kipi
                      KF5::KExiv2
                      KF5::I18n
                      KF5::KIOCore
                      Qt5::Core
                      Qt5::Xml
                      KF5kipiplugins
                     )

target_link_libraries(kipiplugin_dlnaexport
                      HUpnp
                      HUpnpAv
                     )

configure_file(kipiplugin_dlnaexport.desktop.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dlnaexport.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dlnaexport.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_dlnaexport                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_dlnaexportui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
